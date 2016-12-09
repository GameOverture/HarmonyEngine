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

std::vector<HyMonitorDeviceInfo>	HyWindow::sm_MonitorInfoList;
BasicSection						HyWindow::sm_csInfo;

HyWindow::HyWindow() : m_bTakeInput(true)
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

bool HyWindow::IsInputEnabled()
{
	return m_bTakeInput;
}

void HyWindow::SetInputEnabled(bool bEnabled)
{
	m_bTakeInput = bEnabled;
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
