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
	m_uiNumStates(0),
	m_RequiredAtlases(HYFILE_Atlas),
	m_RequiredAudio(HYFILE_AudioBank),
	m_pGltf(nullptr)
{ }

/*virtual*/ IHyNodeData::~IHyNodeData(void)
{ }

uint32 IHyNodeData::GetNumStates() const
{
	return m_uiNumStates;
}

const std::string &IHyNodeData::GetPath() const
{ 
	return m_sPATH;
}

const HyFilesManifest *IHyNodeData::GetRequiredFiles(HyFileType eFileType) const
{
	if(eFileType == HYFILE_Atlas)
		return &m_RequiredAtlases;
	else if(eFileType == HYFILE_AudioBank)
		return &m_RequiredAudio;

	return nullptr;
}

HyGLTF *IHyNodeData::GetGltf() const
{
	return m_pGltf;
}
