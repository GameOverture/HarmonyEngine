/**************************************************************************
*	IHyTransform2d.h
*
*	Harmony Engine
*	Copyright (c) 2014 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyTransform2d_h__
#define __IHyTransform2d_h__

#include "Scene/Nodes/Transforms/Tweens/HyTweenVec2.h"
#include "Scene/Nodes/Transforms/IHyTransform.h"
#include "Scene/Physics/HyBoundingVolume2d.h"

class IHyTransform2d : public IHyTransform<HyTweenVec2>
{
protected:
	glm::mat4						m_mtxCached;
	HyCoordinateUnit				m_eCoordUnit;

	float							m_fRotation;

	HyBoundingVolume2d				m_BoundingVolume;

public:
	HyTweenFloat					rot;

	IHyTransform2d(HyType eInstType);
	virtual ~IHyTransform2d();

	HyBoundingVolume2d &GetBoundingVolume();

	HyCoordinateUnit GetCoordinateUnit();
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	virtual void GetLocalTransform(glm::mat4 &outMtx) const override;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const override;

	void GetWorldTransform(glm::mat4 &outMtx);
};

#endif /* __IHyTransform2d_h__ */
