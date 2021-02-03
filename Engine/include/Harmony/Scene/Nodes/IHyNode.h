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
class HyScene;

class IHyNode
{
	friend class HyScene;
	friend class HyEntity2d;
	friend class HyAnimFloat;
	friend class IHyBody;

protected:
	static HyScene *				sm_pScene;

	// These flags describe this Node's C++ object type and are set upon construction, and are then immutable
	enum TypeFlag
	{
		NODETYPE_HyType				= 0xF,		// First 4 bits are reserved for the HyType
		NODETYPE_Is2d				= 1 << 4,
		NODETYPE_IsLoadable			= 1 << 5,
		NODETYPE_IsDrawable			= 1 << 6,
	};
	// These flags get set by HyAnimFloat's when their corresponding respective values are manipulated
	enum DirtyFlag // NOTE: continue the bits in TypeFlag (stored in same 32bit member 'm_uiFlags')
	{
		DIRTY_FromUpdater			= 1 << 7,
		DIRTY_BoundingVolume		= 1 << 8,
		DIRTY_WorldAABB				= 1 << 9,
		DIRTY_Position				= 1 << 10,
		DIRTY_Rotation				= 1 << 11,
		DIRTY_Scale					= 1 << 12,
		DIRTY_Color					= 1 << 13,
		DIRTY_Scissor				= 1 << 14,
		DIRTY_Audio					= 1 << 15,

		DIRTY_ALL =					DIRTY_FromUpdater | DIRTY_BoundingVolume | DIRTY_WorldAABB | DIRTY_Position | DIRTY_Rotation | DIRTY_Scale | DIRTY_Color | DIRTY_Scissor | DIRTY_Audio
	};
	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlag // NOTE: continue the bits in DirtyFlag (stored in same 32bit member 'm_uiFlags')
	{
		EXPLICIT_Visible			= 1 << 16,
		EXPLICIT_PauseUpdate		= 1 << 17,
		EXPLICIT_Scissor			= 1 << 18,
		EXPLICIT_Stencil			= 1 << 19,
		EXPLICIT_DisplayOrder		= 1 << 20,
		EXPLICIT_CoordinateSystem	= 1 << 21,
	};
	// Various boolean values for node stored in these flags for smaller memory footprint
	enum SettingFlag // NOTE: continue the bits in ExplicitFlag (stored in same 32bit member 'm_uiFlags')
	{
		SETTING_IsRegistered		= 1 << 22,
		SETTING_IsVisible			= 1 << 23,
		SETTING_IsPauseUpdate		= 1 << 24,
	};
	uint32							m_uiFlags;

	std::vector<HyAnimFloat *>		m_ActiveAnimFloatsList;

#ifdef HY_ENABLE_USER_TAGS
	int64_t							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)
#endif

public:
	IHyNode(HyType eNodeType);
	IHyNode(const IHyNode &copyRef);
	IHyNode(IHyNode &&donor) noexcept;
	virtual ~IHyNode();

	IHyNode &operator=(const IHyNode &rhs);
	IHyNode &operator=(IHyNode &&donor);

	HyType GetType() const;
	bool Is2D() const;

	bool IsVisible() const;
	virtual void SetVisible(bool bEnabled);

	bool IsPauseUpdate() const;
	virtual void SetPauseUpdate(bool bUpdateWhenPaused);

#ifdef HY_ENABLE_USER_TAGS
	int64_t GetTag() const;
	void SetTag(int64_t iTag);
#endif

protected:
	uint32 GetInternalFlags() const;

	bool IsRegistered() const;

	virtual void Update();																// Only Scene will invoke this
	
	virtual void _SetVisible(bool bEnabled, bool bIsOverriding);						// Only Entity2d/3d will invoke this
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding);			// Only Entity2d/3d will invoke this

	virtual void SetDirty(uint32 uiDirtyFlags);
	bool IsDirty(uint32 uiDirtyFlags) const;
	void ClearDirty(uint32 uiDirtyFlags);

private:
	void SetRegistered(bool bRegister);
	void InsertActiveAnimFloat(HyAnimFloat *pAnimFloat);								// Only HyAnimFloat will invoke this
};

#endif /* IHyNode_h__ */
