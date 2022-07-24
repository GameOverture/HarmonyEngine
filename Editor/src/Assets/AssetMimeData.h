/**************************************************************************
*	AssetMimeData.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef AssetMimeData_H
#define AssetMimeData_H

#include "Global.h"
#include "IMimeData.h"

class AssetMimeData : public IMimeData
{
	uint32				m_AssetCounts[NUMASSETTYPES];

public:
	AssetMimeData(QList<AssetItemData *> &assetListRef, AssetType eAssetType);
	virtual ~AssetMimeData();

	uint32 GetNumAssetsOfType(AssetType eAssetType) const;
	QJsonArray GetAssetsArray(AssetType eAssetType) const;
};

#endif // AssetMimeData_H
