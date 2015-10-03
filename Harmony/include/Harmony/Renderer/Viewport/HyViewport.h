/**************************************************************************
 *	HyViewport.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyViewport_h__
#define __HyViewport_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/Viewport/HyCamera2d.h"
#include "Renderer/Viewport/HyCamera3d.h"

class HyViewport
{
	friend class HyScene;

	HyWindowInfo			m_Info;
	
	vector<HyCamera2d *>	m_vCams2d;
	vector<HyCamera3d *>	m_vCams3d;

	enum eDirtyFlags
	{
		FLAG_Title		= 1 << 0,
		FLAG_Resolution	= 1 << 1,
		FLAG_Location	= 1 << 2,
		FLAG_Type		= 1 << 3,
		FLAG_BitsPerPix	= 1 << 4
	};
	uint32					m_uiDirtyFlags;

public:
	HyViewport();
	~HyViewport(void);

	const HyWindowInfo &GetWindowInfo();

	std::string			GetTitle();
	void				SetTitle(std::string sTitle);

	glm::ivec2			GetResolution();
	void				SetResolution(glm::ivec2 vResolution);

	glm::ivec2			GetLocation();
	void				SetLocation(glm::ivec2 ptLocation);

	HyWindowType		GetType();
	void				SetType(HyWindowType eType);

	int32				GetBitsPerPixel();
	void				SetBitsPerPixel(int32 iBitsPerPixel);

	HyCamera2d *		CreateCamera2d();
	HyCamera3d *		CreateCamera3d();

	void				RemoveCamera(HyCamera2d *&pCam);
	void				RemoveCamera(HyCamera3d *&pCam);
};

#endif /* __HyViewport_h__ */
