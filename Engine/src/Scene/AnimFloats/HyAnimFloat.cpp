/**************************************************************************
 *	HyAnimFloat.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimFloat.h"
#include "Scene/Nodes/IHyNode.h"
#include "HyEngine.h"
#include "Utilities/HyMath.h"

HyAnimFloat::HyAnimFloat(float &valueReference, IHyNode &ownerRef, uint32 uiDirtyFlags) :
	m_fValueRef(valueReference),
	m_OwnerRef(ownerRef),
	m_uiDIRTY_FLAGS(uiDirtyFlags),
	m_fStart(0.0f),
	m_fTarget(0.0f),
	m_fDuration(0.0f),
	m_fElapsedTime(0.0f),
	m_fpAnimFunc(nullptr),
	m_fpBehaviorUpdate(nullptr),
	m_fpAnimFinishedFunc(NullFinishedCallback),
	m_bAddedToOwnerUpdate(false)
{
	HyAssert(0 == (m_uiDIRTY_FLAGS & IHyNode::DIRTY_FromUpdater), "HyAnimFloat should not be passed 'DIRTY_FromUpdater'");
}

HyAnimFloat::~HyAnimFloat(void)
{
}

float HyAnimFloat::Get() const
{
	return m_fValueRef;
}

void HyAnimFloat::Set(float fValue)
{
	if(m_fValueRef != fValue)
	{
		m_fValueRef = fValue;
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();
}

void HyAnimFloat::Set(const HyAnimFloat &valueRef)
{
	if(m_fValueRef != valueRef.Get())
	{
		m_fValueRef = valueRef.Get();
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();
}

void HyAnimFloat::Offset(float fValue)
{
	if(fValue != 0.0f)
	{
		m_fValueRef += fValue;
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();
}

bool HyAnimFloat::IsAnimating() const
{
	return m_bAddedToOwnerUpdate;
}

void HyAnimFloat::Tween(float fTo, float fSeconds, HyTweenFunc fpTweenFunc /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, HyAnimFinishedCallback fpFinishedCallback /*= NullFinishedCallback*/)
{
	if(fSeconds <= 0.0f)
	{
		Set(fTo);
		fpFinishedCallback(&m_OwnerRef);
		return;
	}

	m_fStart = m_fValueRef;
	m_fTarget = fTo;
	m_fDuration = fSeconds;
	m_fpAnimFunc = fpTweenFunc;
	m_fElapsedTime = fabs(fDeferStart) * -1.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::_Tween;
	m_fpAnimFinishedFunc = fpFinishedCallback;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::TweenOffset(float fOffsetAmt, float fSeconds, HyTweenFunc fpTweenFunc /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, HyAnimFinishedCallback fpFinishedCallback /*= NullFinishedCallback*/)
{
	if(fSeconds <= 0.0f)
	{
		Set(m_fValueRef + fOffsetAmt);
		fpFinishedCallback(&m_OwnerRef);
		return;
	}

	m_fStart = m_fValueRef;
	m_fTarget = m_fValueRef + fOffsetAmt;
	m_fDuration = fSeconds;
	m_fpAnimFunc = fpTweenFunc;
	m_fElapsedTime = fabs(fDeferStart) * -1.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::_Tween;
	m_fpAnimFinishedFunc = fpFinishedCallback;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::Proc(float fSeconds, std::function<float(float)> fpProcFunc, float fDeferStart /*= 0.0f*/, HyAnimFinishedCallback fpFinishedCallback /*= NullFinishedCallback*/)
{
	// Even if duration is instant, we still need to invoke the proc func once. Do so safely by ensuring m_fDuration isn't 0.0
	if(fSeconds <= 0.0f)
	{
		// This will simulate the final update of the proc func
		m_fDuration = 1.0f;
		m_fElapsedTime = 1.0f;
	}
	else
	{
		// Standard initialization
		m_fDuration = fSeconds;
		m_fElapsedTime = fabs(fDeferStart) * -1.0f;
	}
	m_fpAnimFunc = fpProcFunc;
	m_fpBehaviorUpdate = &HyAnimFloat::_Proc;
	m_fpAnimFinishedFunc = fpFinishedCallback;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::Updater(std::function<float(float)> fpUpdaterFunc)
{
	m_fDuration = 0.0f;
	m_fpAnimFunc = fpUpdaterFunc;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::_Updater;
	m_fpAnimFinishedFunc = nullptr;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::Displace(float fMagnitude)
{
	if(fMagnitude == 0.0f)
		StopAnim();
	else
	{
		m_fStart = fMagnitude;
		m_fTarget = m_fValueRef + m_fStart;
		m_fDuration = 0.0f;
		m_fpAnimFunc = nullptr;
		m_fElapsedTime = 0.0f;
		m_fpBehaviorUpdate = &HyAnimFloat::_Displace;
		m_fpAnimFinishedFunc = nullptr;

		m_OwnerRef.InsertActiveAnimFloat(this);
	}
}

void HyAnimFloat::StopAnim()
{
	m_fpBehaviorUpdate = nullptr;
	m_fpAnimFinishedFunc = NullFinishedCallback;
}

float HyAnimFloat::GetAnimDestination() const
{
	return IsAnimating() ? m_fTarget : m_fValueRef;
}

float HyAnimFloat::GetAnimRemainingDuration() const
{
	return m_fDuration - m_fElapsedTime;
}

float HyAnimFloat::Extrapolate(float fExtrapolatePercent) const
{
	if(m_fpBehaviorUpdate == nullptr)
		return Get();
	
	uint32 uiDirtyFlags = 0;
	float fElapsedTime = HyMath::Max(m_fElapsedTime, 0.0f);
	float fExtrapolatedValue = m_fValueRef;
	(this->*m_fpBehaviorUpdate)(HyEngine::DeltaTime() * fExtrapolatePercent, fElapsedTime, fExtrapolatedValue, uiDirtyFlags);

	return fExtrapolatedValue;
}

HyAnimFloat &HyAnimFloat::operator=(const float &rhs)
{
	if(m_fValueRef != rhs)
	{
		m_fValueRef = rhs;
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator+=(const float &rhs)
{
	if(rhs != 0.0f)
	{
		m_fValueRef += rhs;
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const float &rhs)
{
	if(rhs != 0.0f)
	{
		m_fValueRef -= rhs;
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const float &rhs)
{
	if(rhs != 1.0f)
	{
		m_fValueRef *= rhs;
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}
	
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const float &rhs)
{
	HyAssert(rhs != 0.0f, "HyAnimFloat::operator/= was passed a float that == 0.0f");
	if(rhs != 1.0f)
	{
		m_fValueRef /= rhs;
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator=(const HyAnimFloat &rhs)
{
	if(m_fValueRef != rhs.Get())
	{
		m_fValueRef = rhs.Get();
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator+=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
	{
		m_fValueRef += rhs.Get();
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
	{
		m_fValueRef -= rhs.Get();
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 1.0f)
	{
		m_fValueRef *= rhs.Get();
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const HyAnimFloat &rhs)
{
	HyAssert(rhs.Get() != 0.0f, "HyAnimFloat::operator/= was passed a HyAnimFloat that == 0.0f");
	if(rhs.Get() != 1.0f)
	{
		m_fValueRef /= rhs.Get();
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);
	}

	StopAnim();

	return *this;
}

// Returns false if updating is still continuing. True when finished, which signals to IHyNode to remove this instance from the ActiveAnimFloat vector
bool HyAnimFloat::UpdateFloat()
{
	if(m_fpBehaviorUpdate == nullptr)
		return true;

	if(m_fElapsedTime < 0.0f) // Handle deferred start
	{
		m_fElapsedTime = HyMath::Min(m_fElapsedTime + HyEngine::DeltaTime(), 0.0f);
		return false;
	}

	uint32 uiDirtyFlags = 0;
	if((this->*m_fpBehaviorUpdate)(HyEngine::DeltaTime(), m_fElapsedTime, m_fValueRef, uiDirtyFlags))
	{
		m_OwnerRef.SetDirty(uiDirtyFlags);

		// Store the callback in a temp func pointer and clear 'm_fpBehaviorUpdate' with StopAnim().
		// When invoking the temp callback, if it happens to set 'm_fpBehaviorUpdate' again, it will stay 
		// assigned and not be removed from m_OwnerRef's update
		HyAnimFinishedCallback tmpTweenFinishedFunc = m_fpAnimFinishedFunc;

		StopAnim();
		tmpTweenFinishedFunc(&m_OwnerRef);

		return (m_fpBehaviorUpdate == nullptr);
	}
	else
	{
		m_OwnerRef.SetDirty(uiDirtyFlags);
		return false;
	}
}

//////////////////////////////////////////////////////////////////////////
// Behavior Updates
//////////////////////////////////////////////////////////////////////////
bool HyAnimFloat::_Tween(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const
{
	fElapsedTimeOut = HyMath::Clamp(fElapsedTimeOut + fDeltaTime, 0.0f, m_fDuration);
	fValueOut = m_fStart + (m_fTarget - m_fStart) * m_fpAnimFunc(fElapsedTimeOut / m_fDuration);
	uiDirtyFlagsOut = m_uiDIRTY_FLAGS;

	return fElapsedTimeOut == m_fDuration;
}

bool HyAnimFloat::_Proc(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const
{
	fElapsedTimeOut = HyMath::Clamp(fElapsedTimeOut + fDeltaTime, 0.0f, m_fDuration);
	fValueOut = m_fpAnimFunc(fElapsedTimeOut / m_fDuration);
	uiDirtyFlagsOut = m_uiDIRTY_FLAGS;

	return fElapsedTimeOut == m_fDuration;
}

bool HyAnimFloat::_Updater(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const
{
	fElapsedTimeOut += fDeltaTime;
	fValueOut = m_fpAnimFunc(fElapsedTimeOut);
	uiDirtyFlagsOut = m_uiDIRTY_FLAGS | IHyNode::DIRTY_FromUpdater;

	return false;
}

bool HyAnimFloat::_Displace(float fDeltaTime, float &fElapsedTimeOut, float &fValueOut, uint32 &uiDirtyFlagsOut) const
{
	fElapsedTimeOut += fDeltaTime;
	fValueOut *= (m_fStart * fDeltaTime);
	uiDirtyFlagsOut = m_uiDIRTY_FLAGS;

	return false;
}
