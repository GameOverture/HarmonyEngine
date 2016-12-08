/**************************************************************************
 *	HyEntity2d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEntity2d_h__
#define __HyEntity2d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Instances/HyPrimitive2d.h"
#include "Scene/Instances/HyText2d.h"
#include "Scene/Instances/HySprite2d.h"
#include "Scene/Instances/HySpine2d.h"

#include "Utilities/HyMath.h"

class HyEntity2d : public IHyTransform2d
{
	const std::string				m_sNAME;
	const std::string				m_sPREFIX;

	HyLoadState						m_eLoadState;

public:
	HyEntity2d();
	HyEntity2d(const char *szPrefix, const char *szName);
	virtual ~HyEntity2d(void);

	inline void	SetEnabled(bool bEnabled);
	void SetCoordinateType(HyCoordinateType eCoordType, HyCamera2d *pCameraToCovertFrom);

	void SetDisplayOrder(int32 iOrderValue);

	void SetTint(float fR, float fG, float fB);
	void SetTransparency(float fTransparency);

	void Load();
	void Unload();

	virtual void OnUpdate();
};

#endif /* __HyEntity2d_h__ */
