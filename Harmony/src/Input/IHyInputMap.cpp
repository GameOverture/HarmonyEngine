/**************************************************************************
 *	IHyInputMap.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Input/IHyInputMap.h"
#include "Input/IHyInput.h"
#include "Renderer/Components/HyWindow.h"

/*static*/ IHyInput *IHyInputMap::sm_pInputManager = nullptr;

IHyInputMap::IHyInputMap()
{
}

/*virtual*/ IHyInputMap::~IHyInputMap(void)
{
}

/*static*/ glm::vec2 IHyInputMap::GetWorldMousePos()
{
	return sm_pInputManager->GetWorldMousePos();
}

/*static*/ bool IHyInputMap::IsMouseLeftDown()
{
	return sm_pInputManager->IsMouseLeftDown();
}

/*static*/ bool IHyInputMap::IsMouseRightDown()
{
	return sm_pInputManager->IsMouseRightDown();
}

void IHyInputMap::StartRecording()
{
	sm_pInputManager->StartRecording();
}

void IHyInputMap::StopRecording()
{
	sm_pInputManager->StopRecording();
}

void IHyInputMap::SerializeRecording()
{
	sm_pInputManager->SerializeRecording();
}

void IHyInputMap::StartPlayback()
{
	sm_pInputManager->StartPlayback();
}

void IHyInputMap::StopPlayback()
{
	sm_pInputManager->StartPlayback();
}
