/**************************************************************************
 *	HyTweenFloat.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTweenFloat_h__
#define HyTweenFloat_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Tweens/HyTweenFuncs.h"

#include <functional>

class IHyNode;

typedef std::function<void(IHyNode *)> HyTweenFinishedCallback;

class HyTweenFloat
{
	friend class IHyNode;

	float &							m_fValueRef;
	IHyNode &						m_OwnerRef;
	const uint32					m_uiDIRTY_FLAGS;

	float							m_fStart;
	float							m_fTarget;
	float							m_fDuration;
	float							m_fElapsedTime;
	bool							m_bAddedToOwnerUpdate;

	HyTweenUpdateFunc				m_fpTweenFunc;
	HyTweenFinishedCallback			m_fpTweenFinishedFunc;

	bool (HyTweenFloat::*m_fpBehaviorUpdate)();

public:
	HyTweenFloat(float &valueReference, IHyNode &ownerRef, uint32 uiDirtyFlags);
	~HyTweenFloat(void);

	float Get() const;
	void Set(float fValue);
	void Set(const HyTweenFloat &valueRef);
	void Offset(float fValue);

	// Procedural transformation functions
	bool IsTweening();
	void Tween(float fTo, float fSeconds, HyTweenUpdateFunc fpTweenFunc = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);
	void TweenOffset(float fOffsetAmt, float fSeconds, HyTweenUpdateFunc fpTweenFunc = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);
	//void Follow(float &fToFollow, float fOffsetAmt, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);

	void StopTween();

	HyTweenFloat &operator=(const float &rhs);
	HyTweenFloat &operator+=(const float &rhs);
	HyTweenFloat &operator-=(const float &rhs);
	HyTweenFloat &operator*=(const float &rhs);
	HyTweenFloat &operator/=(const float &rhs);

	HyTweenFloat &operator+=(const HyTweenFloat &rhs);
	HyTweenFloat &operator-=(const HyTweenFloat &rhs);
	HyTweenFloat &operator*=(const HyTweenFloat &rhs);
	HyTweenFloat &operator/=(const HyTweenFloat &rhs);

private:
	// Returns false if updating is still continuing. True when finished, which signals to IHyNode to remove this instance from the ActiveAnimFloat vector
	bool UpdateFloat();

	//////////////////////////////////////////////////////////////////////////
	// Update Behaviors
	//////////////////////////////////////////////////////////////////////////
	bool Tween();
	bool Follow();
};

#endif /* HyTweenFloat_h__ */
