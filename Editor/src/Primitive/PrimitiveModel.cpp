/**************************************************************************
 *	PrimitiveModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "PrimitiveModel.h"

PrimitiveModel::PrimitiveModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
}

PrimitiveModel::~PrimitiveModel()
{
}

/*virtual*/ void PrimitiveModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
}

/*virtual*/ FileDataPair PrimitiveModel::GetStateFileData(uint32 uiIndex) const /*override*/
{
	return FileDataPair();
}

/*virtual*/ QList<AssetItemData *> PrimitiveModel::GetAtlasAssets() const /*override*/
{
	return QList<AssetItemData *>();
}

/*virtual*/ QStringList PrimitiveModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
