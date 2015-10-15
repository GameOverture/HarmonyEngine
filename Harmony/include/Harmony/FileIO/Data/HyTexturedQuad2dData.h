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

#include "FileIO\Data\IHyData2d.h"
#include "FileIO\HyFactory.h"

class HyTexturedQuad2dData : public IHyData2d
{
	friend class HyFactory<HyTexturedQuad2dData>;

	uint32				m_uiTextureId;
	uint32				m_uiTextureIndex;
	HyAtlasGroup *		m_pAtlas;

	// Only allow HyFactory instantiate
	HyTexturedQuad2dData(const std::string &sPath);

public:
	virtual ~HyTexturedQuad2dData();

	virtual void DoFileLoad();

	HyAtlasGroup *GetAtlasGroup();
	uint32 GetTextureIndex();
};

#endif /* __HyTexturedQuad2dData_h__ */
