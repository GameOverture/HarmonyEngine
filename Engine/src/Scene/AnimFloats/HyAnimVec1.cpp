/**************************************************************************
*	HyAnimVec1.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimVec1.h"
#include "Utilities/HyMath.h"

HyAnimVec1::HyAnimVec1(IHyNode &ownerRef, uint32 uiDirtyFlags)
{
	m_pAnimFloat = HY_NEW HyAnimFloat(m_vValue.s, ownerRef, uiDirtyFlags);
}

HyAnimVec1::~HyAnimVec1()
{
	delete m_pAnimFloat;
}

float HyAnimVec1::Get() const
{
	return m_vValue.s;
}

void HyAnimVec1::Set(float fNewValue)
{
	*m_pAnimFloat = fNewValue;
}

void HyAnimVec1::Set(const HyAnimVec1 &rhs)
{
	*m_pAnimFloat = rhs.Get();
}

void HyAnimVec1::Offset(float fOffset)
{
	*m_pAnimFloat += fOffset;
}

HyAnimFloat &HyAnimVec1::GetAnimFloat()
{
	return *m_pAnimFloat;
}

float HyAnimVec1::Extrapolate(float fExtrapolatePercent) const
{
	return m_pAnimFloat->Extrapolate(fExtrapolatePercent);
}

void HyAnimVec1::Tween(float fTo, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, std::function<void(IHyNode *)> fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_pAnimFloat->Tween(fTo, fSeconds, fpTween, fDeferStart, fpFinishedCallback);
}

void HyAnimVec1::TweenOffset(float fOffset, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, std::function<void(IHyNode *)> fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_pAnimFloat->TweenOffset(fOffset, fSeconds, fpTween, fDeferStart, fpFinishedCallback);
}

void HyAnimVec1::Displace(float fMagnitude)
{
	m_pAnimFloat->Displace(fMagnitude);
}

bool HyAnimVec1::IsAnimating()
{
	return m_pAnimFloat->IsAnimating();
}

void HyAnimVec1::StopAnim()
{
	m_pAnimFloat->StopAnim();
}

float HyAnimVec1::GetAnimDestination() const
{
	return m_pAnimFloat->GetAnimDestination();
}

float HyAnimVec1::GetAnimRemainingDuration() const
{
	return m_pAnimFloat->GetAnimRemainingDuration();
}

HyAnimVec1 &HyAnimVec1::operator+=(float rhs)
{
	*m_pAnimFloat += rhs;
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator-=(float rhs)
{
	*m_pAnimFloat -= rhs;	
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator*=(float rhs)
{ 
	*m_pAnimFloat *= rhs;
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator/=(float rhs)
{
	*m_pAnimFloat /= rhs;
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator=(const HyAnimVec1 &rhs)
{
	*m_pAnimFloat = rhs.Get();
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator+=(const HyAnimVec1 &rhs)
{
	*m_pAnimFloat += rhs.Get();
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator-=(const HyAnimVec1 &rhs)
{
	*m_pAnimFloat -= rhs.Get();
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator*=(const HyAnimVec1 &rhs)
{
	*m_pAnimFloat *= rhs.Get();
	return *this;
}

HyAnimVec1 &HyAnimVec1::operator/=(const HyAnimVec1 &rhs)
{
	*m_pAnimFloat /= rhs.Get();
	return *this;
}
