/**************************************************************************
*	AssetMimeData.h
*
*	Harmony Engine - HyEditor
*	Copyright (c) 2020 Jason Knobler
*
*	GPL/LGPL License:
*	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
*************************************************************************/
#ifndef AssetMimeData_H
#define AssetMimeData_H

#include "Global.h"
#include "IMimeData.h"

class AssetMimeData : public IMimeData
{
	uint32				m_AssetCounts[NUM_ASSETMANTYPES];

public:
	AssetMimeData(Project &projRef, QList<TreeModelItemData *> &assetListRef, AssetManagerType eAssetType);
	virtual ~AssetMimeData();

	uint32 GetNumAssetsOfType(AssetManagerType eAssetType) const;
	QJsonArray GetAssetsArray(AssetManagerType eAssetType) const;
};

#endif // AssetMimeData_H
