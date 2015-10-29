/**************************************************************************
 *	HyOpenGL_Win.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Harmony/Renderer/OpenGL/Interop/HyOpenGL_Win.h"

#include "GuiTool/HyGuiComms.h"
#include "Utilities/HyStrManip.h"

#include "Renderer/Viewport/HyWindow.h"

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HGLRC	HyOpenGL_Win::DeviceContext::sm_hGLContext;

HyOpenGL_Win::HyOpenGL_Win(HyGfxComms &gfxCommsRef, vector<HyWindow> &viewportsRef) : HyOpenGL(gfxCommsRef, viewportsRef)
{
	DeviceContext::sm_hGLContext = NULL;

	m_uiNumDCs = static_cast<uint32>(m_vWindowRef.size());
	m_ppDeviceContexes = new DeviceContext *[m_uiNumDCs];

	for(uint32 i = 0; i < m_uiNumDCs; ++i)
		m_ppDeviceContexes[i] = new DeviceContext(m_vWindowRef[i].GetWindowInfo());

	vector<HyMonitorDeviceInfo> vMonitorDeviceInfo;
	if(EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&vMonitorDeviceInfo) == false)
		HyLogError("EnumDisplayMonitors failed");

	HyWindow::SetMonitorDeviceInfo(vMonitorDeviceInfo);

	if(HyOpenGL::Initialize() == false)
		HyError("OpenGL API's Initialize() failed");
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	vector<HyMonitorDeviceInfo> &vMonitorDeviceInfo = *reinterpret_cast<vector<HyMonitorDeviceInfo> *>(dwData);

	MONITORINFOEX monitorInfo;
	memset(&monitorInfo, 0, sizeof(monitorInfo));
	monitorInfo.cbSize = sizeof(monitorInfo);

	if(GetMonitorInfo(hMonitor, &monitorInfo) == false)
		HyLogError("GetMonitorInfo failed");

	DISPLAY_DEVICE DispDev = { 0 };
	DispDev.cb = sizeof(DispDev);
	EnumDisplayDevices(monitorInfo.szDevice, 0, &DispDev, 0);
	
	HyMonitorDeviceInfo deviceInfo;
	deviceInfo.sDeviceName = monitorInfo.szDevice;
	deviceInfo.sDeviceDescription = DispDev.DeviceString;
	deviceInfo.bIsPrimaryMonitor = ((monitorInfo.dwFlags & MONITORINFOF_PRIMARY) != 0);

	DEVMODE dm = { 0 };
	dm.dmSize = sizeof(dm);
	for(int iModeNum = 0; EnumDisplaySettings(monitorInfo.szDevice, iModeNum, &dm) != 0; ++iModeNum)
		deviceInfo.vResolutions.push_back(HyMonitorDeviceInfo::Resolution(dm.dmPelsWidth, dm.dmPelsHeight));

	std::sort(deviceInfo.vResolutions.begin(), deviceInfo.vResolutions.end());
	deviceInfo.vResolutions.erase(std::unique(deviceInfo.vResolutions.begin(), deviceInfo.vResolutions.end()), deviceInfo.vResolutions.end());

	vMonitorDeviceInfo.push_back(deviceInfo);

	return TRUE;
}

HyOpenGL_Win::~HyOpenGL_Win()
{
}

HyOpenGL_Win::DeviceContext::DeviceContext(const HyWindowInfo &wndInfo)
{
	m_PixelFormatDescriptor.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	m_PixelFormatDescriptor.nVersion = 1;
	m_PixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	m_PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	m_PixelFormatDescriptor.cColorBits = 32;
	m_PixelFormatDescriptor.cRedBits = m_PixelFormatDescriptor.cRedShift = 0;
	m_PixelFormatDescriptor.cGreenBits = m_PixelFormatDescriptor.cGreenShift = 0;
	m_PixelFormatDescriptor.cBlueBits = m_PixelFormatDescriptor.cBlueShift = 0;
	m_PixelFormatDescriptor.cAlphaBits = m_PixelFormatDescriptor.cAlphaShift = 0;
	m_PixelFormatDescriptor.cAccumBits = m_PixelFormatDescriptor.cAccumRedBits = m_PixelFormatDescriptor.cAccumGreenBits = m_PixelFormatDescriptor.cAccumBlueBits = m_PixelFormatDescriptor.cAccumAlphaBits = 0;
	m_PixelFormatDescriptor.cDepthBits =24;
	m_PixelFormatDescriptor.cStencilBits = 8;
	m_PixelFormatDescriptor.cAuxBuffers = 0;
	m_PixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
	m_PixelFormatDescriptor.bReserved = 0;
	m_PixelFormatDescriptor.dwLayerMask = 0;
	m_PixelFormatDescriptor.dwVisibleMask = 0;
	m_PixelFormatDescriptor.dwDamageMask = 0;

	HINSTANCE hInstance = GetModuleHandle(NULL);

	MSG msg = { 0 };
	WNDCLASS wc = { 0 };

	std::wstring sWindowName = StringToWString(wndInfo.sName);

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = sWindowName.c_str();
	wc.style = CS_OWNDC;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);

	if(!RegisterClass(&wc))
	{
		HyError("DeviceContext RegisterClass() failed");
	}

	DWORD dwStyle;
	DWORD dwExStyle;
	if(wndInfo.eType == HYWINDOW_FullScreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = wndInfo.vResolution.x;
		dmScreenSettings.dmPelsHeight = wndInfo.vResolution.y;
		dmScreenSettings.dmBitsPerPel = m_PixelFormatDescriptor.cColorBits; //wndInfo.iBitsPerPixel;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if(ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			HyError("DeviceContext Full screen change unsuccessful");
		}

		dwExStyle = WS_EX_APPWINDOW;							// Window Extended Style
		dwStyle = WS_POPUP;										// Windows Style
		ShowCursor(FALSE);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	RECT rWndRect;
	SetRect(&rWndRect, 0, 0, wndInfo.vResolution.x, wndInfo.vResolution.y);
	AdjustWindowRectEx(&rWndRect, dwStyle, FALSE, dwExStyle);

	// Create The Window
	m_hWnd = CreateWindowEx(dwExStyle,
							wc.lpszClassName,
							StringToWString(wndInfo.sName).c_str(),
							dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
							wndInfo.vLocation.x,
							wndInfo.vLocation.y,
							rWndRect.right - rWndRect.left,
							rWndRect.bottom - rWndRect.top,
							NULL,
							NULL,
							hInstance,
							this);	// Passed into WM_CREATE lParam

	if(m_hWnd == NULL)
	{
		DWORD dwError = GetLastError();
		HyError("CreateWindowA() returned the error: " << dwError);
	}

	ShowWindow(m_hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(m_hWnd);						// Slightly Higher Priority
	SetFocus(m_hWnd);									// Sets Keyboard Focus To The Window
}

void HyOpenGL_Win::DeviceContext::Resize(GLsizei iWidth, GLsizei iHeight)
{
	// Prevent A Divide By Zero
	if(iHeight == 0)
		iHeight = 1;



	// Reset The Current Viewport
	//glViewport(0, 0, iWidth, iHeight);

	//glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	//glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f, (GLfloat)iWidth / (GLfloat)iHeight, 0.1f, 100.0f);

	//glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	//glLoadIdentity();									// Reset The Modelview Matrix
}

/*virtual*/ void HyOpenGL_Win::SetRenderSurface(eRenderSurfaceType eSurfaceType, uint32 uiIndex, bool bDirty)
{
	if(eSurfaceType == IHyRenderer::RENDERSURFACE_Window)
	{
		wglMakeCurrent(GetDC(m_ppDeviceContexes[uiIndex]->m_hWnd), DeviceContext::sm_hGLContext);

		//if(bDirty)
		// TODO: If fullscreen, make change here
	}
}

