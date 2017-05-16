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

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

class HyTexturedQuad2dData : public IHyNodeData
{
	const uint32		m_uiATLAS_INDEX;
	HyAtlas *			m_pAtlas;

public:
	HyTexturedQuad2dData(const std::string &sPath, HyAssets &assetsRef);
	virtual ~HyTexturedQuad2dData();

	HyAtlas *GetAtlas();
};

#endif /* __HyTexturedQuad2dData_h__ */
