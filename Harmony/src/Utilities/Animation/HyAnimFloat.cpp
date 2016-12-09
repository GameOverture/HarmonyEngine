/**************************************************************************
 *	HyAnimFloat.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Utilities/Animation/HyAnimFloat.h"

#include "Utilities/Animation/IHyTransform.h"

#include "Time/IHyTime.h"
#include "Utilities/HyMath.h"

HyAnimFloat::HyAnimFloat(float &valueReference, IHyTransformNode &ownerRef) :	m_fValueRef(valueReference),
																				m_OwnerRef(ownerRef),
																				m_fStart(0.0f),
																				m_fTarget(0.0f),
																				m_fDuration(0.0f),
																				m_fElapsedTime(0.0f),
																				m_fpEaseFunc(NULL),
																				m_fpBehaviorUpdate(NULL),
																				m_bAddedToOwnerUpdate(false)
{
}


HyAnimFloat::~HyAnimFloat(void)
{
}

void HyAnimFloat::Set(float fValue)
{
	if(m_fValueRef != fValue)
		m_OwnerRef.SetDirty();

	m_fValueRef = fValue;
	m_fpBehaviorUpdate = NULL;
}

void HyAnimFloat::Offset(float fValue)
{
	if(fValue != 0.0f)
		m_OwnerRef.SetDirty();

	m_fValueRef += fValue;
	m_fpBehaviorUpdate = NULL;
}

void HyAnimFloat::Tween(float fFrom, float fTo, float fSeconds, HyTweenUpdateFunc fpEase)
{
	if(m_fValueRef != fFrom)
		m_OwnerRef.SetDirty();

	m_fValueRef = m_fStart = fFrom;
	m_fTarget = fTo;
	m_fDuration = fSeconds;
	m_fpEaseFunc = fpEase;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::Tween;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::Tween(float fTo, float fSeconds, HyTweenUpdateFunc fpEase)
{
	m_fStart = m_fValueRef;
	m_fTarget = fTo;
	m_fDuration = fSeconds;
	m_fpEaseFunc = fpEase;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::Tween;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

void HyAnimFloat::TweenOffset(float fOffsetAmt, float fSeconds, HyTweenUpdateFunc fpEase)
{
	m_fStart = m_fValueRef;
	m_fTarget = m_fValueRef + fOffsetAmt;
	m_fDuration = fSeconds;
	m_fpEaseFunc = fpEase;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::Tween;

	m_OwnerRef.InsertActiveAnimFloat(this);
}

//void HyAnimFloat::Follow(float &fToFollow, float fOffsetAmt)
//{
//	m_pT
//}

HyAnimFloat &HyAnimFloat::operator=(const float &rhs)
{
	if(m_fValueRef != rhs)
		m_OwnerRef.SetDirty();

	m_fValueRef = rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator+=(const float &rhs)
{
	if(rhs != 0.0f)
		m_OwnerRef.SetDirty();

	m_fValueRef += rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const float &rhs)
{
	if(rhs != 0.0f)
		m_OwnerRef.SetDirty();

	m_fValueRef -= rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const float &rhs)
{
	float fProduct = m_fValueRef * rhs;
	if(fProduct != m_fValueRef)
		m_OwnerRef.SetDirty();

	m_fValueRef = fProduct;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const float &rhs)
{
	if(rhs != 1.0f)
		m_OwnerRef.SetDirty();

	m_fValueRef /= rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}


HyAnimFloat &HyAnimFloat::operator+=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_OwnerRef.SetDirty();

	m_fValueRef += rhs.Get();
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_OwnerRef.SetDirty();

	m_fValueRef -= rhs.Get();
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const HyAnimFloat &rhs)
{
	float fProduct = m_fValueRef * rhs.Get();
	if(m_fValueRef != fProduct)
		m_OwnerRef.SetDirty();

	m_fValueRef = fProduct;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 1.0f)
		m_OwnerRef.SetDirty();

	m_fValueRef /= rhs.Get();
	m_fpBehaviorUpdate = NULL;

	return *this;
}

// Returns true if updating is still continuing. False otherwise, to signal HyScene to remove this instance from the ActiveAnimFloat vector
bool HyAnimFloat::UpdateFloat()
{
	if(m_fpBehaviorUpdate == NULL)
		return false;

	return (this->*m_fpBehaviorUpdate)();
}

//////////////////////////////////////////////////////////////////////////
// Update Behaviors
//////////////////////////////////////////////////////////////////////////
bool HyAnimFloat::Tween()
{
	m_fElapsedTime = HyClamp(m_fElapsedTime + IHyTime::GetUpdateStepSeconds(), 0.0f, m_fDuration);
	
	float fFromVal = m_fValueRef;
	m_fValueRef = m_fStart + (m_fTarget - m_fStart) * m_fpEaseFunc(m_fElapsedTime / m_fDuration);
	m_OwnerRef.SetDirty();

	return (m_fElapsedTime != m_fDuration);
}
