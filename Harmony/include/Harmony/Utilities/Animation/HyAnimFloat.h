/**************************************************************************
 *	HyAnimFloat.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAnimFloat_h__
#define __HyAnimFloat_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/Animation/HyTweenFuncs.h"

#include <functional>

class IHyTransformNode;
typedef std::function<void(IHyTransformNode *)> HyTweenFinishedCallback;

class HyAnimFloat
{
	friend class IHyTransformNode;

	float &					m_fValueRef;
	IHyTransformNode &		m_OwnerRef;

	float					m_fStart;
	float					m_fTarget;
	float					m_fDuration;
	float					m_fElapsedTime;
	bool					m_bAddedToOwnerUpdate;

	HyTweenUpdateFunc		m_fpTweenFunc;
	HyTweenFinishedCallback	m_fpTweenFinishedFunc;

	bool (HyAnimFloat::*m_fpBehaviorUpdate)();

public:
	HyAnimFloat(float &valueReference, IHyTransformNode &ownerRef);
	~HyAnimFloat(void);

	inline float Get() const			{ return m_fValueRef; }
	void Set(float fValue);
	void Offset(float fValue);

	// Procedural transformation functions
	bool IsTweening()					{ return m_bAddedToOwnerUpdate; }
	void Tween(float fFrom, float fTo, float fSeconds, HyTweenUpdateFunc fpTweenFunc = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback);
	void Tween(float fTo, float fSeconds, HyTweenUpdateFunc fpTweenFunc = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback);
	void TweenOffset(float fOffsetAmt, float fSeconds, HyTweenUpdateFunc fpTweenFunc = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback);
	//void Follow(float &fToFollow, float fOffsetAmt, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback);

	void ClearCallback();

	HyAnimFloat &operator=(const float &rhs);
	HyAnimFloat &operator+=(const float &rhs);
	HyAnimFloat &operator-=(const float &rhs);
	HyAnimFloat &operator*=(const float &rhs);
	HyAnimFloat &operator/=(const float &rhs);

	HyAnimFloat &operator+=(const HyAnimFloat &rhs);
	HyAnimFloat &operator-=(const HyAnimFloat &rhs);
	HyAnimFloat &operator*=(const HyAnimFloat &rhs);
	HyAnimFloat &operator/=(const HyAnimFloat &rhs);

private:
	// Returns false if updating is still continuing. True when finished, which signals to IHyTransformNode to remove this instance from the ActiveAnimFloat vector
	bool UpdateFloat();

	//////////////////////////////////////////////////////////////////////////
	// Update Behaviors
	//////////////////////////////////////////////////////////////////////////
	bool Tween();
	bool Follow();
};

#endif /* __HyAnimFloat_h__ */
