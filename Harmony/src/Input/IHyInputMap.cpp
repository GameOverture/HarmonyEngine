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

/*static*/ IHyInput *IHyInputMap::sm_pInputManager = NULL;

IHyInputMap::IHyInputMap()
{
}

/*virtual*/ IHyInputMap::~IHyInputMap(void)
{
}

/*static*/ void IHyInputMap::SetManagerPtr(IHyInput *pInputManager)
{
	sm_pInputManager = pInputManager;
}

/*static*/ void IHyInputMap::StartRecording()
{
	sm_pInputManager->StartRecording();
}

/*static*/ void IHyInputMap::StopRecording()
{
	sm_pInputManager->StopRecording();
}

/*static*/ void IHyInputMap::SerializeRecording()
{
	sm_pInputManager->SerializeRecording();
}

/*static*/ void IHyInputMap::StartPlayback()
{
	sm_pInputManager->StartPlayback();
}

/*static*/ void IHyInputMap::StopPlayback()
{
	sm_pInputManager->StartPlayback();
}
