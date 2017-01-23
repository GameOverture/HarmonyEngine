/**************************************************************************
 *	IHyInputMap.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Input/IHyInputMap.h"
#include "Input/IHyInput.h"

IHyInputMap::IHyInputMap(IHyInput *pInputManager) : m_pInputManager(pInputManager)
{
}

/*virtual*/ IHyInputMap::~IHyInputMap(void)
{
}

/*static*/ void IHyInputMap::StartRecording()
{
	m_pInputManager->StartRecording();
}

/*static*/ void IHyInputMap::StopRecording()
{
	m_pInputManager->StopRecording();
}

/*static*/ void IHyInputMap::SerializeRecording()
{
	m_pInputManager->SerializeRecording();
}

/*static*/ void IHyInputMap::StartPlayback()
{
	m_pInputManager->StartPlayback();
}

/*static*/ void IHyInputMap::StopPlayback()
{
	m_pInputManager->StartPlayback();
}
