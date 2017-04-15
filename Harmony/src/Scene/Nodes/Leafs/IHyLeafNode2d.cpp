/**************************************************************************
*	IHyLeafNode2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Leafs/IHyLeafNode2d.h"
#include "Scene/HyScene.h"

IHyLeafNode2d::IHyLeafNode2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent)
{
}

IHyLeafNode2d::~IHyLeafNode2d()
{
}

void IHyLeafNode2d::SetEnabled(bool bEnabled)
{
	m_bEnabled = bEnabled;
	m_uiExplicitFlags |= EXPLICIT_Enabled;
}

void IHyLeafNode2d::SetPauseUpdate(bool bUpdateWhenPaused)
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
