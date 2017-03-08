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

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

#include "spine/spine.h"
#include "spine/extension.h"

class HySpine2dData : public IHyNodeData
{
	spAtlas *				m_SpineAtlasData;
	spSkeletonData *		m_SpineSkeletonData;

public:
	HySpine2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef);
	virtual ~HySpine2dData();

	spSkeletonData *GetSkeletonData()								{ return m_SpineSkeletonData; }

	// Sets every combination of animation blend to this amount
	void AnimInitBlend(float fInterpDur);

	void AnimInitBlend(const char *szAnimFrom, const char *szAnimTo, float fInterpDur);
	void AnimInitBlend(UINT32 uiAnimIdFrom, UINT32 uiAnimIdTo, float fInterpDur);

	virtual void AppendRequiredAtlasIndices(std::set<uint32> &requiredAtlasIdsOut) override;
};

#endif /* __HyEnt2dData_h__ */