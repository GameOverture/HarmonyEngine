/**************************************************************************
 *	HyPrefabData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Nodes/HyPrefabData.h"

HyPrefabData::HyPrefabData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef) :	IHyNodeData(sPath)
{
	//Assimp::Importer importer;
	//m_pAiScene = importer.ReadFile(sPath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	//if(m_pAiScene == nullptr)
	//	return;

	//uint32 uiTexIndex = 0;
	//aiString sTexture;
	//m_pAiScene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, uiTexIndex, &sTexture);
	//m_pAiScene->mMaterials[1]->GetTexture(aiTextureType_DIFFUSE, uiTexIndex, &sTexture);
	//m_pAiScene->mMaterials[2]->GetTexture(aiTextureType_DIFFUSE, uiTexIndex, &sTexture);
	//m_pAiScene->mMaterials[3]->GetTexture(aiTextureType_DIFFUSE, uiTexIndex, &sTexture);
}

HyPrefabData::~HyPrefabData(void)
{
}
