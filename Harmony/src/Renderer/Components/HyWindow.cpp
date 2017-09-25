/**************************************************************************
 *	HyWindow.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Components/HyWindow.h"

uint32 HyWindow::sm_uiIdCounter = 0;

#ifdef HY_PLATFORM_DESKTOP
	void glfw_WindowSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight)
	{
		HyWindow *pHyWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		pHyWindow->m_Info.vSize.x = iWidth;
		pHyWindow->m_Info.vSize.y = iHeight;
	}

	void glfw_FramebufferSizeCallback(GLFWwindow *pWindow, int32 iWidth, int32 iHeight)
	{
		HyWindow *pHyWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		pHyWindow->m_vFramebufferSize.x = iWidth;
		pHyWindow->m_vFramebufferSize.y = iHeight;
	}

	void glfw_WindowPosCallback(GLFWwindow *pWindow, int32 iX, int32 iY)
	{
		HyWindow *pHyWindow = reinterpret_cast<HyWindow *>(glfwGetWindowUserPointer(pWindow));
		pHyWindow->m_Info.ptLocation.x = iX;
		pHyWindow->m_Info.ptLocation.y = iY;
	}
#endif

HyWindow::HyWindow(const HyWindowInfo &windowInfoRef, HyRenderSurfaceHandleInterop hSharedContext) : m_uiID(sm_uiIdCounter)
{
	sm_uiIdCounter++;
	m_Info = windowInfoRef;

#ifdef HY_PLATFORM_DESKTOP
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	//if(m_Init.eType
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	//m_WindowList[i]->SetType(m_Init.windowInfo[i].eType);

	m_hData = glfwCreateWindow(static_cast<int32>(m_Info.vSize.x),
							   static_cast<int32>(m_Info.vSize.y),
							   m_Info.sName.c_str(),
							   nullptr, // GLFWmonitor
							   hSharedContext);
	if(m_hData == nullptr)
	{
		HyLogError("HyOpenGL_Desktop::Initialize() - glfwCreateWindow returned nullptr (At least OpenGL 3.3 is required, or window or OpenGL context creation failed)");
		return;
	}

	glfwSetWindowUserPointer(m_hData, this);
	glfwSetWindowPos(m_hData, m_Info.ptLocation.x, m_Info.ptLocation.y);

	// Set callbacks
	glfwSetWindowSizeCallback(m_hData, glfw_WindowSizeCallback);
	glfwSetFramebufferSizeCallback(m_hData, glfw_FramebufferSizeCallback);
	glfwSetWindowPosCallback(m_hData, glfw_WindowPosCallback);
#endif
}

HyWindow::~HyWindow(void)
{
	while(m_Cams2dList.empty() == false)
		RemoveCamera(m_Cams2dList.back());

	while(m_Cams3dList.empty() == false)
		RemoveCamera(m_Cams3dList.back());

#ifdef HY_PLATFORM_DESKTOP
	glfwDestroyWindow(m_hData);
	m_hData = nullptr;
#endif
}

uint32 HyWindow::GetId() const
{
	return m_uiID;
}

std::string HyWindow::GetTitle()
{
	return m_Info.sName;
}

void HyWindow::SetTitle(const std::string &sTitle)
{
	m_Info.sName = sTitle;

#ifdef HY_PLATFORM_DESKTOP
	glfwSetWindowTitle(m_hData, m_Info.sName.c_str());
#endif
}

glm::ivec2 HyWindow::GetWindowSize()
{
	return m_Info.vSize;
}

void HyWindow::SetWindowSize(glm::ivec2 vResolutionHint)
{
	m_Info.vSize = vResolutionHint;
	
#ifdef HY_PLATFORM_DESKTOP
	glfwSetWindowSize(m_hData, m_Info.vSize.x, m_Info.vSize.y);
#endif
}

glm::ivec2 HyWindow::GetFramebufferSize()
{
	return m_vFramebufferSize;
}

glm::ivec2 HyWindow::GetLocation()
{
	return m_Info.ptLocation;
}

void HyWindow::SetLocation(glm::ivec2 ptLocation)
{
	m_Info.ptLocation = ptLocation;

#ifdef HY_PLATFORM_DESKTOP
	glfwSetWindowPos(m_hData, m_Info.ptLocation.x, m_Info.ptLocation.y);
#endif
}

HyWindowType HyWindow::GetType()
{
	return m_Info.eType;
}

void HyWindow::SetType(HyWindowType eType)
{
	HyError("HyWindow::SetType has not been implemented");
	m_Info.eType = eType;
}

HyCamera2d *HyWindow::CreateCamera2d()
{
	HyCamera2d *pNewCam = HY_NEW HyCamera2d(this);
	m_Cams2dList.push_back(pNewCam);

	return pNewCam;
}

HyCamera3d *HyWindow::CreateCamera3d()
{
	HyCamera3d *pNewCam = HY_NEW HyCamera3d(this);
	m_Cams3dList.push_back(pNewCam);

	return pNewCam;
}

void HyWindow::RemoveCamera(HyCamera2d *&pCam)
{
	for(std::vector<HyCamera2d *>::iterator iter = m_Cams2dList.begin(); iter != m_Cams2dList.end(); ++iter)
	{
		if((*iter) == pCam)
		{
			delete pCam;
			m_Cams2dList.erase(iter);

			return;
		}
	}
}

void HyWindow::RemoveCamera(HyCamera3d *&pCam)
{
	for(std::vector<HyCamera3d *>::iterator iter = m_Cams3dList.begin(); iter != m_Cams3dList.end(); ++iter)
	{
		if((*iter) == pCam)
		{
			delete pCam;
			m_Cams3dList.erase(iter);

			return;
		}
	}
}

glm::vec2 HyWindow::ConvertViewportCoordinateToWorldPos(glm::vec2 ptViewportCoordinate)
{
	glm::vec2 ptWorldPos(0.0f);

	// Find the first camera that encompasses this normalized point in the viewport
	for(uint32 i = 0; i < m_Cams2dList.size(); ++i)
	{
		const HyRectangle<float> &viewportRect = m_Cams2dList[i]->GetViewport();

		if(ptViewportCoordinate.x >= viewportRect.left   && ptViewportCoordinate.x <= viewportRect.right &&
		   ptViewportCoordinate.y >= viewportRect.bottom && ptViewportCoordinate.y <= viewportRect.top)
		{
			// Find local coordinate in found camera's viewport
			glm::vec2 vOffsetInViewport;
			vOffsetInViewport.x = (ptViewportCoordinate.x - viewportRect.left) / (viewportRect.right - viewportRect.left);
			vOffsetInViewport.y = (ptViewportCoordinate.y - viewportRect.bottom) / (viewportRect.top - viewportRect.bottom);

			// Now using the found camera's transformation convert to the world position
			const b2AABB &aabbWorldRef = m_Cams2dList[i]->GetWorldViewBounds();
			
			float fDeg = m_Cams2dList[i]->rot.Get();

			if(fDeg != 0.0f)
			{
				// TODO: Calculate world vector orientation
				HyError("ConvertViewportCoordinateToWorldPos() TODO: Calculate world vector orientation for camera");

				glm::vec2 vOrientation(cos(glm::radians(fDeg)), sin(glm::radians(fDeg)));
			}
			else
			{
				//const b2AABB &aabbWorldRef = m_Cams2dList[i]->GetWorldAABB();

				ptWorldPos.x = (aabbWorldRef.upperBound.x - aabbWorldRef.lowerBound.x) * vOffsetInViewport.x;
				ptWorldPos.x += aabbWorldRef.lowerBound.x;

				ptWorldPos.y = (aabbWorldRef.upperBound.y - aabbWorldRef.lowerBound.y) * vOffsetInViewport.y;
				ptWorldPos.y += aabbWorldRef.lowerBound.y;
			}

			break;
		}
	}

	return ptWorldPos;
}

HyRenderSurfaceHandleInterop HyWindow::GetHandle()
{
	return m_hData;
}
