/**************************************************************************
 *	HySpine2dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyEnt2dData_h__
#define __HyEnt2dData_h__

#include "Afx/HyStdAfx.h"
#include "Creator\Data\IData.h"

#include "Utilities/HySpineRuntime.h"

class HyTexture;
class IGfxApi;

class HySpine2dData : public IData
{
	friend class HyFactory<HySpine2dData>;

	spAtlas *				m_SpineAtlasData;
	spSkeletonData *		m_SpineSkeletonData;

	// Only allow HyFactory instantiate
	HySpine2dData(const std::string &sPath);

public:
	virtual ~HySpine2dData();

	spSkeletonData *GetSkeletonData()								{ return m_SpineSkeletonData; }

	// Sets every combination of animation blend to this amount
	void AnimInitBlend(float fInterpDur);

	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
	void AnimInitBlend(UINT32 uiAnimIdFrom, UINT32 uiAnimIdTo, float fInterpDur);

	virtual void DoFileLoad();
	virtual void OnGfxLoad(IGfxApi &gfxApi);
	virtual void OnGfxRemove(IGfxApi &gfxApi);
};

#endif /* __HyEnt2dData_h__ */