/**************************************************************************
*	HyTexturedQuad2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyTexturedQuad2dData_h__
#define __HyTexturedQuad2dData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Data/IHyData.h"
#include "Assets/HyAssets.h"

class HyTexturedQuad2dData : public IHyData
{
	const uint32		m_uiATLASGROUP_ID;
	HyAtlasGroup *		m_pAtlas;

public:
	HyTexturedQuad2dData(const std::string &sPath, HyAssets &assetsRef);
	virtual ~HyTexturedQuad2dData();

	HyAtlasGroup *GetAtlasGroup();

	virtual void AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIdsOut) override;
};

#endif /* __HyTexturedQuad2dData_h__ */
