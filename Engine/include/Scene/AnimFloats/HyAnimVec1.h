/**************************************************************************
 *	HyAnimVec1.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAnimVec1_h__
#define HyAnimVec1_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimFloat.h"

class HyAnimVec1
{
	glm::vec1					m_vValue;
	HyAnimFloat *				m_pAnimFloat;

public:
	HyAnimVec1(IHyNode &ownerRef, uint32 uiDirtyFlags);
	HyAnimVec1(const HyAnimVec1 &) = delete;
	HyAnimVec1(HyAnimVec1 &&) = delete;
	~HyAnimVec1();

	float Get() const;
	void Set(float fNewValue);
	void Set(const HyAnimVec1 &rhs);
	void Offset(float fOffset);

	HyAnimFloat &GetAnimFloat();
	float Extrapolate(float fExtrapolatePercent) const;
	
	void Tween(float fTo, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void TweenOffset(float fOffset, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, std::function<void(IHyNode *)> fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	void Displace(float fMagnitude);
	
	bool IsAnimating();
	void StopAnim();

	float GetAnimDestination() const;
	float GetAnimRemainingDuration() const;

	HyAnimVec1 &operator+=(float rhs);
	HyAnimVec1 &operator-=(float rhs);
	HyAnimVec1 &operator*=(float rhs);
	HyAnimVec1 &operator/=(float rhs);

	HyAnimVec1 &operator=(const HyAnimVec1 &rhs);
	HyAnimVec1 &operator+=(const HyAnimVec1 &rhs);
	HyAnimVec1 &operator-=(const HyAnimVec1 &rhs);
	HyAnimVec1 &operator*=(const HyAnimVec1 &rhs);
	HyAnimVec1 &operator/=(const HyAnimVec1 &rhs);
};

#endif /* HyAnimVec1_h__ */
