/**************************************************************************
*	IHyNodeData.cpp
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/IHyNodeData.h"

IHyNodeData::IHyNodeData(const std::string &sPath) :
	m_sPATH(sPath),
	m_RequiredAtlasIndices(HYFILE_Atlas),
	m_pGltf(nullptr),
	m_pAudioBank(nullptr)
{ }

/*virtual*/ IHyNodeData::~IHyNodeData(void)
{ }

const std::string &IHyNodeData::GetPath() const
{ 
	return m_sPATH;
}

const HyFilesManifest &IHyNodeData::GetRequiredAtlasIndices() const
{
	return m_RequiredAtlasIndices;
}

HyGLTF *IHyNodeData::GetGltf() const
{
	return m_pGltf;
}

HyAudioBank *IHyNodeData::GetAudioBank() const
{
	return m_pAudioBank;
}
