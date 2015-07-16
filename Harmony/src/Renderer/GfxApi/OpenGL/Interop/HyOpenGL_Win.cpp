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


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


HyOpenGL_Win::HyOpenGL_Win()
{
	
}

HyOpenGL_Win::~HyOpenGL_Win()
{
}

/*virtual*/ bool HyOpenGL_Win::Initialize()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	MSG msg = { 0 };
	WNDCLASSA wc = { 0 };

	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = "Harmony Engine";
	wc.style = CS_OWNDC;

	if(!RegisterClassA(&wc))
	{
		HyError("HyOpenGL_Win::HyOpenGL_Win() - RegisterClass() failed");
	}

	m_hWnd = CreateWindowA(wc.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, 0, 0, hInstance, 0);

	SetPropA(m_hWnd, "ThisPtr", this);

	while(PeekMessageA(&msg, m_hWnd, 0, 0, PM_REMOVE))
		DispatchMessageA(&msg);

	m_pGfxComms->SetGfxInfo(reinterpret_cast<HyGfxComms::tGfxInfo *>(1));

	return true;
}

/*virtual*/ bool HyOpenGL_Win::PollApi()
{
	// TODO: return false when windows close message comes in or something similar
	MSG msg = { 0 };
	while(PeekMessageA(&msg, m_hWnd, 0, 0, PM_REMOVE))
		DispatchMessageA(&msg);

	return true;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE hThis = GetPropA(hWnd, "ThisPtr");
	HyOpenGL_Win *pThis = reinterpret_cast<HyOpenGL_Win *>(hThis);

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

			pThis->m_hGLContext = wglCreateContext(hDeviceContext);
			wglMakeCurrent(hDeviceContext, pThis->m_hGLContext);

			pThis->HyOpenGL::Initialize();

			//char *pVersion = (char *)glGetString(GL_VERSION);
			//HyLogInfo(pVersion);
		}
		break;

	case WM_DESTROY:
		wglDeleteContext(pThis->m_hGLContext);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}
