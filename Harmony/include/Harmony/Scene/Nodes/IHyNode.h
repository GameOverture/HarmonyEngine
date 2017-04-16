/**************************************************************************
*	IHyNode.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyNode_h__
#define __IHyNode_h__

#include "Afx/HyStdAfx.h"

class HyTweenFloat;

enum HyNodeDirtyType
{
	HYNODEDIRTY_Transform	= 1 << 0,
	HYNODEDIRTY_Color		= 1 << 1,
	HYNODEDIRTY_DontCare	= 1 << 2
};

class IHyNode
{
	friend class HyScene;
	friend class HyEntity2d;
	friend class HyTweenFloat;

protected:
	const HyType					m_eTYPE;

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

	
	uint32							m_uiTweenDirtyFlags;
	std::vector<HyTweenFloat *>		m_ActiveTweenFloatsList;

	bool							m_bEnabled;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

public:
	IHyNode(HyType eNodeType);
	virtual ~IHyNode();

	HyType GetType();

	bool IsEnabled();

	int64 GetTag();
	void SetTag(int64 iTag);

protected:
	virtual void NodeUpdate() = 0;
	
	virtual void _SetEnabled(bool bEnabled, bool bIsOverriding);						// Only Entity2d/3d will invoke this
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bIsOverriding);			// Only Entity2d/3d will invoke this

	virtual void SetDirty(HyNodeDirtyType eDirtyType);
	bool IsDirty(HyNodeDirtyType eDirtyType);
	void ClearDirty(HyNodeDirtyType eDirtyType);

private:
	void InsertActiveTweenFloat(HyTweenFloat *pTweenFloat);									// Only HyTweenFloat will invoke this
	void Update();																			// Only Scene will invoke this
};

#endif /* __IHyNode_h__ */
