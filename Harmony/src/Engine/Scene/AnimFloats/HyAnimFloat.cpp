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
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = fValue;
	StopAnim();
}

void HyAnimFloat::Set(const HyAnimFloat &valueRef)
{
	if(m_fValueRef != valueRef.Get())
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = valueRef.Get();
	StopAnim();
}

void HyAnimFloat::Offset(float fValue)
{
	if(fValue != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef += fValue;
	StopAnim();
}

bool HyAnimFloat::IsAnimating()
{
	return m_bAddedToOwnerUpdate;
}

void HyAnimFloat::Tween(float fTo, float fSeconds, HyTweenFunc fpTweenFunc /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= NullFinishedCallback*/)
{
	if(fSeconds <= 0.0f)
	{
		Set(fTo);
		return;
	}

	m_fStart = m_fValueRef;
	m_fTarget = fTo;
	m_fDuration = fSeconds;
	m_fpAnimFunc = fpTweenFunc;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::_Tween;
	m_fpAnimFinishedFunc = fpFinishedCallback;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::TweenOffset(float fOffsetAmt, float fSeconds, HyTweenFunc fpTweenFunc /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= NullFinishedCallback*/)
{
	if(fSeconds <= 0.0f)
	{
		Set(m_fValueRef + fOffsetAmt);
		return;
	}

	m_fStart = m_fValueRef;
	m_fTarget = m_fValueRef + fOffsetAmt;
	m_fDuration = fSeconds;
	m_fpAnimFunc = fpTweenFunc;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::_Tween;
	m_fpAnimFinishedFunc = fpFinishedCallback;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::Proc(float fSeconds, std::function<float(float)> fpProcFunc, HyAnimFinishedCallback fpFinishedCallback /*= NullFinishedCallback*/)
{
	m_fDuration = fSeconds;
	m_fpAnimFunc = fpProcFunc;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::_Proc;
	m_fpAnimFinishedFunc = fpFinishedCallback;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::StopAnim()
{
	m_fpBehaviorUpdate = nullptr;
	m_fpAnimFinishedFunc = NullFinishedCallback;
}

float HyAnimFloat::GetTweenDestination() const
{
	return m_fTarget;
}

float HyAnimFloat::GetTweenRemainingDuration() const
{
	return m_fDuration - m_fElapsedTime;
}

HyAnimFloat &HyAnimFloat::operator=(const float &rhs)
{
	if(m_fValueRef != rhs)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = rhs;
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator+=(const float &rhs)
{
	if(rhs != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef += rhs;
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const float &rhs)
{
	if(rhs != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef -= rhs;
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const float &rhs)
{
	float fProduct = m_fValueRef * rhs;
	if(fProduct != m_fValueRef)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = fProduct;
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const float &rhs)
{
	HyAssert(rhs != 0.0f, "HyAnimFloat::operator/= was passed a float that == 0.0f");
	if(rhs != 1.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef /= rhs;
	StopAnim();

	return *this;
}


HyAnimFloat &HyAnimFloat::operator+=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef += rhs.Get();
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef -= rhs.Get();
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const HyAnimFloat &rhs)
{
	float fProduct = m_fValueRef * rhs.Get();
	if(m_fValueRef != fProduct)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = fProduct;
	StopAnim();

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const HyAnimFloat &rhs)
{
	HyAssert(rhs.Get() != 0.0f, "HyAnimFloat::operator/= was passed a HyAnimFloat that == 0.0f");

	if(rhs.Get() != 1.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef /= rhs.Get();
	StopAnim();

	return *this;
}

// Returns false if updating is still continuing. True when finished, which signals to IHyNode to remove this instance from the ActiveAnimFloat vector
bool HyAnimFloat::UpdateFloat()
{
	if(m_fpBehaviorUpdate == nullptr)
		return true;

	if((this->*m_fpBehaviorUpdate)())
	{
		// Store the callback in a temp func pointer and clear 'm_fpBehaviorUpdate' with StopAnim().
		// When invoking the temp callback, if it happens to set 'm_fpBehaviorUpdate' again, it will stay 
		// assigned and not be removed from m_OwnerRef's update
		HyAnimFinishedCallback tmpTweenFinishedFunc = m_fpAnimFinishedFunc;

		StopAnim();
		tmpTweenFinishedFunc(&m_OwnerRef);

		return (m_fpBehaviorUpdate == nullptr);
	}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
// Update Behaviors
//////////////////////////////////////////////////////////////////////////
bool HyAnimFloat::_Tween()
{
	m_fElapsedTime = HyClamp(m_fElapsedTime + Hy_UpdateStep(), 0.0f, m_fDuration);
	
	m_fValueRef = m_fStart + (m_fTarget - m_fStart) * m_fpAnimFunc(m_fElapsedTime / m_fDuration);
	m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	return m_fElapsedTime == m_fDuration;
}

bool HyAnimFloat::_Proc()
{
	m_fElapsedTime = HyClamp(m_fElapsedTime + Hy_UpdateStep(), 0.0f, m_fDuration);

	m_fValueRef = m_fpAnimFunc(m_fElapsedTime / m_fDuration);
	m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	return m_fElapsedTime == m_fDuration;
}
