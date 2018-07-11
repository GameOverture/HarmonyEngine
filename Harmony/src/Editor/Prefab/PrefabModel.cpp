/**************************************************************************
 *	PrefabModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "PrefabModel.h"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/Exporter.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

PrefabModel::PrefabModel(ProjectItem &itemRef, QJsonValue initValue) : IModel(itemRef)
{
	// If initValue is just a string, then it represents the import file
	if(initValue.isString())
	{
		Assimp::Importer importer;
		const aiScene *pScene = importer.ReadFile(initValue.toString().toStdString(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		pScene->HasMeshes();
	}
}

/*virtual*/ void PrefabModel::OnSave() /*override*/
{
}

/*virtual*/ QJsonObject PrefabModel::PopStateAt(uint32 uiIndex) /*override*/
{
	return QJsonObject();
}

/*virtual*/ QJsonValue PrefabModel::GetJson() const /*override*/
{
	return QJsonValue();
}

/*virtual*/ QList<AtlasFrame *> PrefabModel::GetAtlasFrames() const /*override*/
{
	return QList<AtlasFrame *>();
}

/*virtual*/ QStringList PrefabModel::GetFontUrls() const /*override*/
{
	return QStringList();
}

/*virtual*/ void PrefabModel::Refresh() /*override*/
{
}
