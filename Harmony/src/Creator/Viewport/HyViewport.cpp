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

HyViewport::HyViewport(const HarmonyInit &initStruct)
{
	m_CurResolution.iWidth = initStruct.vStartResolution.x;
	m_CurResolution.iHeight = initStruct.vStartResolution.y;

	m_sWindowName = initStruct.szGameName;

	m_eWindowType = initStruct.eWindowType;
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

HyViewport::~HyViewport(void)
{
}
