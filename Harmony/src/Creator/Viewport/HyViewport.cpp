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

HyViewport::HyViewport(const HarmonyInit &initStruct) : m_uiNumWindows(initStruct.uiNumWindows)
{
	HyAssert(m_uiNumWindows > 0, "HyViewport was initialized with '0' windows");
	m_pWindows = new HyWindowInfo[m_uiNumWindows];

	for(uint32 i = 0; i < m_uiNumWindows; ++i)
		m_pWindows[i] = initStruct.windowInfo[i];
}

HyViewport::~HyViewport(void)
{
	delete [] m_pWindows;
}

uint32 HyViewport::GetNumWindows()
{
	return m_uiNumWindows;
}

HyWindowInfo &HyViewport::GetWindowInfo(uint32 uiIndex)
{
	return m_pWindows[uiIndex];
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
