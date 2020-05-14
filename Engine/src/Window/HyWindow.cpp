/**************************************************************************
 *	HyWindow.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Window/HyWindow.h"

#ifdef HY_USE_GLFW
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

HyWindow::HyWindow(uint32 uiIndex, const HyWindowInfo &windowInfoRef, bool bShowCursor, HyWindowHandle hSharedContext) :
	m_uiINDEX(uiIndex),
	m_hData(nullptr)
{
	m_Info = windowInfoRef;
	m_vFramebufferSize = m_Info.vSize;

#ifdef HY_USE_GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	GLFWmonitor *pMonitorOwner = nullptr;
	switch(m_Info.eType)
	{
	case HYWINDOW_WindowedSizeable:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		break;
	case HYWINDOW_WindowedFixed:
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		break;
	case HYWINDOW_BorderlessWindow:
		glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
		break;
	case HYWINDOW_FullScreen:
		pMonitorOwner = GetGlfwMonitor();
		break;
	}
	
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);		// Will be shown after positioned

	m_hData = glfwCreateWindow(static_cast<int32>(m_Info.vSize.x),
							   static_cast<int32>(m_Info.vSize.y),
							   m_Info.sName.c_str(),
							   pMonitorOwner,
							   hSharedContext);
	if(m_hData == nullptr)
	{
		HyLogError("HyOpenGL_Desktop::Initialize() - glfwCreateWindow returned nullptr (At least OpenGL 3.1 is required, or window or OpenGL context creation failed)");
		return;
	}

	int32 iFrameBufferX, iFrameBufferY;
	glfwGetFramebufferSize(m_hData, &iFrameBufferX, &iFrameBufferY);
	m_vFramebufferSize.x = iFrameBufferX;
	m_vFramebufferSize.y = iFrameBufferY;

	glfwSetWindowUserPointer(m_hData, this);
	glfwSetWindowPos(m_hData, m_Info.ptLocation.x, m_Info.ptLocation.y);

	if(bShowCursor == false)
		glfwSetInputMode(m_hData, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glfwShowWindow(m_hData);

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

//#ifdef HY_USE_GLFW
//	glfwDestroyWindow(m_hData);
//#endif
}

uint32 HyWindow::GetIndex() const
{
	return m_uiINDEX;
}

std::string HyWindow::GetTitle()
{
	return m_Info.sName;
}

void HyWindow::SetTitle(const std::string &sTitle)
{
	m_Info.sName = sTitle;

#ifdef HY_USE_GLFW
	glfwSetWindowTitle(m_hData, m_Info.sName.c_str());
#endif
}

int32 HyWindow::GetWidth()
{
	return m_Info.vSize.x;
}

int32 HyWindow::GetHeight()
{
	return m_Info.vSize.y;
}

float HyWindow::GetWidthF(float fPercent /*= 1.0f*/)
{
	return static_cast<float>(m_Info.vSize.x) * fPercent;
}

float HyWindow::GetHeightF(float fPercent /*= 1.0f*/)
{
	return static_cast<float>(m_Info.vSize.y) * fPercent;
}

glm::ivec2 HyWindow::GetWindowSize()
{
	return m_Info.vSize;
}

void HyWindow::SetWindowSize(glm::ivec2 vResolutionHint)
{
	m_Info.vSize = vResolutionHint;
	
#ifdef HY_USE_GLFW
	glfwSetWindowSize(m_hData, m_Info.vSize.x, m_Info.vSize.y);
#elif defined(HY_PLATFORM_GUI)
	m_vFramebufferSize = m_Info.vSize;
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

#ifdef HY_USE_GLFW
	glfwSetWindowPos(m_hData, m_Info.ptLocation.x, m_Info.ptLocation.y);
#endif
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

uint32 HyWindow::GetNumCameras2d()
{
	return static_cast<uint32>(m_Cams2dList.size());
}

HyCamera2d *HyWindow::GetCamera2d(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_Cams2dList.size()), "HyWindow::GetCamera2d was passed an invalid index: " << uiIndex);
	return m_Cams2dList[uiIndex];
}

const std::vector<HyCamera2d *> &HyWindow::GetCamera2dList()
{
	return m_Cams2dList;
}

uint32 HyWindow::GetNumCameras3d()
{
	return static_cast<uint32>(m_Cams3dList.size());
}

HyCamera3d *HyWindow::GetCamera3d(uint32 uiIndex)
{
	HyAssert(uiIndex < static_cast<uint32>(m_Cams3dList.size()), "HyWindow::GetCamera3d was passed an invalid index: " << uiIndex);
	return m_Cams3dList[uiIndex];
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
	// Y-axis goes up when working with world coordinates
	ptViewportCoordinate.y = GetWindowSize().y - ptViewportCoordinate.y;

	// Convert to normalized [0.0 - 1.0]
	ptViewportCoordinate /= GetWindowSize();

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

HyWindowHandle HyWindow::GetHandle()
{
	return m_hData;
}

#ifdef HY_USE_GLFW
GLFWmonitor *HyWindow::GetGlfwMonitor()
{
	GLFWmonitor *pClosestMonitor = nullptr;

	glm::vec2 ptWindowCenter = glm::vec2(m_Info.ptLocation.x, m_Info.ptLocation.y) + (glm::vec2(m_Info.vSize.x, m_Info.vSize.y) / 2.0f);
	float fClosestDist = 999999999.0f;

	int iNumMonitors;
	GLFWmonitor **ppMonitors = glfwGetMonitors(&iNumMonitors);
	for(int i = 0; i < iNumMonitors; ++i)
	{
		int32 iX, iY;
		glfwGetMonitorPos(ppMonitors[i], &iX, &iY);
		const GLFWvidmode *pVidMode = glfwGetVideoMode(ppMonitors[i]);

		glm::vec2 ptMonitorCenter = glm::vec2(iX, iY) + (glm::vec2(pVidMode->width, pVidMode->height) / 2.0f);

		float fDist = glm::distance(ptWindowCenter, ptMonitorCenter);
		if(fClosestDist > fDist)
		{
			pClosestMonitor = ppMonitors[i];
			fClosestDist = fDist;
		}
	}

	return pClosestMonitor;
}

bool HyWindow::IsFullScreen()
{
	return glfwGetWindowMonitor(m_hData) != nullptr;
}

void HyWindow::SetFullScreen(bool bFullScreen)
{
	if(bFullScreen)
		glfwSetWindowMonitor(m_hData, GetGlfwMonitor(), m_Info.ptLocation.x, m_Info.ptLocation.y, m_Info.vSize.x, m_Info.vSize.y, GLFW_DONT_CARE);
	else
		glfwSetWindowMonitor(m_hData, nullptr, m_Info.ptLocation.x, m_Info.ptLocation.y, m_Info.vSize.x, m_Info.vSize.y, GLFW_DONT_CARE);
}
#endif
