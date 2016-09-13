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

IHyData::IHyData(HyDataType eDataType, HyInstanceType eInstType, const std::string &sPath) :	m_eDATATYPE(eDataType),
																								m_eINSTTYPE(eInstType),
																								m_sFILEPATH(sPath),
																								m_eLoadState(HYLOADSTATE_Inactive),
																								m_iRefCount(0)
{ }

/*virtual*/ IHyData::~IHyData(void)
{ }

HyDataType IHyData::GetDataType()
{ 
	return m_eDATATYPE;
}

HyInstanceType IHyData::GetInstType()
{
	return m_eINSTTYPE;
}

const std::string &IHyData::GetPath()
{ 
	return m_sFILEPATH;
}

/*virtual*/ void IHyData::SetLoadState(HyLoadState eState)
{ 
	m_eLoadState = eState;
}

HyLoadState IHyData::GetLoadState()
{
	return m_eLoadState;
}

void IHyData::IncRef()
{ 
	m_iRefCount++;
}

bool IHyData::DecRef()
{
	m_iRefCount--;
	return m_iRefCount <= 0;
}

int32 IHyData::GetRefCount()
{
	return m_iRefCount;
}

/*virtual*/ void IHyData::OnLoadThread()
{
	DoFileLoad();
}
