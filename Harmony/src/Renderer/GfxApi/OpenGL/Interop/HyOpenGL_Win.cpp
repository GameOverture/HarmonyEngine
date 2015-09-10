/**************************************************************************
 *	HyOpenGL_Win.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/

#include "Harmony/Renderer/GfxApi/OpenGL/Interop/HyOpenGL_Win.h"

#include "GuiTool/HyGuiComms.h"
#include "Utilities/HyStrManip.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


HyOpenGL_Win::HyOpenGL_Win()
{
	
}

HyOpenGL_Win::~HyOpenGL_Win()
{
}

HyOpenGL_Win::DeviceContext::DeviceContext(HyWindowInfo &wndInfo)
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	MSG msg = { 0 };
	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = L"Harmony Engine";
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
		dmScreenSettings.dmBitsPerPel = wndInfo.iBitsPerPixel;
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
	glViewport(0, 0, iWidth, iHeight);

	//glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	//glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f, (GLfloat)iWidth / (GLfloat)iHeight, 0.1f, 100.0f);

	//glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	//glLoadIdentity();									// Reset The Modelview Matrix
}

/*virtual*/ bool HyOpenGL_Win::CreateWindows()
{
	m_uiNumDCs = m_pGfxComms->GetGfxInit().uiNumWindows;
	m_ppDeviceContexes = new DeviceContext *[m_uiNumDCs];

	for(uint32 i = 0; i < m_uiNumDCs; ++i)
		m_ppDeviceContexes[i] = new DeviceContext(m_pGfxComms->GetGfxInit().windowInfo[i]);

	return true;
}

/*virtual*/ bool HyOpenGL_Win::Initialize()
{
	m_pGfxComms->SetGfxInfo(reinterpret_cast<HyGfxComms::tGfxInfo *>(1));

	return HyOpenGL::Initialize();
}

/*virtual*/ bool HyOpenGL_Win::PollApi()
{
	// TODO: return false when windows close message comes in or something similar
	MSG msg = { 0 };
	for(uint32 i = 0; i < m_uiNumDCs; ++i)
	{
		while(PeekMessageA(&msg, m_ppDeviceContexes[i]->m_hWnd, 0, 0, PM_REMOVE))
			DispatchMessageA(&msg);		
	}

	return true;
}

/*virtual*/ void HyOpenGL_Win::FinishRender()
{
	for(uint32 i = 0; i < m_uiNumDCs; ++i)
	{
		HDC hDeviceContext = GetDC(m_ppDeviceContexes[i]->m_hWnd);
		SwapBuffers(hDeviceContext);
	}
}



PIXELFORMATDESCRIPTOR pfd =
{
	sizeof(PIXELFORMATDESCRIPTOR),
	1,
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
	PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
	32,                        //Colordepth of the framebuffer.
	0, 0, 0, 0, 0, 0,
	0,
	0,
	0,
	0, 0, 0, 0,
	24,                        //Number of bits for the depthbuffer
	8,                        //Number of bits for the stencilbuffer
	0,                        //Number of Aux buffers in the framebuffer.
	PFD_MAIN_PLANE,
	0,
	0, 0, 0
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
	{
		HDC hDeviceContext = GetDC(hWnd);

		int iPixelFormat = ChoosePixelFormat(hDeviceContext, &pfd);
		SetPixelFormat(hDeviceContext, iPixelFormat, &pfd);

		CREATESTRUCT *pCreateStruct = reinterpret_cast<CREATESTRUCT *>(lParam);
		HyOpenGL_Win::DeviceContext *pThis = reinterpret_cast<HyOpenGL_Win::DeviceContext *>(pCreateStruct->lpCreateParams);

		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG>(pThis));

		pThis->m_hGLContext = wglCreateContext(hDeviceContext);
		wglMakeCurrent(hDeviceContext, pThis->m_hGLContext);
	}
	break;

	case WM_DESTROY:
	{
		HyOpenGL_Win::DeviceContext *pThis = reinterpret_cast<HyOpenGL_Win::DeviceContext *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

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
		HyOpenGL_Win::DeviceContext *pThis = reinterpret_cast<HyOpenGL_Win::DeviceContext *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

		pThis->Resize(LOWORD(lParam), HIWORD(lParam));  // LoWord=Width, HiWord=Height
		return 0;
	}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

