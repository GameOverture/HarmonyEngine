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

std::vector<HyMonitorDeviceInfo>	HyWindow::sm_MonitorInfoList;
BasicSection						HyWindow::sm_csInfo;

HyWindow::HyWindow()
{
}

HyWindow::~HyWindow(void)
{
	while(m_Cams2dList.empty() == false)
		RemoveCamera(m_Cams2dList.back());

	while(m_Cams3dList.empty() == false)
		RemoveCamera(m_Cams3dList.back());
}

const HyWindowInfo &HyWindow::GetWindowInfo()
{
	return m_Info_Update;
}

std::string HyWindow::GetTitle()
{
	return m_Info_Update.sName;
}

void HyWindow::SetTitle(std::string sTitle)
{
	m_Info_Update.sName = sTitle;
	m_Info_Update.uiDirtyFlags |= HyWindowInfo::FLAG_Title;
}

glm::ivec2 HyWindow::GetResolution()
{
	return m_Info_Update.vResolution;
}

void HyWindow::SetResolution(glm::ivec2 vResolution)
{
	m_Info_Update.vResolution = vResolution;
	m_Info_Update.uiDirtyFlags |= HyWindowInfo::FLAG_Resolution;
}

glm::ivec2 HyWindow::GetLocation()
{
	return m_Info_Update.vLocation;
}

void HyWindow::SetLocation(glm::ivec2 ptLocation)
{
	m_Info_Update.vLocation = ptLocation;
	m_Info_Update.uiDirtyFlags |= HyWindowInfo::FLAG_Location;
}

HyWindowType HyWindow::GetType()
{
	return m_Info_Update.eType;
}

void HyWindow::SetType(HyWindowType eType)
{
	m_Info_Update.eType = eType;
	m_Info_Update.uiDirtyFlags |= HyWindowInfo::FLAG_Type;
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

/*static*/ void HyWindow::MonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &vDeviceInfoOut)
{
	vDeviceInfoOut.clear();

	sm_csInfo.Lock();

	for(uint32 i = 0; i < static_cast<uint32>(sm_MonitorInfoList.size()); ++i)
		vDeviceInfoOut.push_back(sm_MonitorInfoList[i]);

	sm_csInfo.Unlock();
}

/*static*/ void HyWindow::SetMonitorDeviceInfo(std::vector<HyMonitorDeviceInfo> &info)
{
	sm_csInfo.Lock();
	sm_MonitorInfoList.clear();
	sm_MonitorInfoList = info;
	sm_csInfo.Unlock();
}

void HyWindow::Update()
{
	if(m_Info_Update.uiDirtyFlags)
	{
		m_cs.Lock();
		m_Info_Shared = m_Info_Update;
		m_cs.Unlock();

		m_Info_Update.uiDirtyFlags = 0;
	}
}

HyWindowInfo &HyWindow::Update_Render()
{
	m_cs.Lock();
	if(m_Info_Shared.uiDirtyFlags)
	{
		m_Info_Render = m_Info_Shared;
		m_Info_Shared.uiDirtyFlags = 0;
	}
	m_cs.Unlock();

	return m_Info_Render;
}
