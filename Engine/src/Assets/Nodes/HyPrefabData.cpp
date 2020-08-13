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

HyPrefabData::HyPrefabData(const std::string &sPath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(sPath)
{
	HyJsonArray prefabArray = itemDataObj["uvRects"].GetArray();

	m_UvRectList.reserve(prefabArray.Size());
	for(uint32 i = 0; i < prefabArray.Size(); ++i)
	{
		m_UvRectList.emplace_back();
		m_UvRectList[i].first = assetsRef.GetAtlas(prefabArray[i].GetUint(), m_UvRectList[i].second);
		m_RequiredAtlases.Set(m_UvRectList[i].first->GetManifestIndex());
	}

	m_pGltf = assetsRef.GetGltf(sPath);
}

HyPrefabData::~HyPrefabData(void)
{
}
