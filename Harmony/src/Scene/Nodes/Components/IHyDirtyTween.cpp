/**************************************************************************
*	IHyDirtyTween.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Components/IHyDirtyTween.h"

IHyDirtyTween::IHyDirtyTween() :	m_bDirty(false)
{
}

IHyDirtyTween::~IHyDirtyTween()
{
}

void IHyDirtyTween::SetDirty()
{
	m_bDirty = true;
}
