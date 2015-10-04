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

#include "Renderer/Viewport/HyViewport.h"

class HyOpenGL_Win : public HyOpenGL
{
	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	struct DeviceContext
	{
		HWND m_hWnd;
		HGLRC m_hGLContext;

		DeviceContext(const HyWindowInfo &wndInfo);
		~DeviceContext();

		void Resize(GLsizei width, GLsizei height);
	};
	DeviceContext **	m_ppDeviceContexes;
	uint32				m_uiNumDCs;

public:
	HyOpenGL_Win(HyGfxComms &gfxCommsRef, vector<HyViewport> &viewportsRef);
	~HyOpenGL_Win();

	uint32 GetNumDeviceContexts()				{ return m_uiNumDCs; }
	HWND GetDeviceContextHWND(uint32 uiIndex)	{ return m_ppDeviceContexes[uiIndex]->m_hWnd; }

	virtual void FinishRender();
};

#endif /* __HyOpenGL_Win_h__ */
