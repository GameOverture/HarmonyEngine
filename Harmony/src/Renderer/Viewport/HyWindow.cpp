/**************************************************************************
 *	HyWindow.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Viewport/HyWindow.h"

vector<HyMonitorDeviceInfo>	HyWindow::sm_vMonitorInfo;
BasicSection				HyWindow::sm_csInfo;

HyWindow::HyWindow()
{
}

HyWindow::~HyWindow(void)
{
	while(m_vCams2d.empty() == false)
		RemoveCamera(m_vCams2d.back());

	while(m_vCams3d.empty() == false)
		RemoveCamera(m_vCams3d.back());
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
	m_vCams2d.push_back(pNewCam);

	return pNewCam;
}

HyCamera3d *HyWindow::CreateCamera3d()
{
	HyCamera3d *pNewCam = HY_NEW HyCamera3d(this);
	m_vCams3d.push_back(pNewCam);

	return pNewCam;
}

void HyWindow::RemoveCamera(HyCamera2d *&pCam)
{
	for(vector<HyCamera2d *>::iterator iter = m_vCams2d.begin(); iter != m_vCams2d.end(); ++iter)
	{
		if((*iter) == pCam)
		{
			delete pCam;
			m_vCams2d.erase(iter);

			return;
		}
	}
}

void HyWindow::RemoveCamera(HyCamera3d *&pCam)
{
	for(vector<HyCamera3d *>::iterator iter = m_vCams3d.begin(); iter != m_vCams3d.end(); ++iter)
	{
		if((*iter) == pCam)
		{
			delete pCam;
			m_vCams3d.erase(iter);

			return;
		}
	}
}

/*static*/ void HyWindow::MonitorDeviceInfo(vector<HyMonitorDeviceInfo> &vDeviceInfoOut)
{
	vDeviceInfoOut.clear();

	sm_csInfo.Lock();

	for(uint32 i = 0; i < static_cast<uint32>(sm_vMonitorInfo.size()); ++i)
		vDeviceInfoOut.push_back(sm_vMonitorInfo[i]);

	sm_csInfo.Unlock();
}

/*static*/ void HyWindow::SetMonitorDeviceInfo(vector<HyMonitorDeviceInfo> &info)
{
	sm_csInfo.Lock();
	sm_vMonitorInfo.clear();
	sm_vMonitorInfo = info;
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
