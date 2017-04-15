/**************************************************************************
*	IHyNode2d.h
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyNode2d_h__
#define __IHyNode2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenVec2.h"
#include "Scene/Physics/HyBoundingVolume2d.h"

#include <functional>

class HyEntity2d;

class IHyNode2d : public IHyNode
{
	friend class HyEntity2d;

protected:
	bool							m_bDirty;
	bool							m_bIsDraw2d;
	bool							m_bPauseOverride;	// Will continue to Update when game is paused

	HyEntity2d *					m_pParent;

	// When directly manipulating a node, store a flag to indicate that this attribute has been explicitly set. If later 
	// changes occur to a parent of this node, it may optionally ignore the change when it propagates down the child hierarchy.
	enum ExplicitFlags
	{
		EXPLICIT_Enabled		= 1 << 0,
		EXPLICIT_PauseUpdate	= 1 << 1,
		EXPLICIT_DisplayOrder	= 1 << 2
	};
	uint32							m_uiExplicitFlags;

	glm::mat4						m_mtxCached;
	HyCoordinateUnit				m_eCoordUnit;

	float							m_fRotation;		// Reference value used in 'rot' HyTweenFloat

	HyBoundingVolume2d				m_BoundingVolume;

public:
	HyTweenVec2						pos;
	HyTweenFloat					rot;
	HyTweenVec2						rot_pivot;
	HyTweenVec2						scale;
	HyTweenVec2						scale_pivot;

public:
	IHyNode2d(HyType eNodeType, HyEntity2d *pParent);
	virtual ~IHyNode2d();
	
	bool IsDraw2d();

	void ParentDetach();
	bool ParentExists();

	HyCoordinateUnit GetCoordinateUnit();
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	void GetLocalTransform(glm::mat4 &outMtx) const;
	void GetWorldTransform(glm::mat4 &outMtx);

protected:
	virtual void SetNewChildAttributes(IHyNode2d &childInst);

private:
	virtual void SetDirty() override;

	virtual void _SetEnabled(bool bEnabled, bool bOverrideExplicitChildren);
	virtual void _SetPauseUpdate(bool bUpdateWhenPaused, bool bOverrideExplicitChildren);
};

#endif /* __IHyNode2d_h__ */
