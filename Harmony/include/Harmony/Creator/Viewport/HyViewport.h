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

#include "Creator/Viewport/HyCamera2d.h"
#include "Creator/Viewport/HyCamera3d.h"

//struct HyResolution
//{
//	int32 iWidth;
//	int32 iHeight;
//
//	int32 iRedBits;
//	int32 iBlueBits;
//	int32 iGreenBits;
//
//	bool IsUnknown() const	{ return (iWidth == -1 && iHeight == -1); }
//	void SetUnknown()		{ iWidth = -1; iHeight = -1; }
//
//	HyResolution()			{ SetUnknown(); }
//};

class HyViewport
{
	friend class HyCreator;

	uint32					m_uiNumWindows;
	HyWindowInfo *			m_pWindows;
	
	vector<HyCamera2d *>	m_vCams2d;
	vector<HyCamera3d *>	m_vCams3d;

public:
	HyViewport(const HarmonyInit &initStruct);
	~HyViewport(void);

	uint32 GetNumWindows();
	HyWindowInfo &GetWindowInfo(uint32 uiIndex);

	HyCamera2d *CreateCamera2d();
	HyCamera3d *CreateCamera3d();

	void RemoveCamera(HyCamera2d *&pCam);
	void RemoveCamera(HyCamera3d *&pCam);
};

#endif /* __HyViewport_h__ */
