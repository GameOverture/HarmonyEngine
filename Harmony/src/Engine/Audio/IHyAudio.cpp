/**************************************************************************
 *	IHyAudio.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/IHyAudio.h"
#include "Renderer/Components/HyWindow.h"

IHyAudio::IHyAudio(std::vector<HyWindow *> &windowListRef) :	m_WindowListRef(windowListRef)
{
}

IHyAudio::~IHyAudio(void)
{
}
