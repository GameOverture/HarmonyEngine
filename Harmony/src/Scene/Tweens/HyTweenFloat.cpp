/**************************************************************************
 *	HyTweenFloat.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Tweens/HyTweenFloat.h"
#include "Scene/Nodes/IHyNode.h"
#include "HyEngine.h"
#include "Utilities/HyMath.h"

HyTweenFloat::HyTweenFloat(float &valueReference, IHyNode &ownerRef, uint32 uiDirtyFlags) :	m_fValueRef(valueReference),
																							m_OwnerRef(ownerRef),
																							m_uiDIRTY_FLAGS(uiDirtyFlags),
																							m_fStart(0.0f),
																							m_fTarget(0.0f),
																							m_fDuration(0.0f),
																							m_fElapsedTime(0.0f),
																							m_fpTweenFunc(nullptr),
																							m_fpBehaviorUpdate(nullptr),
																							m_fpTweenFinishedFunc(HyTween::NullTweenCallback),
																							m_bAddedToOwnerUpdate(false)
{
}


HyTweenFloat::~HyTweenFloat(void)
{
}

float HyTweenFloat::Get() const
{
	return m_fValueRef;
}

void HyTweenFloat::Set(float fValue)
{
	if(m_fValueRef != fValue)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = fValue;
	StopTween();
}

void HyTweenFloat::Offset(float fValue)
{
	if(fValue != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef += fValue;
	StopTween();
}

bool HyTweenFloat::IsTweening()
{
	return m_bAddedToOwnerUpdate;
}

void HyTweenFloat::Tween(float fTo, float fSeconds, HyTweenUpdateFunc fpTweenFunc /*= HyTween::Linear*/, HyTweenFinishedCallback tweenFinishedCallback /*= HyTween::NullTweenCallback*/)
{
	if(fSeconds <= 0.0f)
	{
		Set(fTo);
		return;
	}

	m_fStart = m_fValueRef;
	m_fTarget = fTo;
	m_fDuration = fSeconds;
	m_fpTweenFunc = fpTweenFunc;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyTweenFloat::Tween;
	m_fpTweenFinishedFunc = tweenFinishedCallback;

	m_OwnerRef.InsertActiveTweenFloat(this);
}

void HyTweenFloat::TweenOffset(float fOffsetAmt, float fSeconds, HyTweenUpdateFunc fpTweenFunc /*= HyTween::Linear*/, HyTweenFinishedCallback tweenFinishedCallback /*= HyTween::NullTweenCallback*/)
{
	if(fSeconds <= 0.0f)
	{
		Set(m_fValueRef + fOffsetAmt);
		return;
	}

	m_fStart = m_fValueRef;
	m_fTarget = m_fValueRef + fOffsetAmt;
	m_fDuration = fSeconds;
	m_fpTweenFunc = fpTweenFunc;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyTweenFloat::Tween;
	m_fpTweenFinishedFunc = tweenFinishedCallback;

	m_OwnerRef.InsertActiveTweenFloat(this);
}

void HyTweenFloat::StopTween()
{
	m_fpBehaviorUpdate = nullptr;
	m_fpTweenFinishedFunc = HyTween::NullTweenCallback;
}

HyTweenFloat &HyTweenFloat::operator=(const float &rhs)
{
	if(m_fValueRef != rhs)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = rhs;
	StopTween();

	return *this;
}

HyTweenFloat &HyTweenFloat::operator+=(const float &rhs)
{
	if(rhs != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef += rhs;
	StopTween();

	return *this;
}

HyTweenFloat &HyTweenFloat::operator-=(const float &rhs)
{
	if(rhs != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef -= rhs;
	StopTween();

	return *this;
}

HyTweenFloat &HyTweenFloat::operator*=(const float &rhs)
{
	float fProduct = m_fValueRef * rhs;
	if(fProduct != m_fValueRef)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = fProduct;
	StopTween();

	return *this;
}

HyTweenFloat &HyTweenFloat::operator/=(const float &rhs)
{
	HyAssert(rhs != 0.0f, "HyTweenFloat::operator/= was passed a float that == 0.0f");
	if(rhs != 1.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef /= rhs;
	StopTween();

	return *this;
}


HyTweenFloat &HyTweenFloat::operator+=(const HyTweenFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef += rhs.Get();
	StopTween();

	return *this;
}

HyTweenFloat &HyTweenFloat::operator-=(const HyTweenFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef -= rhs.Get();
	StopTween();

	return *this;
}

HyTweenFloat &HyTweenFloat::operator*=(const HyTweenFloat &rhs)
{
	float fProduct = m_fValueRef * rhs.Get();
	if(m_fValueRef != fProduct)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef = fProduct;
	StopTween();

	return *this;
}

HyTweenFloat &HyTweenFloat::operator/=(const HyTweenFloat &rhs)
{
	HyAssert(rhs.Get() != 0.0f, "HyTweenFloat::operator/= was passed a HyTweenFloat that == 0.0f");

	if(rhs.Get() != 1.0f)
		m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	m_fValueRef /= rhs.Get();
	StopTween();

	return *this;
}

// Returns false if updating is still continuing. True when finished, which signals to IHyNode to remove this instance from the ActiveAnimFloat vector
bool HyTweenFloat::UpdateFloat()
{
	if(m_fpBehaviorUpdate == nullptr)
		return true;

	if((this->*m_fpBehaviorUpdate)())
	{
		// Store the callback in a temp func pointer and clear 'm_fpBehaviorUpdate' with StopTween().
		// When invoking the temp callback, if it happens to set 'm_fpBehaviorUpdate' again, it will stay 
		// assigned and not be removed from m_OwnerRef's update
		HyTweenFinishedCallback tmpTweenFinishedFunc = m_fpTweenFinishedFunc;

		StopTween();
		tmpTweenFinishedFunc(&m_OwnerRef);

		return (m_fpBehaviorUpdate == nullptr);
	}
	else
		return false;
}

//////////////////////////////////////////////////////////////////////////
// Update Behaviors
//////////////////////////////////////////////////////////////////////////
bool HyTweenFloat::Tween()
{
	m_fElapsedTime = HyClamp(m_fElapsedTime + Hy_UpdateStep(), 0.0f, m_fDuration);
	
	float fFromVal = m_fValueRef;
	m_fValueRef = m_fStart + (m_fTarget - m_fStart) * m_fpTweenFunc(m_fElapsedTime / m_fDuration);
	m_OwnerRef.SetDirty(m_uiDIRTY_FLAGS);

	return m_fElapsedTime == m_fDuration;
}
