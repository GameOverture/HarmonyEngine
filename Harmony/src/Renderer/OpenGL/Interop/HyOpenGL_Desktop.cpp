/**************************************************************************
 *	HyOpenGL_Desktop.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/OpenGL/Interop/HyOpenGL_Desktop.h"
#include "Renderer/Components/HyWindow.h"
#include "Renderer/Components/HyRenderSurface.h"
#include "Utilities/HyStrManip.h"

HyOpenGL_Desktop::HyOpenGL_Desktop(HyDiagnostics &diagnosticsRef, bool bShowCursor, std::vector<HyWindow *> &windowListRef) :	HyOpenGL(diagnosticsRef, bShowCursor, windowListRef)
{
}

HyOpenGL_Desktop::~HyOpenGL_Desktop()
{
}

/*virtual*/ bool HyOpenGL_Desktop::Initialize() /*override*/
{
	int32 iMajor, iMinor, iRevision;
	glfwGetVersion(&iMajor, &iMinor, &iRevision);
	HyLogInfo("GLFW Version: " << iMajor << "." << iMinor << "." << iRevision);

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		GLFWwindow *pNewGlfwWindow = glfwCreateWindow(static_cast<int32>(m_WindowListRef[i]->GetResolution().x),
													  static_cast<int32>(m_WindowListRef[i]->GetResolution().y),
													  m_WindowListRef[i]->GetTitle().c_str(),
													  nullptr, // GLFWmonitor
													  nullptr);// GLFWmonitor
		if(pNewGlfwWindow == nullptr)
		{
			HyLogError("HyOpenGL_Desktop::Initialize() - glfwCreateWindow failed");
			glfwTerminate();
			return false;
		}

		m_glfwWindowList.push_back(pNewGlfwWindow);
	}

	return HyOpenGL::Initialize();
}

/*virtual*/ void HyOpenGL_Desktop::StartRender()
{
	if(m_RenderSurfaceIter->GetType() == HYRENDERSURFACE_Window)
		glfwMakeContextCurrent(m_glfwWindowList[m_RenderSurfaceIter->GetId()]);

	HyOpenGL::StartRender();
}

/*virtual*/ void HyOpenGL_Desktop::FinishRender()
{
	//glfwSwapBuffers(window);

	//glfwPollEvents();
	HDC hDeviceContext = GetDC(m_RenderSurfaceIter->GetHandle());
	SwapBuffers(hDeviceContext);
}
