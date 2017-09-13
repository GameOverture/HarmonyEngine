/**************************************************************************
 *	HyOpenGL_Win.h
 *
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyOpenGL_Win_h__
#define HyOpenGL_Win_h__

#include "Afx/HyStdAfx.h"
#include "Renderer/OpenGL/HyOpenGL.h"

class HyOpenGL_Win : public HyOpenGL
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HGLRC					m_hGLContext;
	PIXELFORMATDESCRIPTOR	m_PixelFormatDescriptor;

public:
	HyOpenGL_Win(HyGfxComms &gfxCommsRef, HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef);
	~HyOpenGL_Win();

	virtual bool Initialize() override;
	
	HWND ConstructWindow(const HyWindowInfo &wndInfo);

	HWND GetHWND(int32 iWindowIndex);

	virtual void StartRender() override;
	virtual void FinishRender() override;
};

#endif /* HyOpenGL_Win_h__ */
