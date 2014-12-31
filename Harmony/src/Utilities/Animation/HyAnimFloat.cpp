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

#include "Creator/HyCreator.h"

#include "Time/ITimeApi.h"

#include "Utilities/HyMath.h"

/*static*/ HyCreator *HyAnimFloat::sm_pCtor = NULL;

HyAnimFloat::HyAnimFloat() :	m_fValueRef(*(new float)),
								m_fStart(0.0f),
								m_fTarget(0.0f),
								m_fDuration(0.0f),
								m_fElapsedTime(0.0f),
								m_fpEaseFunc(NULL),
								m_fpBehaviorUpdate(NULL),
								m_fpOnDirty(HyAnimFloat::_NullOnChange),
								m_pOnChangeParam(NULL),
								m_kbSelfAllocated(true)
{
}

HyAnimFloat::HyAnimFloat(float &valueReference) :	m_fValueRef(valueReference), 
													m_fStart(0.0f),
													m_fTarget(0.0f),
													m_fDuration(0.0f),
													m_fElapsedTime(0.0f),
													m_fpEaseFunc(NULL),
													m_fpBehaviorUpdate(NULL),
													m_fpOnDirty(HyAnimFloat::_NullOnChange),
													m_pOnChangeParam(NULL),
													m_kbSelfAllocated(false)
{
}


HyAnimFloat::~HyAnimFloat(void)
{
	if(m_kbSelfAllocated)
		delete &m_fValueRef;	// This looks dangerous, but it should be fine.
}

void HyAnimFloat::Set(float fValue)
{
	if(m_fValueRef != fValue)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef = fValue;
	m_fpBehaviorUpdate = NULL;
}

void HyAnimFloat::Offset(float fValue)
{
	if(fValue != 0.0f)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef += fValue;
	m_fpBehaviorUpdate = NULL;
}

void HyAnimFloat::Animate(float fFrom, float fTo, float fSeconds, EaseUpdateFunc fpEase)
{
	HyAssert(sm_pCtor, "HyAnimFloat::Animate() cannot be used before IGame::Initialize() is invoked.");

	if(m_fValueRef != fFrom)
		m_fpOnDirty(m_pOnChangeParam);
	m_fValueRef = m_fStart = fFrom;
	m_fTarget = fTo;
	m_fDuration = fSeconds;
	m_fpEaseFunc = fpEase;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::Tween;

	sm_pCtor->InsertActiveAnimFloat(this);
}

void HyAnimFloat::Animate(float fTo, float fSeconds, EaseUpdateFunc fpEase)
{
	HyAssert(sm_pCtor, "HyAnimFloat::Animate() cannot be used before IGame::Initialize() is invoked.");

	m_fStart = m_fValueRef;
	m_fTarget = fTo;
	m_fDuration = fSeconds;
	m_fpEaseFunc = fpEase;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::Tween;

	sm_pCtor->InsertActiveAnimFloat(this);
}

void HyAnimFloat::AnimateOffset(float fOffsetAmt, float fSeconds, EaseUpdateFunc fpEase)
{
	HyAssert(sm_pCtor, "HyAnimFloat::AnimateOffset() cannot be used before IGame::Initialize() is invoked.");

	m_fStart = m_fValueRef;
	m_fTarget = m_fValueRef + fOffsetAmt;
	m_fDuration = fSeconds;
	m_fpEaseFunc = fpEase;
	m_fElapsedTime = 0.0f;
	m_fpBehaviorUpdate = &HyAnimFloat::Tween;

	sm_pCtor->InsertActiveAnimFloat(this);
}

void HyAnimFloat::SetOnDirtyCallback(void (*fpOnDirty)(void *), void *pParam /*= NULL*/)
{
	m_fpOnDirty = (fpOnDirty == NULL) ? _NullOnChange : fpOnDirty;
	m_pOnChangeParam = pParam;
}

//void HyAnimFloat::Follow(float &fToFollow, float fOffsetAmt)
//{
//	m_pT
//}

// Returns true if updating is still continuing. False otherwise, to signal HyCreator to remove this instance from the ActiveAnimFloat vector
bool HyAnimFloat::Update()
{
	if(m_fpBehaviorUpdate == NULL)
		return false;
	
	return (this->*m_fpBehaviorUpdate)();
}

HyAnimFloat &HyAnimFloat::operator=(const float &rhs)
{
	if(m_fValueRef != rhs)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef = rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator+=(const float &rhs)
{
	if(rhs != 0.0f)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef += rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const float &rhs)
{
	if(rhs != 0.0f)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef -= rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const float &rhs)
{
	float fProduct = m_fValueRef * rhs;
	if(fProduct != m_fValueRef)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef = fProduct;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const float &rhs)
{
	if(rhs != 1.0f)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef /= rhs;
	m_fpBehaviorUpdate = NULL;

	return *this;
}


HyAnimFloat &HyAnimFloat::operator+=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef += rhs.Get();
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator-=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 0.0f)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef -= rhs.Get();
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator*=(const HyAnimFloat &rhs)
{
	float fProduct = m_fValueRef * rhs.Get();
	if(m_fValueRef != fProduct)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef = fProduct;
	m_fpBehaviorUpdate = NULL;

	return *this;
}

HyAnimFloat &HyAnimFloat::operator/=(const HyAnimFloat &rhs)
{
	if(rhs.Get() != 1.0f)
		m_fpOnDirty(m_pOnChangeParam);

	m_fValueRef /= rhs.Get();
	m_fpBehaviorUpdate = NULL;

	return *this;
}


//////////////////////////////////////////////////////////////////////////
// Update Behaviors
//////////////////////////////////////////////////////////////////////////
bool HyAnimFloat::Tween()
{
	m_fElapsedTime = HyClamp(m_fElapsedTime + ITimeApi::GetUpdateStepSeconds(), 0.0f, m_fDuration);
	
	float fFromVal = m_fValueRef;
	m_fValueRef = m_fStart + (m_fTarget - m_fStart) * m_fpEaseFunc(m_fElapsedTime / m_fDuration);
	m_fpOnDirty(m_pOnChangeParam);

	return (m_fElapsedTime != m_fDuration);
}
