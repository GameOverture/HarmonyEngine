/**************************************************************************
*	IHyNode.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNode_h__
#define IHyNode_h__

#include "Afx/HyStdAfx.h"

class HyAnimFloat;

class IHyNode
{
	friend class HyScene;
	friend class HyEntity2d;
	friend class HyAnimFloat;
	friend class IHyVisable;

protected:
	const HyType					m_eTYPE;

	enum NodeDirtyFlag
	{
		DIRTY_BoundingVolume		= 1 << 0,
		DIRTY_WorldAABB				= 1 << 1,
		DIRTY_Transform				= 1 << 2,
		DIRTY_Color					= 1 << 3,
		DIRTY_Scissor				= 1 << 4,

		DIRTY_ALL =					DIRTY_BoundingVolume | DIRTY_WorldAABB | DIRTY_Transform | DIRTY_Color | DIRTY_Scissor
	};
	uint32							m_uiDirtyFlags;

	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlags
	{
		EXPLICIT_Visible			= 1 << 0,
		EXPLICIT_PauseUpdate		= 1 << 1,
		EXPLICIT_Scissor			= 1 << 2,
		EXPLICIT_Stencil			= 1 << 3,
		EXPLICIT_DisplayOrder		= 1 << 4,
		EXPLICIT_CoordinateSystem	= 1 << 5,
	};
	enum NodeTypeFlags // NOTE: continue the bits in ExplicitFlags (stored in same 32bit member)
	{
		NODETYPE_Is2d				= 1 << 6,
		NODETYPE_IsLoadable			= 1 << 7,
		NODETYPE_IsDrawable			= 1 << 8,
		NODETYPE_IsVisable			= 1 << 9,
	};
	uint32							m_uiExplicitAndTypeFlags;

	std::vector<HyAnimFloat *>		m_ActiveAnimFloatsList;

	bool							m_bVisible;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

	// Don't allow move semantics since pointers to these nodes are stored in things like HyScene, and those pointers would become invalid
	IHyNode(IHyNode &&moveRef) HY_NOEXCEPT = delete;
	IHyNode &operator=(IHyNode &&moveRef) HY_NOEXCEPT = delete;

public:
	IHyNode(HyType eNodeType);
	IHyNode(const IHyNode &copyRef);
	virtual ~IHyNode();

	const IHyNode &operator=(const IHyNode &rhs);

	HyType GetType() const;
	bool Is2D() const;
	uint32 GetExplicitAndTypeFlags() const;

	bool IsVisible() const;
	virtual void SetVisible(bool bEnabled);

	bool IsPauseUpdate() const;
	virtual void SetPauseUpdate(bool bUpdateWhenPaused);

	int64 GetTag() const;
	void SetTag(int64 iTag);

protected:
	virtual void Update();																// Only Scene will invoke this
	
	virtual void _SetVisible(bool bEnabled, bool bIsOverriding);						// Only Entity2d/3d will invoke this
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding);			// Only Entity2d/3d will invoke this

	virtual void SetDirty(uint32 uiDirtyFlags);
	bool IsDirty(NodeDirtyFlag eDirtyType);
	void ClearDirty(NodeDirtyFlag eDirtyType);

private:
	void InsertActiveAnimFloat(HyAnimFloat *pAnimFloat);								// Only HyAnimFloat will invoke this
};

#endif /* IHyNode_h__ */
