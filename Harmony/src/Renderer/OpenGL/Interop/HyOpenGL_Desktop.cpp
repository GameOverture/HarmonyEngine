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
	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		glfwDestroyWindow(m_RenderSurfaceList[i].GetHandle());
		m_RenderSurfaceList[i].SetHandle(nullptr);
	}
}

/*virtual*/ bool HyOpenGL_Desktop::Initialize() /*override*/
{
	int32 iMajor, iMinor, iRevision;
	glfwGetVersion(&iMajor, &iMinor, &iRevision);
	HyLogInfo("GLFW Version: " << iMajor << "." << iMinor << "." << iRevision);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	for(uint32 i = 0; i < static_cast<uint32>(m_WindowListRef.size()); ++i)
	{
		// NOTE: IHyRenderer initializes 'm_RenderSurfaceList' with enough surfaces to account for every window
		m_RenderSurfaceList[i].SetHandle(glfwCreateWindow(static_cast<int32>(m_WindowListRef[i]->GetResolution().x),
														  static_cast<int32>(m_WindowListRef[i]->GetResolution().y),
														  m_WindowListRef[i]->GetTitle().c_str(),
														  nullptr, // GLFWmonitor
														  nullptr));// GLFWmonitor
		
		if(m_RenderSurfaceList[i].GetHandle() == nullptr)
		{
			HyLogError("HyOpenGL_Desktop::Initialize() - glfwCreateWindow returned nullptr (At least OpenGL 3.3 is required, or window or OpenGL context creation failed)");
			return false;
		}
	}

	glfwMakeContextCurrent(m_RenderSurfaceList[0].GetHandle());
	return HyOpenGL::Initialize();
}

/*virtual*/ void HyOpenGL_Desktop::StartRender()
{
	

	HyOpenGL::StartRender();
}

/*virtual*/ void HyOpenGL_Desktop::FinishRender()
{
	//glfwSwapBuffers(window);

	//glfwPollEvents();
//	HDC hDeviceContext = GetDC(m_RenderSurfaceIter->GetHandle());
//	SwapBuffers(hDeviceContext);
}
