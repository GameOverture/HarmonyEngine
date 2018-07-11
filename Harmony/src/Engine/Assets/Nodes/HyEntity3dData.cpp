/**************************************************************************
 *	HyEntity3dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Nodes/HyEntity3dData.h"

HyEntity3dData::HyEntity3dData(const std::string &sPath) :	IHyNodeData(HYTYPE_Entity3d, sPath)
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

HyEntity3dData::~HyEntity3dData(void)
{
}
