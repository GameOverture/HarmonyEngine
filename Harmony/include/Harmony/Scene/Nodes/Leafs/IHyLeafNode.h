/**************************************************************************
*	IHyLeafNode.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyLeafNode_h__
#define __IHyLeafNode_h__

#include "Scene/Nodes/IHyNode.h"

class IHyLeafNode : public IHyNode
{
	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlags
	{
		EXPLICIT_Enabled = 1 << 0,
		EXPLICIT_PauseUpdate = 1 << 1,
		EXPLICIT_DisplayOrder = 1 << 2,
		EXPLICIT_Tint = 1 << 3,
		EXPLICIT_Alpha = 1 << 4,
	};
	uint32							m_uiExplicitFlags;

public:
	IHyLeafNode(HyType eNodeType, IHyNode *pParent);
	virtual ~IHyLeafNode();

	void SetEnabled(bool bEnabled);
	void SetPauseUpdate(bool bUpdateWhenPaused);
};

#endif /* __IHyLeafNode_h__ */
