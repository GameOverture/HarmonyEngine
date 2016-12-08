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
	// Transformation hierarchy
	IHyTransform2d *				m_pParent;
	bool							m_bDirty;
	glm::mat4						m_mtxCached;
	vector<IHyTransform2d *>		m_ChildList;

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
	virtual void SetOnDirtyCallback(void(*fpOnDirty)(void *), void *pParam = NULL);

	void GetWorldTransform(glm::mat4 &outMtx);

	void AddChild(IHyTransform2d &childInst);
	void Detach();

	void SetDirty();
	static void OnDirty(void *);

	virtual void OnUpdate() = 0;
};

#endif /* __IHyTransform2d_h__ */