/*virtual*/ void HyOpenGL_Win::FinishRender()
{
	HDC hDeviceContext = GetDC(m_ppDeviceContexes[m_iWindowIndex]->m_hWnd);
	SwapBuffers(hDeviceContext);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
		{
			CREATESTRUCT *pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
			HyOpenGL_Win::DeviceContext *pThis = reinterpret_cast<HyOpenGL_Win::DeviceContext *>(pCreateStruct->lpCreateParams);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(pThis));

			HDC hDeviceContext = GetDC(hWnd);

			int iPixelFormat = ChoosePixelFormat(hDeviceContext, &pThis->m_PixelFormatDescriptor);
			SetPixelFormat(hDeviceContext, iPixelFormat, &pThis->m_PixelFormatDescriptor);

			if(pThis->sm_hGLContext == NULL)
			{
				HDC hDC = GetDC(hWnd);
				pThis->sm_hGLContext = wglCreateContext(hDC);
				wglMakeCurrent(hDC, pThis->sm_hGLContext);
			}
		}
		break;

		case WM_DESTROY:
		{
			HyOpenGL_Win::DeviceContext *pThis = reinterpret_cast<HyOpenGL_Win::DeviceContext *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			HDC hDeviceContext = GetDC(hWnd);
			wglMakeCurrent(hDeviceContext, pThis->sm_hGLContext);

			wglDeleteContext(pThis->sm_hGLContext);
			PostQuitMessage(0);

			DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_SYSCOMMAND:
		{
			switch(wParam)
			{
			case SC_SCREENSAVE:		// Screen saver Trying To Start?
			case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
				// Prevent From Happening
				return 0;
			}
			break;
		}

		case WM_SIZE:
		{
			HyOpenGL_Win::DeviceContext *pThis = reinterpret_cast<HyOpenGL_Win::DeviceContext *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			pThis->Resize(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

