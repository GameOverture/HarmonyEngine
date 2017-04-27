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
#include "Scene/Tweens/HyTweenVec2.h"
#include "Scene/Physics/HyBoundingVolume2d.h"

#include <functional>

class HyEntity2d;

class IHyNode2d : public IHyNode
{
	friend class HyEntity2d;

protected:
	HyEntity2d *					m_pParent;

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

	void ParentDetach();
	HyEntity2d *ParentGet();

	HyCoordinateUnit GetCoordinateUnit();
	// TODO: This needs to apply to everything in its hierarchy
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	void GetLocalTransform(glm::mat4 &outMtx) const;
	void GetWorldTransform(glm::mat4 &outMtx);

protected:
	virtual void NodeUpdate() = 0;

	// '_' functions are used to propagate values down from parent, and are overridden with proper functionality later in hierarchy
	virtual void _SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding)	{ }
	virtual int32 _SetDisplayOrder(int32 iOrderValue, bool bIsOverriding)							{ return iOrderValue; }

private:
	virtual bool IsLoaded() const { return true; }
	virtual void Load() { }
	virtual void Unload() { }
};

#endif /* __IHyNode2d_h__ */