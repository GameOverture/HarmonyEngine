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

AudioModel::AudioModel(ProjectItem &itemRef) :
	IModel(itemRef)
{
}

AudioModel::~AudioModel()
{
}

/*virtual*/ QJsonObject AudioModel::GetStateJson(uint32 uiIndex) const /*override*/
{
	return QJsonObject();
}

/*virtual*/ QJsonValue AudioModel::GetJson() const /*override*/
{
	return QJsonValue();
}

/*virtual*/ QList<AtlasFrame *> AudioModel::GetAtlasFrames() const /*override*/
{
	return QList<AtlasFrame *>();
}

/*virtual*/ QStringList AudioModel::GetFontUrls() const /*override*/
{
	return QStringList();
}
