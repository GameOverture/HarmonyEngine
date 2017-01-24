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

#include "Afx/HyInteropAfx.h"

/*static*/ glm::vec2 IHyInputMap::sm_ptWorldMousePos(0.0f);
/*static*/ bool IHyInputMap::sm_bMouseLeftDown = false;
/*static*/ bool IHyInputMap::sm_bMouseRightDown = false;

IHyInputMap::IHyInputMap(IHyInput *pInputManager) : m_pInputManager(pInputManager)
{
}

/*virtual*/ IHyInputMap::~IHyInputMap(void)
{
}

/*static*/ glm::vec2 IHyInputMap::GetWorldMousePos()
{
	return sm_ptWorldMousePos;
}

/*static*/ bool IHyInputMap::IsMouseLeftDown()
{
	return sm_bMouseLeftDown;
}

/*static*/ bool IHyInputMap::IsMouseRightDown()
{
	return sm_bMouseRightDown;
}

void IHyInputMap::StartRecording()
{
	m_pInputManager->StartRecording();
}

void IHyInputMap::StopRecording()
{
	m_pInputManager->StopRecording();
}

void IHyInputMap::SerializeRecording()
{
	m_pInputManager->SerializeRecording();
}

void IHyInputMap::StartPlayback()
{
	m_pInputManager->StartPlayback();
}

void IHyInputMap::StopPlayback()
{
	m_pInputManager->StartPlayback();
}
