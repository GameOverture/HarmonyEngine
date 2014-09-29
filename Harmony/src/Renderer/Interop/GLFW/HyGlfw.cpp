/**************************************************************************
 *	HyGlfw.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Interop/GLFW/HyGlfw.h"

#ifndef HY_PLATFORM_GUI

#include "Utilities/HyStrManip.h"
#include "FileIO/HyFileIO.h"

#include "Renderer/Interop/GLFW/HyGlfwInput.h"
#include "Creator/Viewport/HyViewport.h"

GLFWwindow * HyGlfw::sm_pGLFWwindow = NULL;

/*static*/ glm::ivec2		HyGlfw::sm_vWindowSize;
/*static*/ bool				HyGlfw::sm_bViewportDirty = false;

HyGlfw::HyGlfw()
{
}

HyGlfw::~HyGlfw(void)
{
}

/*virtual*/ bool HyGlfw::Initialize()
{
	HyGfxComms::tGfxInfo *pGfxInfo = new HyGfxComms::tGfxInfo;

	//////////////////////////////////////////////////////////////////////////
	// Init GLFW
	//////////////////////////////////////////////////////////////////////////

	// Setup the error callback for GLFW
	glfwSetErrorCallback(OnErrorCallback);

	switch(m_pGfxComms->GetGfxInit()->eWindowType)
	{
	case HYWINDOW_WindowedFixed:
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		break;
	case HYWINDOW_WindowedSizeable:
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
		break;

	case HYWINDOW_FullScreen:
		break;

	case HYWINDOW_BorderlessWindow:
		glfwWindowHint(GLFW_DECORATED, GL_FALSE);
		break;
	}
	
	
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	//glfwWindowHint(GLFW_SAMPLES,4);
	//glfwDisable( GLFW_AUTO_POLL_EVENTS );

	// TODO: Use glGet() ? with argument GL_MAX_ELEMENTS_VERTICES and GL_MAX_ELEMENTS_INDICES and check bounds


	// Query resolutions (video modes) and store them.
	const GLFWvidmode *		pGLFWvidModeList;

	pGLFWvidModeList = glfwGetVideoModes(glfwGetPrimaryMonitor(), &pGfxInfo->uiNumNativeResolutions);
	pGfxInfo->pResolutionList = new HyResolution[pGfxInfo->uiNumNativeResolutions];
	for(int32 i = 0; i < pGfxInfo->uiNumNativeResolutions; ++i)
	{
		pGfxInfo->pResolutionList[i].iRedBits = pGLFWvidModeList[i].redBits;
		pGfxInfo->pResolutionList[i].iGreenBits = pGLFWvidModeList[i].greenBits;
		pGfxInfo->pResolutionList[i].iBlueBits = pGLFWvidModeList[i].blueBits;

		pGfxInfo->pResolutionList[i].iWidth = pGLFWvidModeList[i].width;
		pGfxInfo->pResolutionList[i].iHeight = pGLFWvidModeList[i].height;
	}

	const HyResolution &curResolution = m_pGfxComms->GetGfxInit()->startResolution;
	const char *szWindowName = m_pGfxComms->GetGfxInit()->szWindowName.c_str();

	// Open an OpenGL window
	sm_pGLFWwindow = glfwCreateWindow(curResolution.iWidth, curResolution.iHeight, szWindowName, NULL, NULL);

	if(!sm_pGLFWwindow)
	{
		return false;
	}

	sm_vWindowSize.x = curResolution.iWidth;
	sm_vWindowSize.y = curResolution.iHeight;
	sm_bViewportDirty = true;

	// Make sure the window context is current with this render thread
	glfwMakeContextCurrent(sm_pGLFWwindow);

	// Read game settings file. If no resolution is set, use default desktop resolution
	const GLFWvidmode *pDeskTopMode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	// Center window within user's monitor
	int iWndCurWidth = 0;
	int iWndCurHeight = 0;
	glfwGetWindowSize(sm_pGLFWwindow, &iWndCurWidth, &iWndCurHeight);
	glfwSetWindowPos(sm_pGLFWwindow, (pDeskTopMode->width / 2) - (iWndCurWidth / 2), (pDeskTopMode->height / 2) - (iWndCurHeight / 2));

	//int iGLVersionMajor = glfwGetWindowParam(sm_pGLFWwindow, GLFW_CONTEXT_VERSION_MAJOR);
	//int iGLVersionMinor = glfwGetWindowParam(sm_pGLFWwindow, GLFW_CONTEXT_VERSION_MINOR);

	glfwMakeContextCurrent(sm_pGLFWwindow);

	// Callbacks
	glfwSetWindowSizeCallback(sm_pGLFWwindow, GLFWResizeCallback);

	glfwSetKeyCallback(sm_pGLFWwindow, GLFWKeyCallback);

	HyOpenGL::Initialize();
	
	m_pGfxComms->SetGfxInfo(pGfxInfo);

	return true;
}

/*virtual*/ bool HyGlfw::PollApi()
{
	HyGlfwInput::Lock();
	glfwPollEvents();
	HyGlfwInput::Unlock();

	if(sm_bViewportDirty)
	{
		glViewport(0, 0, (GLsizei)sm_vWindowSize.x, (GLsizei)sm_vWindowSize.y);
		m_mtxProj = glm::ortho(sm_vWindowSize.x * -0.5f, sm_vWindowSize.x * 0.5f, sm_vWindowSize.y * -0.5f, sm_vWindowSize.y * 0.5f);

		sm_bViewportDirty = false;
	}

	return (!glfwWindowShouldClose(sm_pGLFWwindow));
}

/*virtual*/ bool HyGlfw::CheckDevice()
{
	return true;
}

/*virtual*/ void HyGlfw::FinishRender()
{
	glfwSwapBuffers(sm_pGLFWwindow);
}

/*virtual*/ bool HyGlfw::Shutdown()
{
	glfwDestroyWindow(sm_pGLFWwindow);

	return true;
}

/*static*/ void HyGlfw::GLFWResizeCallback(GLFWwindow*,int iWidth, int iHeight)
{
	sm_vWindowSize.x = iWidth;
	sm_vWindowSize.y = iHeight;

	sm_bViewportDirty = true;
}

/*static*/ void HyGlfw::GLFWKeyCallback(GLFWwindow *pWindow, int iKey, int iScancode, int iAction, int iMod)
{
	// GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT.
	// GLFW_MOD_CONTROL
	// GLFW_MOD_ALT
	// GLFW_MOD_SUPER
	HyGlfwInput::SetKey(iKey, iAction, iMod);
}

/*static*/ void HyGlfw::OnErrorCallback(int iError, const char *szDescription)
{
	HyError(szDescription);
}

#endif /* !HY_PLATFORM_GUI */
