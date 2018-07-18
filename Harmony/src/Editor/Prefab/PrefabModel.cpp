/**************************************************************************
 *	PrefabModel.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2018 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#define TINYGLTF_IMPLEMENTATION
#include "PrefabModel.h"
#include "Project.h"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/Exporter.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

PrefabModel::PrefabModel(ProjectItem &itemRef, QJsonValue initValue) : IModel(itemRef)
{
	// initValue is just a string, that is either prefixed with HYGUI_ImportPrefix indicating to create a new prefab asset, or is the prefix/name of an existing prefab
	if(initValue.isString() == false)
	{
		HyGuiLog("PrefabModel was given improper initValue", LOGTYPE_Error);
		return;
	}

	QString sInitValue = initValue.toString();
	if(sInitValue.contains(HYGUI_ImportPrefix))	// Importing a new prefab asset
	{
		sInitValue = sInitValue.remove(HYGUI_ImportPrefix);

		Assimp::Importer importer;
		const aiScene *pScene = importer.ReadFile(sInitValue.toStdString(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if(pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			HyGuiLog(sInitValue + " was incomplete and is invalid", LOGTYPE_Error);
			return;
		}

		QDir dataDir(itemRef.GetProject().GetAssetsAbsPath() % HyGlobal::ItemName(ITEM_Prefab, true));
		if(false == dataDir.mkpath(itemRef.GetPrefix()))
		{
			HyGuiLog("PrefabModel::PrefabModel() - Creating directory path failed: " % itemRef.GetPrefix(), LOGTYPE_Error);
			return;
		}
		QDir prefabDir(dataDir.absolutePath() % "/" % itemRef.GetPrefix());
		QString sAbsFilePath = prefabDir.absoluteFilePath(itemRef.GetName(false) % ".gltf");

		Assimp::Exporter exporter;
		const aiExportFormatDesc *pDesc = nullptr;
		for(uint i = 0; i < exporter.GetExportFormatCount(); ++i)
		{
			pDesc = exporter.GetExportFormatDescription(i);
			if(strcmp(pDesc->description, "GL Transmission Format v. 2") == 0)
				break;
		}
		aiReturn ret = exporter.Export(pScene, pDesc->id, sAbsFilePath.toStdString());
		if(ret != aiReturn_SUCCESS)
		{
			HyGuiLog(sAbsFilePath + " failed to export to glTF", LOGTYPE_Error);
			return;
		}

		tinygltf::Model model;
		tinygltf::TinyGLTF loader;
		std::string err;
		bool bTinyGltfLoaded = loader.LoadASCIIFromFile(&model, &err, sAbsFilePath.toStdString());
		if(bTinyGltfLoaded == false)
		{
			HyGuiLog("tinygltf::TinyGLTF failed to load: " % sAbsFilePath, LOGTYPE_Error);
			return;
		}
	}
}

//void PrefabModel::CopyNodesWithMeshes( aiNode node, SceneObject targetParent, Matrix4x4 accTransform)
//{
//	SceneObject parent;
//	Matrix4x4 transform;

//	// if node has meshes, create a new scene object for it
//	if(node.mNumMeshes > 0)
//	{
//		SceneObjekt newObject = new SceneObject;
//		targetParent.addChild( newObject);
//		// copy the meshes
//		CopyMeshes( node, newObject);
//		// the new object is the parent for all child nodes
//		parent = newObject;
//		transform.SetUnity();
//	}
//	else
//	{
//		// if no meshes, skip the node, but keep its transformation
//		parent = targetParent;
//		transform = node.mTransformation * accTransform;
//	}

//	// continue for all child nodes
//	for( all node.mChildren)
//	CopyNodesWithMeshes( node.mChildren[a], parent, transform);
//}

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
