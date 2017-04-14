/**************************************************************************
*	HyTransform2d.h
*
*	Harmony Engine
*	Copyright (c) 2014 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyTransform2d_h__
#define __HyTransform2d_h__

#include "Scene/Nodes/IHyNode.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenVec2.h"
#include "Scene/Physics/HyBoundingVolume2d.h"

class HyTransform2d : public IHyNode
{
protected:
	glm::mat4						m_mtxCached;
	HyCoordinateUnit				m_eCoordUnit;

	float							m_fRotation;

	HyBoundingVolume2d				m_BoundingVolume;

public:
	HyTweenVec2						pos;
	HyTweenFloat					rot;
	HyTweenVec2						rot_pivot;
	HyTweenVec2						scale;
	HyTweenVec2						scale_pivot;

	HyTransform2d(HyType eInstType, IHyNode *pParent = nullptr);
	virtual ~HyTransform2d();

	HyCoordinateUnit GetCoordinateUnit();
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	void GetLocalTransform(glm::mat4 &outMtx) const;
	void GetWorldTransform(glm::mat4 &outMtx);
};

#endif /* __HyTransform2d_h__ */
