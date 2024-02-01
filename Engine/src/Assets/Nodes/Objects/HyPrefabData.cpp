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
#include "Assets/Nodes/Objects/HyPrefabData.h"
#include "Assets/Files/HyFileAtlas.h"

HyPrefabData::HyPrefabData(const HyNodePath &nodePath, HyJsonObj itemDataObj, HyAssets &assetsRef) :
	IHyNodeData(nodePath)
{
	HyJsonArray prefabArray = itemDataObj["uvRects"].GetArray();

	m_UvRectList.reserve(prefabArray.Size());
	uint64 uiCropMask = 0;
	for(uint32 i = 0; i < prefabArray.Size(); ++i)
	{
		HyError("HyPrefabData::HyPrefabData - Determine bank ID");
		int32 iBankId = 0;
		m_UvRectList.emplace_back();
		m_UvRectList[i].first = assetsRef.GetAtlas(prefabArray[i].GetUint(), iBankId, m_UvRectList[i].second, uiCropMask);
		m_RequiredFiles[HYFILE_Atlas].Set(m_UvRectList[i].first->GetManifestIndex());
	}

	//m_pGltf = assetsRef.GetGltf(sPath);
}

HyPrefabData::~HyPrefabData(void)
{
}
