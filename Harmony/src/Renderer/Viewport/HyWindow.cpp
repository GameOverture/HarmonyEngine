/**************************************************************************
 *	HyViewport.cpp
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

HyWindow::HyWindow() : m_uiDirtyFlags(0)
{
}

HyWindow::~HyWindow(void)
{
	while(m_vCams2d.empty() == false)
		RemoveCamera(m_vCams2d[0]);

	while(m_vCams3d.empty() == false)
		RemoveCamera(m_vCams3d[0]);
}

const HyWindowInfo &HyWindow::GetWindowInfo()
{
	return m_Info;
}

std::string HyWindow::GetTitle()
{
	return m_Info.sName;
}

void HyWindow::SetTitle(std::string sTitle)
{
	m_Info.sName = sTitle;
	m_uiDirtyFlags |= FLAG_Title;
}

glm::ivec2 HyWindow::GetResolution()
{
	return m_Info.vResolution;
}

void HyWindow::SetResolution(glm::ivec2 vResolution)
{
	m_Info.vResolution = vResolution;
	m_uiDirtyFlags |= FLAG_Resolution;
}

glm::ivec2 HyWindow::GetLocation()
{
	return m_Info.vLocation;
}

void HyWindow::SetLocation(glm::ivec2 ptLocation)
{
	m_Info.vLocation = ptLocation;
	m_uiDirtyFlags |= FLAG_Location;
}

HyWindowType HyWindow::GetType()
{
	return m_Info.eType;
}

void HyWindow::SetType(HyWindowType eType)
{
	m_Info.eType = eType;
	m_uiDirtyFlags |= FLAG_Type;
}

int32 HyWindow::GetBitsPerPixel()
{
	return m_Info.iBitsPerPixel;
}

void HyWindow::SetBitsPerPixel(int32 iBitsPerPixel)
{
	m_Info.iBitsPerPixel = iBitsPerPixel;
	m_uiDirtyFlags |= FLAG_BitsPerPix;
}

HyCamera2d *HyWindow::CreateCamera2d()
{
	HyCamera2d *pNewCam = new HyCamera2d(this);
	m_vCams2d.push_back(pNewCam);

	return pNewCam;
}

HyCamera3d *HyWindow::CreateCamera3d()
{
	HyCamera3d *pNewCam = new HyCamera3d(this);
	m_vCams3d.push_back(pNewCam);

	return pNewCam;
}

void HyWindow::RemoveCamera(HyCamera2d *&pCam)
{
	for(vector<HyCamera2d *>::iterator iter = m_vCams2d.begin(); iter != m_vCams2d.end(); ++iter)
	{
		if((*iter) == pCam)
		{
			m_vCams2d.erase(iter);
			pCam = NULL;
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
			m_vCams3d.erase(iter);
			pCam = NULL;
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

