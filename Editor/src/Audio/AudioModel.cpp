/**************************************************************************
*	AudioModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "AudioModel.h"

AudioModel::AudioModel(ProjectItemData &itemRef, FileDataPair &itemFileDataRef) :
	IModel(itemRef, itemFileDataRef)
{
}

AudioModel::~AudioModel()
{
}

/*virtual*/ void AudioModel::InsertItemSpecificData(FileDataPair &itemSpecificFileDataOut) /*override*/
{
}

/*virtual*/ FileDataPair AudioModel::GetStateFileData(uint32 uiIndex) const /*override*/
{
	return FileDataPair();
}

/*virtual*/ QList<AssetItemData *> AudioModel::GetAtlasAssets() const /*override*/
{
	return QList<AssetItemData *>();
}

/*virtual*/ QStringList AudioModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
