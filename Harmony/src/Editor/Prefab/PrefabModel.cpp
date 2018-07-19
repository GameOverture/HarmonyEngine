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
#include "AtlasWidget.h"

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

		// Import the DCC file from artist
		Assimp::Importer importer;
		const aiScene *pScene = importer.ReadFile(sInitValue.toStdString(), aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
		if(pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			HyGuiLog(sInitValue + " was incomplete and is invalid", LOGTYPE_Error);
			return;
		}

		// Ensure the destination directory exists (<DataDir>/Prefabs/<prefix>/)
		QDir dataDir(itemRef.GetProject().GetAssetsAbsPath() % HyGlobal::ItemName(ITEM_Prefab, true));
		if(false == dataDir.mkpath(itemRef.GetPrefix()))
		{
			HyGuiLog("PrefabModel::PrefabModel() - Creating directory path failed: " % itemRef.GetPrefix(), LOGTYPE_Error);
			return;
		}
		QDir prefabDir(dataDir.absolutePath() % "/" % itemRef.GetPrefix());
		QString sAbsFilePath = prefabDir.absoluteFilePath(itemRef.GetName(false) % ".gltf");

		// Using the imported DCC file, export it to a glTF 2.0 at the above destination
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

		// Open and parse the newly exported glTF file. Any image dependencies should be imported into the project's atlas manager.
		// Modify the glTF file to also include the "checksum" of these imported images, so they can be queried by Harmony Engine.
		QFile gltfFile(sAbsFilePath);
		if(gltfFile.exists())
		{
			if(!gltfFile.open(QIODevice::ReadOnly))
				HyGuiLog(QString("PrefabModel::PrefabModel() could not open ") % sAbsFilePath, LOGTYPE_Error);

			QJsonDocument gltfDoc = QJsonDocument::fromJson(gltfFile.readAll());
			QJsonObject gltfObj = gltfDoc.object();

			// Find all the required image dependencies and store their file paths in 'sImageFilePaths'
			QStringList sImageFilePaths;
			QList<quint32> imageChecksumList;
			QJsonArray imagesArray = gltfObj["images"].toArray();
			for(int i = 0; i < imagesArray.size(); ++i)
			{
				QJsonObject imageObj = imagesArray[i].toObject();
				if(imageObj.find("uri") != imageObj.end())
				{
					QFileInfo importFileInfo(sInitValue);
					sImageFilePaths.append(importFileInfo.absolutePath() % "/" % imageObj["uri"].toString());
				}
				else if(imageObj.find("bufferView") != imageObj.end())
					HyGuiLog("TODO: glTF files which store their images in the accompanied binary blob file are not yet supported. They should either use a \"uri\" reference or once implemented, be stripped out after importing.", LOGTYPE_Info);
				else
					HyGuiLog("PrefabModel::PrefabModel() - while importing, glTF images did not have either a \"uri\" or a \"bufferView\": " % sAbsFilePath, LOGTYPE_Error);
			}

			if(sImageFilePaths.size() != imagesArray.size())
				HyGuiLog("PrefabModel::PrefabModel() - sImageFilePaths.size() != imagesArray.size(): " % sAbsFilePath, LOGTYPE_Error);
			else
			{
				quint32 uiChecksum;

				// Import images into the selected atlas group, or default one
				quint32 uiAtlasGrpId = itemRef.GetProject().GetAtlasModel().GetAtlasGrpIdFromAtlasGrpIndex(0);
				if(itemRef.GetProject().GetAtlasWidget())
					uiAtlasGrpId = itemRef.GetProject().GetAtlasWidget()->GetSelectedAtlasGrpId();

				QList<AtlasTreeItem *> correspondingParentList;
				for(int i = 0; i < sImageFilePaths.size(); ++i)
					correspondingParentList.push_back(nullptr);
				
				QSet<AtlasFrame *> newlyImportedAtlasFrameSet = itemRef.GetProject().GetAtlasModel().ImportImages(sImageFilePaths, uiAtlasGrpId, ITEM_Prefab, correspondingParentList);
				if(newlyImportedAtlasFrameSet.empty() == false)
					itemRef.GetProject().GetAtlasModel().Repack(itemRef.GetProject().GetAtlasModel().GetAtlasGrpIndexFromAtlasGrpId(uiAtlasGrpId), QSet<int>(), newlyImportedAtlasFrameSet);

				// START HERE:
				//imageObj.insert("checksum", QJsonValue(static_cast<qint64>(uiChecksum)));
				//imagesArray[i] = imageObj;

				//gltfObj.insert("images", imagesArray);
			}
		}
		else
			HyGuiLog("PrefabModel::PrefabModel() could not find the exported gltf file: " % sAbsFilePath, LOGTYPE_Error);
	}
	else // Prefab/glTF already exists for Harmony
	{
		//sInitValue
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
