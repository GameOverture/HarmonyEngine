/**************************************************************************
 *	HyTweenFloat.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyTweenFloat_h__
#define __HyTweenFloat_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenFuncs.h"
#include <functional>

class IHyNode2d;
typedef std::function<void(IHyNode2d *)> HyTweenFinishedCallback;

class HyTweenFloat
{
	friend class IHyNode2d;

	float &					m_fValueRef;
	IHyNode2d &				m_OwnerRef;

	float					m_fStart;
	float					m_fTarget;
	float					m_fDuration;
	float					m_fElapsedTime;
	bool					m_bAddedToOwnerUpdate;

	HyTweenUpdateFunc		m_fpTweenFunc;
	HyTweenFinishedCallback	m_fpTweenFinishedFunc;

	bool (HyTweenFloat::*m_fpBehaviorUpdate)();

public:
	HyTweenFloat(float &valueReference, IHyNode2d &ownerRef);
	~HyTweenFloat(void);

	float Get() const;
	void Set(float fValue);
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
	// Returns false if updating is still continuing. True when finished, which signals to IHyNode2d to remove this instance from the ActiveAnimFloat vector
	bool UpdateFloat();

	//////////////////////////////////////////////////////////////////////////
	// Update Behaviors
	//////////////////////////////////////////////////////////////////////////
	bool Tween();
	bool Follow();
};

#endif /* __HyTweenFloat_h__ */
