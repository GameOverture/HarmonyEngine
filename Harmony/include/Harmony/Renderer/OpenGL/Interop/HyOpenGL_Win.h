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
#include "Renderer/OpenGL/HyOpenGL.h"

class HyOpenGL_Win : public HyOpenGL
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HGLRC					m_hGLContext;
	PIXELFORMATDESCRIPTOR	m_PixelFormatDescriptor;

public:
	HyOpenGL_Win(HyGfxComms &gfxCommsRef, std::vector<HyWindow *> &windowListRef);
	~HyOpenGL_Win();

	HWND ConstructWindow(const HyWindowInfo &wndInfo);

	HWND GetHWND(int32 iWindowIndex);

	virtual void StartRender();
	virtual void FinishRender();

	virtual void OnRenderSurfaceChanged(RenderSurface &renderSurfaceRef, uint32 uiChangedFlags);
};

#endif /* __HyOpenGL_Win_h__ */
