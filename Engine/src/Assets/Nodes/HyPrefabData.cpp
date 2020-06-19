/**************************************************************************
 *	HyPrefabData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyPrefabData.h"

HyPrefabData::HyPrefabData(const std::string &sPath, const jsonxx::Object &itemDataObjRef, HyAssets &assetsRef) :
	IHyNodeData(sPath)
{
	const jsonxx::Array &prefabArray = itemDataObjRef.get<jsonxx::Array>("uvRects");

	m_UvRectList.reserve(prefabArray.size());
	for(uint32 i = 0; i < static_cast<uint32>(prefabArray.size()); ++i)
	{
		m_UvRectList.emplace_back();
		m_UvRectList[i].first = assetsRef.GetAtlas(static_cast<uint32>(prefabArray.get<jsonxx::Number>(i)), m_UvRectList[i].second);
		m_RequiredAtlases.Set(m_UvRectList[i].first->GetManifestIndex());
	}

	m_pGltf = assetsRef.GetGltf(sPath);
}

HyPrefabData::~HyPrefabData(void)
{
}
