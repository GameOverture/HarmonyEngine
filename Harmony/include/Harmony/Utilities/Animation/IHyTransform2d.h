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

#include "Utilities/Animation/HyAnimVec2.h"
#include "Utilities/Animation/IHyTransform.h"

class IHyTransform2d : public IHyTransform<HyAnimVec2>
{
protected:
	glm::mat4						m_mtxCached;

	float							m_fRotation;

public:
	IHyTransform2d(HyType eInstType);
	virtual ~IHyTransform2d();
	
	HyAnimFloat						rot;

	void QueuePos(float fX, float fY, float fTweenDuration, HyTweenUpdateFunc fpEase, float fDefer = 0.0f);
	void QueueRot(float fX, float fY, float fTweenDuration, HyTweenUpdateFunc fpEase, float fDefer = 0.0f);
	void QueueScale(float fX, float fY, float fTweenDuration, HyTweenUpdateFunc fpEase, float fDefer = 0.0f);
	void QueueCallback(void(*fpCallback)(IHyTransform2d *, void *), void *pParam = NULL, float fDefer = 0.0f);

	virtual void GetLocalTransform(glm::mat4 &outMtx) const;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const;

	void GetWorldTransform(glm::mat4 &outMtx);

private:
	virtual void OnUpdate() = 0;
};

#endif /* __IHyTransform2d_h__ */
