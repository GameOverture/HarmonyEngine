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

IHyNodeData::IHyNodeData(const HyNodePath &nodePath) :
	m_PATH(nodePath),
	m_uiNumStates(0),
	m_RequiredFiles{ HYFILE_Atlas, HYFILE_GLTF, HYFILE_AudioBank, HYFILE_Shader }
{
}

/*virtual*/ IHyNodeData::~IHyNodeData(void)
{ }

const HyNodePath &IHyNodeData::GetPath() const
{
	return m_PATH;
}

uint32 IHyNodeData::GetNumStates() const
{
	return m_uiNumStates;
}

const HyFilesManifest *IHyNodeData::GetManifestFiles(HyFileType eFileType) const
{
	if(m_PATH.IsAuxiliary())
		return nullptr;

	return &m_RequiredFiles[eFileType];
}

/*virtual*/ IHyFile *IHyNodeData::GetAuxiliaryFile() const
{
	return nullptr;
}
