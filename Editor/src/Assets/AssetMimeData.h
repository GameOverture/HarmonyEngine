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
public:
	AssetMimeData(QList<AssetItemData *> &assetListRef);
	virtual ~AssetMimeData();
};

#endif // AssetMimeData_H
