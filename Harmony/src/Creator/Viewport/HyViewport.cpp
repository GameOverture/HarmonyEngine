/**************************************************************************
 *	HyViewport.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Viewport/HyViewport.h"

HyViewport::HyViewport() : m_uiDirtyFlags(0)
{
}

HyViewport::~HyViewport(void)
{
	while(m_vCams2d.empty() == false)
		RemoveCamera(m_vCams2d[0]);

	while(m_vCams3d.empty() == false)
		RemoveCamera(m_vCams3d[0]);
}

const HyWindowInfo &HyViewport::GetWindowInfo()
{
	return m_Info;
}

std::string HyViewport::GetTitle()
{
	return m_Info.sName;
}

void HyViewport::SetTitle(std::string sTitle)
{
	m_Info.sName = sTitle;
	m_uiDirtyFlags |= FLAG_Title;
}

glm::ivec2 HyViewport::GetResolution()
{
	return m_Info.vResolution;
}

void HyViewport::SetResolution(glm::ivec2 vResolution)
{
	m_Info.vResolution = vResolution;
	m_uiDirtyFlags |= FLAG_Resolution;
}

glm::ivec2 HyViewport::GetLocation()
{
	return m_Info.vLocation;
}

void HyViewport::SetLocation(glm::ivec2 ptLocation)
{
	m_Info.vLocation = ptLocation;
	m_uiDirtyFlags |= FLAG_Location;
}

HyWindowType HyViewport::GetType()
{
	return m_Info.eType;
}

void HyViewport::SetType(HyWindowType eType)
{
	m_Info.eType = eType;
	m_uiDirtyFlags |= FLAG_Type;
}

int32 HyViewport::GetBitsPerPixel()
{
	return m_Info.iBitsPerPixel;
}

void HyViewport::SetBitsPerPixel(int32 iBitsPerPixel)
{
	m_Info.iBitsPerPixel = iBitsPerPixel;
	m_uiDirtyFlags |= FLAG_BitsPerPix;
}

HyCamera2d *HyViewport::CreateCamera2d()
{
	HyCamera2d *pNewCam = new HyCamera2d(this);
	m_vCams2d.push_back(pNewCam);

	return pNewCam;
}

HyCamera3d *HyViewport::CreateCamera3d()
{
	HyCamera3d *pNewCam = new HyCamera3d(this);
	m_vCams3d.push_back(pNewCam);

	return pNewCam;
}

void HyViewport::RemoveCamera(HyCamera2d *&pCam)
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

void HyViewport::RemoveCamera(HyCamera3d *&pCam)
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
