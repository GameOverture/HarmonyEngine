
/**************************************************************************
 *	HyAnimFloat.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAnimFloat_h__
#define HyAnimFloat_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyTweenFuncs.h"

#include <functional>

class IHyNode;

using HyAnimFinishedCallback = std::function<void(IHyNode *)>;

class HyAnimFloat
{
	friend class IHyNode;

	float &							m_fValueRef;
	IHyNode &						m_OwnerRef;
	const uint32					m_uiDIRTY_FLAGS;

	float							m_fStart; // Or magnitude when Displace()
	float							m_fTarget;
	float							m_fDuration;
	float							m_fElapsedTime;
	bool							m_bAddedToOwnerUpdate;

	HyTweenFunc						m_fpAnimFunc;
	HyAnimFinishedCallback			m_fpAnimFinishedFunc;

	bool (HyAnimFloat::*m_fpBehaviorUpdate)(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const;

public:
	HyAnimFloat(float &valueReference, IHyNode &ownerRef, uint32 uiDirtyFlags);
	HyAnimFloat(const HyAnimFloat &) = delete;
	HyAnimFloat(HyAnimFloat &&) = delete;
	~HyAnimFloat(void);

	float Get() const;
	void Set(float fValue);
	void Set(const HyAnimFloat &valueRef);
	void Offset(float fValue);

	// Procedural transformation functions
	bool IsAnimating() const;
	void Tween(float fTo, float fSeconds, HyTweenFunc fpTweenFunc = HyTween::Linear, float fDeferStart = 0.0f, HyAnimFinishedCallback fpFinishedCallback = NullFinishedCallback);
	void TweenOffset(float fOffsetAmt, float fSeconds, HyTweenFunc fpTweenFunc = HyTween::Linear, float fDeferStart = 0.0f, HyAnimFinishedCallback fpFinishedCallback = NullFinishedCallback);

	void Proc(float fSeconds, std::function<float(float)> fpProcFunc, float fDeferStart = 0.0f, HyAnimFinishedCallback fpFinishedCallback = NullFinishedCallback);

	void Updater(std::function<float(float)> fpUpdaterFunc);

	void Displace(float fMagnitude);

	void StopAnim();

	float GetAnimDestination() const;
	float GetAnimRemainingDuration() const;
	void ChangeElapsedTime(float fNewElapsedTime);

	float Extrapolate(float fExtrapolatePercent) const;

	HyAnimFloat &operator=(const float &rhs);
	HyAnimFloat &operator+=(const float &rhs);
	HyAnimFloat &operator-=(const float &rhs);
	HyAnimFloat &operator*=(const float &rhs);
	HyAnimFloat &operator/=(const float &rhs);

	HyAnimFloat &operator=(const HyAnimFloat &rhs);
	HyAnimFloat &operator+=(const HyAnimFloat &rhs);
	HyAnimFloat &operator-=(const HyAnimFloat &rhs);
	HyAnimFloat &operator*=(const HyAnimFloat &rhs);
	HyAnimFloat &operator/=(const HyAnimFloat &rhs);

private:
	// Returns false if updating is still continuing. True when finished, which signals to IHyNode to remove this instance from the ActiveAnimFloat vector
	bool UpdateFloat();

	//////////////////////////////////////////////////////////////////////////
	// Update Behaviors
	//////////////////////////////////////////////////////////////////////////
	bool _Tween(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const;
	bool _Proc(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const;
	bool _Updater(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const;
	bool _Displace(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const;

public:
	// Default (do-nothing) callback when tween finishes
	static void NullFinishedCallback(IHyNode *)
	{ }
};

#endif /* HyAnimFloat_h__ */
