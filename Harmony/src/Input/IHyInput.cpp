/**************************************************************************
 *	HyInput.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Input/IHyInput.h"

IHyInput::IHyInput(vector<IHyInputMap> &vInputMapsRef) :	m_vInputMapsRef(vInputMapsRef),
															m_eReplayState(REPLAY_Off),
															m_uiRecordCount(0)
{
}

IHyInput::~IHyInput(void)
{
}

void IHyInput::Update()
{
	ProcessInput();
}
