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

#include "Assets\Data\IHyData.h"
#include "Assets\HyManager_Data.h"

class HyTexturedQuad2dData : public IHyData
{
	friend class HyNodeDataContainer<HyTexturedQuad2dData>;

	const uint32		m_uiATLASGROUP_ID;
	HyAtlasGroup *		m_pAtlas;

	// Only allow HyNodeDataContainer instantiate
	HyTexturedQuad2dData(const std::string &sPath, int32 iShaderId);

public:
	virtual ~HyTexturedQuad2dData();

	virtual void DoFileLoad() override;

	HyAtlasGroup *GetAtlasGroup();

	virtual void SetRequiredAtlasIds(HyGfxData &gfxDataOut) override;
};

#endif /* __HyTexturedQuad2dData_h__ */
