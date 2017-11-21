/**************************************************************************
*	IHyInst2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Misc/IHyInst2d.h"
#include "Scene/HyScene.h"

IHyInst2d::IHyInst2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent)
{
}

IHyInst2d::~IHyInst2d()
{
}

void IHyInst2d::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
	m_uiExplicitFlags |= EXPLICIT_Enabled;
}

void IHyInst2d::SetPauseUpdate(bool bUpdateWhenPaused)
{
	if(bUpdateWhenPaused)
	{
		if(m_bPauseOverride == false)
			HyScene::AddNode_PauseUpdate(this);
	}
	else
	{
		if(m_bPauseOverride == true)
			HyScene::RemoveNode_PauseUpdate(this);
	}

	m_bPauseOverride = bUpdateWhenPaused;
	m_uiExplicitFlags |= EXPLICIT_PauseUpdate;
}
