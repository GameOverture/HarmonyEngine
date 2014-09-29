/**************************************************************************
 *	HyGlfw.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyGlfw_h__
#define __HyGlfw_h__

#include "Afx/HyStdAfx.h"

#if !defined(HY_PLATFORM_GUI)

#include "Renderer/Interop/OpenGL/HyOpenGL.h"

static void ResizeViewport(int32 iWidth, int32 iHeight);

class HyGlfw : public HyOpenGL
{
	static GLFWwindow *		sm_pGLFWwindow;


	static glm::ivec2		sm_vWindowSize;
	static bool				sm_bViewportDirty;

public:
	HyGlfw();
	~HyGlfw(void);

	static inline GLFWwindow *GetGLFWWindow()	{ return sm_pGLFWwindow; }

	virtual bool Initialize();
	virtual bool PollApi();
	virtual bool CheckDevice();

	virtual void FinishRender();

	virtual bool Shutdown();

	static void GLFWResizeCallback(GLFWwindow*,int iWidth, int iHeight);
	static void GLFWKeyCallback(GLFWwindow *pWindow, int iKey, int iScancode, int iAction, int iMod);

private:
	static void OnErrorCallback(int iError, const char *szDescription);
};

#endif /* !HY_PLATFORM_GUI */

#endif /* __HyGlfw_h__ */
