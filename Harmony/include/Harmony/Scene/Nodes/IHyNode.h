/**************************************************************************
*	IHyNode.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNode_h__
#define IHyNode_h__

#include "Afx/HyStdAfx.h"

class HyTweenFloat;

class IHyNode
{
	friend class HyScene;
	friend class HyEntity2d;
	friend class HyTweenFloat;

protected:
	const HyType					m_eTYPE;

	enum HyNodeDirtyFlag
	{
		DIRTY_BoundingVolume	= 1 << 0,
		DIRTY_WorldAABB			= 1 << 1,
		DIRTY_Transform			= 1 << 2,
		DIRTY_Color				= 1 << 3,
		DIRTY_DontCare			= 1 << 4
	};
	uint32							m_uiDirtyFlags;

	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlags
	{
		EXPLICIT_Enabled		= 1 << 0,
		EXPLICIT_PauseUpdate	= 1 << 1,
		EXPLICIT_Scissor		= 1 << 2,
		EXPLICIT_DisplayOrder	= 1 << 3
	};
	uint32							m_uiExplicitFlags;

	std::vector<HyTweenFloat *>		m_ActiveTweenFloatsList;

	bool							m_bEnabled;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

public:
	IHyNode(HyType eNodeType);
	virtual ~IHyNode();

	HyType GetType();

	virtual bool IsEnabled();

	int64 GetTag();
	void SetTag(int64 iTag);

protected:
	virtual void PhysicsUpdate() = 0;
	virtual void NodeUpdate() = 0;
	
	virtual void _SetEnabled(bool bEnabled, bool bIsOverriding);						// Only Entity2d/3d will invoke this
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding);			// Only Entity2d/3d will invoke this

	virtual void SetDirty(uint32 uiDirtyFlags);
	bool IsDirty(HyNodeDirtyFlag eDirtyType);
	void ClearDirty(HyNodeDirtyFlag eDirtyType);

private:
	void InsertActiveTweenFloat(HyTweenFloat *pTweenFloat);								// Only HyTweenFloat will invoke this
	void Update();																		// Only Scene will invoke this
};

#endif /* IHyNode_h__ */
