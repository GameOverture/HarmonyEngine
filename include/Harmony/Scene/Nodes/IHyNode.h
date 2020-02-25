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
	friend class IHyDrawable;

protected:
	const HyType					m_eTYPE;

	// These flags describe this Node's C++ object type and are set upon construction, and are immutable
	enum TypeFlag
	{
		NODETYPE_Is2d				= 1 << 0,
		NODETYPE_IsLoadable			= 1 << 1,
		NODETYPE_IsDrawable			= 1 << 2,
	};
	// These flags get set by HyAnimFloat's when their corresponding respective values are manipulated
	enum DirtyFlag // NOTE: continue the bits in TypeFlag (stored in same 32bit member 'm_uiFlags')
	{
		DIRTY_BoundingVolume		= 1 << 3,
		DIRTY_WorldAABB				= 1 << 4,
		DIRTY_Transform				= 1 << 5,
		DIRTY_Color					= 1 << 6,
		DIRTY_Scissor				= 1 << 7,

		DIRTY_ALL =					DIRTY_BoundingVolume | DIRTY_WorldAABB | DIRTY_Transform | DIRTY_Color | DIRTY_Scissor
	};
	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlag // NOTE: continue the bits in DirtyFlag (stored in same 32bit member 'm_uiFlags')
	{
		EXPLICIT_Visible			= 1 << 8,
		EXPLICIT_PauseUpdate		= 1 << 9,
		EXPLICIT_Scissor			= 1 << 10,
		EXPLICIT_Stencil			= 1 << 11,
		EXPLICIT_DisplayOrder		= 1 << 12,
		EXPLICIT_CoordinateSystem	= 1 << 13,
	};
	// Various boolean values for node stored in these flags for smaller memory footprint
	enum SettingFlag // NOTE: continue the bits in ExplicitFlag (stored in same 32bit member 'm_uiFlags')
	{
		SETTING_IsRegistered		= 1 << 14,
		SETTING_IsVisible			= 1 << 15,
		SETTING_IsPauseUpdate		= 1 << 16,
	};
	uint32							m_uiFlags;

	std::vector<HyAnimFloat *>		m_ActiveAnimFloatsList;

#ifdef HY_ENABLE_USER_TAGS
	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)
#endif

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

#ifdef HY_ENABLE_USER_TAGS
	int64 GetTag() const;
	void SetTag(int64 iTag);
#endif

protected:
	virtual void Update();																// Only Scene will invoke this
	
	virtual void _SetVisible(bool bEnabled, bool bIsOverriding);						// Only Entity2d/3d will invoke this
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding);			// Only Entity2d/3d will invoke this

	virtual void SetDirty(uint32 uiDirtyFlags);
	bool IsDirty(DirtyFlag eDirtyType) const;
	void ClearDirty(DirtyFlag eDirtyType);

private:
	void InsertActiveAnimFloat(HyAnimFloat *pAnimFloat);								// Only HyAnimFloat will invoke this
};

#endif /* IHyNode_h__ */
