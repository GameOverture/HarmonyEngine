/**************************************************************************
*	IHyNode2d.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyTransformNode_h__
#define __IHyTransformNode_h__

#include "Afx/HyStdAfx.h"
#include "Scene/HyScene.h"

#include <functional>

class IHyNode2d
{
	friend class HyScene;
	friend class HyTweenFloat;

protected:
	const HyType					m_eTYPE;

	bool							m_bDirty;
	bool							m_bIsDraw2d;
	bool							m_bEnabled;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	IHyNode2d *						m_pParent;
	std::vector<IHyNode2d *>			m_ChildList;

	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlags
	{
		EXPLICIT_Enabled		= 1 << 0,
		EXPLICIT_PauseUpdate	= 1 << 1,
		EXPLICIT_DisplayOrder	= 1 << 2,
		EXPLICIT_Tint			= 1 << 3,
		EXPLICIT_Alpha			= 1 << 4,
	};
	uint32							m_uiExplicitFlags;

	int64							m_iTag;				// This 'tag' isn't used by the engine, and solely used for whatever purpose the client wishes (tracking, unique ID, etc.)

	std::vector<HyTweenFloat *>		m_ActiveAnimFloatsList;

public:
	IHyNode2d(HyType eNodeType, IHyNode2d *pParent);
	virtual ~IHyNode2d();
	
	HyType GetType();
	bool IsDraw2d();

	bool IsEnabled();
	void SetEnabled(bool bEnabled, bool bOverrideExplicitChildren = true);

	// Sets whether to Update when game is paused
	void SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren = true);

	int64 GetTag();
	void SetTag(int64 iTag);

	void ChildAppend(IHyNode2d &childInst);
	bool ChildInsert(IHyNode2d &insertBefore, IHyNode2d &childInst);
	bool ChildFind(IHyNode2d &childInst);
	void ChildrenTransfer(IHyNode2d &newParent);
	uint32 ChildCount();
	IHyNode2d *ChildGet(uint32 uiIndex);

	void ParentDetach();
	bool ParentExists();

	void ForEachChild(std::function<void(IHyNode2d *)> func);

protected:
	void Update();
	virtual void InstUpdate() = 0;

	void SetDirty();
	void InsertActiveAnimFloat(HyTweenFloat *pAnimFloat);

	virtual void SetNewChildAttributes(IHyNode2d &childInst);

private:
	void _SetEnabled(bool bEnabled, bool bOverrideExplicitChildren);
	void _SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren);

};

#endif /* __IHyTransformNode_h__ */
