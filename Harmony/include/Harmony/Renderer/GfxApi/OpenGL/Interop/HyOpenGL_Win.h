/**************************************************************************
 *	HyOpenGL_Win.h
 *
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyOpenGL_Win_h__
#define __HyOpenGL_Win_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/GfxApi/OpenGL/HyOpenGL.h"

class HyOpenGL_Win : public HyOpenGL
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HGLRC m_hGLContext;
	HWND m_hWnd;

public:
	HyOpenGL_Win();
	~HyOpenGL_Win();

	virtual bool Initialize();
	virtual bool PollApi();
};

#endif /* __HyOpenGL_Win_h__ */
