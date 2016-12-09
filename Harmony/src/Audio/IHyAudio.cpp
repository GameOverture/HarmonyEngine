/**************************************************************************
 *	IHyAudio.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Audio/IHyAudio.h"
#include "Renderer/Viewport/HyWindow.h"

IHyAudio::IHyAudio(std::vector<HyWindow *> &windowListRef) :	m_WindowListRef(windowListRef)
{
}

IHyAudio::~IHyAudio(void)
{
}
