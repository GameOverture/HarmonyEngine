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

PrimitiveModel::PrimitiveModel(ProjectItem &itemRef) : IModel(itemRef)
{
}

PrimitiveModel::~PrimitiveModel()
{
}

/*virtual*/ void PrimitiveModel::OnSave() /*override*/
{
}

/*virtual*/ QJsonObject PrimitiveModel::PopStateAt(uint32 uiIndex) /*override*/
{
	return QJsonObject();
}

/*virtual*/ QJsonValue PrimitiveModel::GetJson() const /*override*/
{
	return QJsonValue();
}

/*virtual*/ QList<AtlasFrame *> PrimitiveModel::GetAtlasFrames() const /*override*/
{
	return QList<AtlasFrame *>();
}

/*virtual*/ QStringList PrimitiveModel::GetFontUrls() const /*override*/
{
	return QStringList();
}

/*virtual*/ void PrimitiveModel::Refresh() /*override*/
{
}
