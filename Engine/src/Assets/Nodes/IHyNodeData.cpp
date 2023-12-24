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

IHyNodeData::IHyNodeData(bool bIsExtrinsic) :
	m_sPATH(""),
	m_uiNumStates(0),
	m_RequiredFiles{ HYFILE_Atlas, HYFILE_GLTF, HYFILE_AudioBank, HYFILE_Shader }
{ }

IHyNodeData::IHyNodeData(const std::string &sPath) :
	m_sPATH(sPath),
	m_uiNumStates(0),
	m_RequiredFiles{ HYFILE_Atlas, HYFILE_GLTF, HYFILE_AudioBank, HYFILE_Shader }
{
	HyAssert(m_sPATH.empty() == false, "IHyNodeData::IHyNodeData() 'sPath' cannot be empty");
}

/*virtual*/ IHyNodeData::~IHyNodeData(void)
{ }

bool IHyNodeData::IsExtrinsic() const
{
	return m_sPATH.empty();
}

uint32 IHyNodeData::GetNumStates() const
{
	return m_uiNumStates;
}

const std::string &IHyNodeData::GetPath() const
{ 
	return m_sPATH;
}

const HyFilesManifest *IHyNodeData::GetManifestFiles(HyFileType eFileType) const
{
	if(IsExtrinsic())
		return nullptr;

	return &m_RequiredFiles[eFileType];
}

/*virtual*/ IHyFile *IHyNodeData::GetExtrinsicFile() const
{
	return nullptr;
}
