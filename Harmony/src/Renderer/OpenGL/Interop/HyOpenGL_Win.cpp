/**************************************************************************
 *	HyOpenGL_Win.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/OpenGL/Interop/HyOpenGL_Win.h"
#include "Renderer/Components/HyGfxComms.h"
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Components/HyRenderSurface.h"
#include "Utilities/HyStrManip.h"

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HyOpenGL_Win::HyOpenGL_Win(HyGfxComms &gfxCommsRef, HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef) :	HyOpenGL(gfxCommsRef, diagnosticsRef, bShowCursor, windowListRef),
																																				m_hGLContext(NULL)
{
}

/*virtual*/ bool HyOpenGL_Win::Initialize() /*override*/
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

	for(uint32 i = 0; i < m_RenderSurfaceList.size(); ++i)
		m_RenderSurfaceList[i].SetHandle(ConstructWindow(m_WindowListRef[i]->GetWindowInfo()));

	std::vector<HyMonitorDeviceInfo> vMonitorDeviceInfo;
	if(EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&vMonitorDeviceInfo) == false)
		HyError("EnumDisplayMonitors failed");

	SetMonitorDeviceInfo(vMonitorDeviceInfo);

	ShowCursor(m_bShowCursor);

	return HyOpenGL::Initialize();
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	std::vector<HyMonitorDeviceInfo> &vMonitorDeviceInfo = *reinterpret_cast<std::vector<HyMonitorDeviceInfo> *>(dwData);

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
	DEVMODE dmScreenSettings;
	switch(wndInfo.eType)
	{
	case HYWINDOW_WindowedFixed:
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
		break;
	case HYWINDOW_WindowedSizeable:
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_OVERLAPPEDWINDOW;							// Windows Style
		break;
	case HYWINDOW_FullScreen:
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
		break;
	case HYWINDOW_BorderlessWindow:
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle = WS_POPUP;							// Windows Style
		break;
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
	for(uint32 i = 0; i < m_RenderSurfaceList.size(); ++i)
	{
		if(m_RenderSurfaceList[i].GetType() == HYRENDERSURFACE_Window && m_RenderSurfaceList[i].GetId() == iWindowIndex)
			return m_RenderSurfaceList[i].GetHandle();
	}

	return nullptr;
}

/*virtual*/ void HyOpenGL_Win::StartRender()
{
	if(m_RenderSurfaceIter->GetType() == HYRENDERSURFACE_Window)
	{
		wglMakeCurrent(GetDC(m_RenderSurfaceIter->GetHandle()), m_hGLContext);
		HyErrorCheck_OpenGL("HyOpenGL_Win::StartRender", "wglMakeCurrent");

		//if(bDirty)
		// TODO: If fullscreen, make change here
	}

	HyOpenGL::StartRender();
}

/*virtual*/ void HyOpenGL_Win::FinishRender()
{
	HDC hDeviceContext = GetDC(m_RenderSurfaceIter->GetHandle());
	SwapBuffers(hDeviceContext);
}

/*virtual*/ void HyOpenGL_Win::OnRenderSurfaceChanged(HyRenderSurface &renderSurfaceRef, uint32 uiChangedFlags)
{
	// TODO: Update window size, change window title, etc based on uiChangedFlags

	HyOpenGL::OnRenderSurfaceChanged(renderSurfaceRef, uiChangedFlags);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_CREATE: {
			CREATESTRUCT *pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(pCreateStruct->lpCreateParams);

			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));

			HDC hDeviceContext = GetDC(hWnd);

			DescribePixelFormat(hDeviceContext, 1, sizeof(PIXELFORMATDESCRIPTOR), &pThis->m_PixelFormatDescriptor);
			int iPixelFormat = ChoosePixelFormat(hDeviceContext, &pThis->m_PixelFormatDescriptor);
			SetPixelFormat(hDeviceContext, iPixelFormat, &pThis->m_PixelFormatDescriptor);

			if(pThis->m_hGLContext == NULL)
			{
				HDC hDC = GetDC(hWnd);
				pThis->m_hGLContext = wglCreateContext(hDC);
				wglMakeCurrent(hDC, pThis->m_hGLContext);
				HyErrorCheck_OpenGL("WndProc:WM_CREATE", "wglMakeCurrent");
			}
		} break;

		case WM_DESTROY: {
			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			HDC hDeviceContext = GetDC(hWnd);
			wglMakeCurrent(hDeviceContext, pThis->m_hGLContext);
			HyErrorCheck_OpenGL("WndProc:WM_DESTROY", "wglMakeCurrent");

			wglDeleteContext(pThis->m_hGLContext);
			HyErrorCheck_OpenGL("WndProc:WM_DESTROY", "wglDeleteContext");

			PostQuitMessage(0);

			pThis->RequestQuit();
		} break;

		case WM_SYSCOMMAND: {
			switch(wParam)
			{
			case SC_SCREENSAVE:		// Screen saver Trying To Start?
			case SC_MONITORPOWER:	// Monitor Trying To Enter Powersave?
				// Prevent From Happening
				return 0;
			}
		} break;

		case WM_SIZE: {
			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

			for(uint32 i = 0; i < static_cast<uint32>(pThis->m_RenderSurfaceList.size()); ++i)
			{
				if(pThis->m_RenderSurfaceList[i].GetHandle() == hWnd)
					pThis->m_RenderSurfaceList[i].Resize(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
			}
		} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

