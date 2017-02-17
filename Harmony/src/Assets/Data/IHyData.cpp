/**************************************************************************
*	IHyData.cpp
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/

#include "Assets/Data/IHyData.h"

IHyData::IHyData(HyType eType, const std::string &sPath) :	m_eTYPE(eType),
															m_sPATH(sPath)
{ }

/*virtual*/ IHyData::~IHyData(void)
{ }

HyType IHyData::GetInstType()
{
	return m_eTYPE;
}

const std::string &IHyData::GetPath()
{ 
	return m_sPATH;
}
