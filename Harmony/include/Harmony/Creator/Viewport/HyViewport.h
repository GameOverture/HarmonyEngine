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

struct HyResolution
{
	int32 iWidth;
	int32 iHeight;

	int32 iRedBits;
	int32 iBlueBits;
	int32 iGreenBits;

	bool IsUnknown() const	{ return (iWidth == -1 && iHeight == -1); }
	void SetUnknown()		{ iWidth = -1; iHeight = -1; }

	HyResolution()			{ SetUnknown(); }
};

class HyViewport
{
	friend class HyCreator;

	std::string				m_sWindowName;
	HyResolution			m_CurResolution;
    HyWindowType			m_eWindowType;

	vector<HyCamera2d *>	m_vCams2d;
	vector<HyCamera3d *>	m_vCams3d;

public:
	HyViewport(const HarmonyInit &initStruct);
	~HyViewport(void);

	HyCamera2d *CreateCamera2d();
	HyCamera3d *CreateCamera3d();

	void RemoveCamera(HyCamera2d *&pCam);
	void RemoveCamera(HyCamera3d *&pCam);

	const char *GetWindowName()			{ return m_sWindowName.c_str(); }
	const HyResolution &GetResolution()	{ return m_CurResolution; }
	HyWindowType GetWindowType()		{ return m_eWindowType; }
};

#endif /* __HyViewport_h__ */
