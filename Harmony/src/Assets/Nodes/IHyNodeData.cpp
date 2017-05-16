/**************************************************************************
*	IHyNodeData.cpp
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/

#include "Assets/Nodes/IHyNodeData.h"

IHyNodeData::IHyNodeData(HyType eType, const std::string &sPath) :	m_eTYPE(eType),
																	m_sPATH(sPath)
{ }

/*virtual*/ IHyNodeData::~IHyNodeData(void)
{ }

HyType IHyNodeData::GetInstType()
{
	return m_eTYPE;
}

const std::string &IHyNodeData::GetPath()
{ 
	return m_sPATH;
}

const HyAtlasIndices &IHyNodeData::GetRequiredAtlasIndices()
{
	return m_RequiredAtlasIndices;
}
