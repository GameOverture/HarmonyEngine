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

/*virtual*/ bool HyOpenGL_Win::CreateWindows()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	m_uiNumDCs = m_pGfxComms->GetGfxInit().uiNumWindows;
	m_pDeviceContexes = new DeviceContext[m_uiNumDCs];

	for(uint32 i = 0; i < m_uiNumDCs; ++i)
	{
		HyWindowInfo &wndInfo = m_pGfxComms->GetGfxInit().windowInfo[i];

		MSG msg = { 0 };
		WNDCLASS wc = { 0 };

		wc.lpfnWndProc = WndProc;
		wc.hInstance = hInstance;
		wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
		wc.lpszClassName = L"Harmony Engine";
		wc.style = CS_OWNDC;

		if(!RegisterClass(&wc))
		{
			HyError("HyOpenGL_Win::HyOpenGL_Win() - RegisterClass() failed");
		}

		
		m_pDeviceContexes[i].m_hWnd = CreateWindow(wc.lpszClassName, StringToWString(wndInfo.sName).c_str(), WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, this);

		if(m_pDeviceContexes[i].m_hWnd == NULL)
		{
			DWORD dwError = GetLastError();
			HyLogError("CreateWindowA() returned the error: " << dwError);
		}
		
	}

	return true;
}

/*virtual*/ bool HyOpenGL_Win::Initialize()
{
	MSG msg = { 0 };
	// TODO: fix this so it PeekMessage() on each window every loop. Continue once all windows have been created.
	for(uint32 i = 0; i < m_uiNumDCs; ++i)
	{
		while(GetMessage(&msg, m_pDeviceContexes[i].m_hWnd, 0, 0) != 0)
		{
			DispatchMessage(&msg);

			if(WM_CREATE == msg.message || WM_NCCREATE == msg.message)
				break;
		}
	}

	m_pGfxComms->SetGfxInfo(reinterpret_cast<HyGfxComms::tGfxInfo *>(1));

	return HyOpenGL::Initialize();
}

/*virtual*/ bool HyOpenGL_Win::PollApi()
{
	// TODO: return false when windows close message comes in or something similar
	MSG msg = { 0 };
	for(uint32 i = 0; i < m_uiNumDCs; ++i)
	{
		while(PeekMessageA(&msg, m_pDeviceContexes[i].m_hWnd, 0, 0, PM_REMOVE))
			DispatchMessageA(&msg);
	}

	return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_CREATE:
		{
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

			HDC hDeviceContext = GetDC(hWnd);

			int iPixelFormat = ChoosePixelFormat(hDeviceContext, &pfd);
			SetPixelFormat(hDeviceContext, iPixelFormat, &pfd);

			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(lParam);

			pThis->m_pDeviceContexes[0].m_hGLContext = wglCreateContext(hDeviceContext);
			wglMakeCurrent(hDeviceContext, pThis->m_pDeviceContexes[0].m_hGLContext);

			//char *pVersion = (char *)glGetString(GL_VERSION);
			//HyLogInfo(pVersion);
		}
		break;

	case WM_DESTROY:
		{
			HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(lParam);

			wglDeleteContext(pThis->m_pDeviceContexes[0].m_hGLContext);
			PostQuitMessage(0);
		}
		break;

	default:

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}
