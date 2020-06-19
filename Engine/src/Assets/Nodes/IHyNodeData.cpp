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
	m_RequiredAtlases(HYFILE_Atlas),
	m_RequiredAudio(HYFILE_AudioBank),
	m_pGltf(nullptr)
{ }

/*virtual*/ IHyNodeData::~IHyNodeData(void)
{ }

const std::string &IHyNodeData::GetPath() const
{ 
	return m_sPATH;
}

const HyFilesManifest &IHyNodeData::GetRequiredAtlases() const
{
	return m_RequiredAtlases;
}

const HyFilesManifest &IHyNodeData::GetRequiredAudio() const
{
	return m_RequiredAudio;
}

HyGLTF *IHyNodeData::GetGltf() const
{
	return m_pGltf;
}
