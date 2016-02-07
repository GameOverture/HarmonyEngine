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

#include "Diagnostics/HyGuiComms.h"
#include "Utilities/HyStrManip.h"

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HyOpenGL_Win::HyOpenGL_Win(HyGfxComms &gfxCommsRef, vector<HyWindow> &viewportsRef) :	HyOpenGL(gfxCommsRef, viewportsRef),
																						m_hGLContext(NULL)
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
	m_PixelFormatDescriptor.cDepthBits = 24;
	m_PixelFormatDescriptor.cStencilBits = 8;
	m_PixelFormatDescriptor.cAuxBuffers = 0;
	m_PixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
	m_PixelFormatDescriptor.bReserved = 0;
	m_PixelFormatDescriptor.dwLayerMask = 0;
	m_PixelFormatDescriptor.dwVisibleMask = 0;
	m_PixelFormatDescriptor.dwDamageMask = 0;

	for(uint32 i = 0; i < m_RenderSurfaces.size(); ++i)
		m_RenderSurfaces[i].m_pExData = ConstructWindow(m_vWindowRef[i].GetWindowInfo());

	vector<HyMonitorDeviceInfo> vMonitorDeviceInfo;
	if(EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&vMonitorDeviceInfo) == false)
		HyError("EnumDisplayMonitors failed");

	SetMonitorDeviceInfo(vMonitorDeviceInfo);

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
		HyError("GetMonitorInfo failed");

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

HWND HyOpenGL_Win::ConstructWindow(const HyWindowInfo &wndInfo)
{
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
	HWND hWnd = CreateWindowEx(dwExStyle,
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

	if(hWnd == NULL)
	{
		DWORD dwError = GetLastError();
		HyError("CreateWindowA() returned the error: " << dwError);
	}

	ShowWindow(hWnd, SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window

	return hWnd;
}

HWND HyOpenGL_Win::GetHWND(int32 iWindowIndex)
{
	for(uint32 i = 0; i < m_RenderSurfaces.size(); ++i)
	{
		if(m_RenderSurfaces[i].m_eType == IHyRenderer::RENDERSURFACE_Window && m_RenderSurfaces[i].m_iID == iWindowIndex)
			return reinterpret_cast<HWND>(m_RenderSurfaces[i].m_pExData);
	}

	return NULL;
}

/*virtual*/ void HyOpenGL_Win::StartRender()
{
	if(m_RenderSurfaceIter->m_eType == IHyRenderer::RENDERSURFACE_Window)
	{
		wglMakeCurrent(GetDC(reinterpret_cast<HWND>(m_RenderSurfaceIter->m_pExData)), m_hGLContext);

		//if(bDirty)
		// TODO: If fullscreen, make change here
	}

	HyOpenGL::StartRender();
}

/*virtual*/ void HyOpenGL_Win::FinishRender()
{
	HDC hDeviceContext = GetDC(reinterpret_cast<HWND>(m_RenderSurfaceIter->m_pExData));
	SwapBuffers(hDeviceContext);
}

/*virtual*/ void HyOpenGL_Win::OnRenderSurfaceChanged(RenderSurface &renderSurfaceRef, uint32 uiChangedFlags)
{
	// TODO: Update window size, change window title, etc based on uiChangedFlags

	HyOpenGL::OnRenderSurfaceChanged(renderSurfaceRef, uiChangedFlags);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE:
		{
			CREATESTRUCT *pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(pCreateStruct->lpCreateParams);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(pThis));

			HDC hDeviceContext = GetDC(hWnd);

			int iPixelFormat = ChoosePixelFormat(hDeviceContext, &pThis->m_PixelFormatDescriptor);
			SetPixelFormat(hDeviceContext, iPixelFormat, &pThis->m_PixelFormatDescriptor);

			if(pThis->m_hGLContext == NULL)
			{
				HDC hDC = GetDC(hWnd);
				pThis->m_hGLContext = wglCreateContext(hDC);
				wglMakeCurrent(hDC, pThis->m_hGLContext);
			}
		}
		break;

		case WM_DESTROY:
		{
			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			HDC hDeviceContext = GetDC(hWnd);
			wglMakeCurrent(hDeviceContext, pThis->m_hGLContext);

			wglDeleteContext(pThis->m_hGLContext);
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
			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			for(uint32 i = 0; i < static_cast<uint32>(pThis->m_RenderSurfaces.size()); ++i)
			{
				if(reinterpret_cast<HWND>(pThis->m_RenderSurfaces[i].m_pExData) == hWnd)
					pThis->m_RenderSurfaces[i].Resize(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
			}
			return 0;
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

