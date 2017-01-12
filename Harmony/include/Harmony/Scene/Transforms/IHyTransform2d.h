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

#include "Scene/Transforms/Tweens/HyTweenVec2.h"
#include "Scene/Transforms/IHyTransform.h"

class IHyTransform2d : public IHyTransform<HyTweenVec2>
{
protected:
	glm::mat4						m_mtxCached;
	HyCoordinateUnit				m_eCoordUnit;

	float							m_fRotation;

public:
	HyTweenFloat					rot;

	IHyTransform2d(HyType eInstType);
	virtual ~IHyTransform2d();

	HyCoordinateUnit GetCoordinateUnit();
	void SetCoordinateUnit(HyCoordinateUnit eCoordUnit, bool bDoConversion);

	virtual void GetLocalTransform(glm::mat4 &outMtx) const;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const;

	void GetWorldTransform(glm::mat4 &outMtx);
};

#endif /* __IHyTransform2d_h__ */
