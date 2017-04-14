/**************************************************************************
*	IHyLeafNode.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Leafs/IHyLeafNode.h"

IHyLeafNode::IHyLeafNode(HyType eNodeType, IHyNode *pParent) :	IHyNode(eNodeType, pParent),
																m_uiExplicitFlags(0)
{
}

IHyLeafNode::~IHyLeafNode()
{
}

void IHyLeafNode::SetEnabled(bool bEnabled)
{
}

void IHyLeafNode::SetPauseUpdate(bool bUpdateWhenPaused)
{
}
