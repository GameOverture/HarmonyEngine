/**************************************************************************
 *	HyPrefabData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Nodes/HyPrefabData.h"

HyPrefabData::HyPrefabData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef) :	IHyNodeData(sPath)
{
	const jsonxx::Object &prefabObj = dataValueRef.get<jsonxx::Object>();
	const jsonxx::Array &prefabImagesArray = prefabObj.get<jsonxx::Array>("images");

	m_UvRectList.reserve(prefabImagesArray.size());
	for(uint32 i = 0; i < static_cast<uint32>(prefabImagesArray.size()); ++i)
	{
		m_UvRectList.emplace_back();
		m_UvRectList[i].first = assetsRef.GetAtlas(static_cast<uint32>(prefabImagesArray.get<jsonxx::Object>(i).get<jsonxx::Number>("checksum")), m_UvRectList[i].second);
		m_RequiredAtlasIndices.Set(m_UvRectList[i].first->GetMasterIndex());
	}
}

HyPrefabData::~HyPrefabData(void)
{
}
