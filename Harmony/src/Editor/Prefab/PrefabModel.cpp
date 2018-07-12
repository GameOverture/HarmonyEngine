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
		if(pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			HyGuiLog(initValue.toString() + " was incomplete and is invalid", LOGTYPE_Error);
			return;
		}

		//  http://ogldev.atspace.co.uk/index.html

		m_MeshList.reserve(pScene->mNumMeshes);
		m_sTextureList.reserve(pScene->mNumMaterials);

		// Rip all required textures from this prefab
		if(pScene->HasTextures())
		{
		}

		// GetTexture Filenames and Numb of Textures
		for(uint i = 0; i < pScene->mNumMaterials; ++i)
		{
			uint uiTexIndex = 0;
			aiReturn texFound = AI_SUCCESS;
			aiString sFilePath;
			while(true)
			{
				texFound = pScene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, uiTexIndex, &sFilePath);
				if(texFound == AI_SUCCESS)
				{
					m_sTextureDiffuseList.append(sFilePath.data);
					uiTexIndex++;
				}
				else
					break;
			}
		}

		//int iNumTextures = m_sTextureFilePathList.size();
		//iNumTextures = iNumTextures;
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
