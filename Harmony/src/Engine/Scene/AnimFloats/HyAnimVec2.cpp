/**************************************************************************
*	HyAnimVec2.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimVec2.h"

HyAnimVec2::HyAnimVec2(IHyNode &ownerRef, uint32 uiDirtyFlags)
{
	m_AnimFloatList.push_back(HyAnimFloat(m_vValue[0], ownerRef, uiDirtyFlags));
	m_AnimFloatList.push_back(HyAnimFloat(m_vValue[1], ownerRef, uiDirtyFlags));
}

HyAnimVec2::~HyAnimVec2()
{ }

const glm::vec2 &HyAnimVec2::Get() const
{
	return m_vValue;
}

const glm::vec3 HyAnimVec2::Extrapolate() const
{
	return glm::vec3(m_vValue.x, m_vValue.y, 0.0f);
}

float HyAnimVec2::X() const
{
	return m_AnimFloatList[0].Get();
}

void HyAnimVec2::X(float fValue)
{
	m_AnimFloatList[0] = fValue;
	m_AnimFloatList[1].StopAnim();
}

void HyAnimVec2::X(int32 iValue)
{
	m_AnimFloatList[0] = static_cast<float>(iValue);
	m_AnimFloatList[1].StopAnim();
}

float HyAnimVec2::Y() const
{
	return m_AnimFloatList[1].Get();
}

void HyAnimVec2::Y(float fValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = fValue;
}

void HyAnimVec2::Y(int32 iValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = static_cast<float>(iValue);
}

void HyAnimVec2::Set(float fAll)
{
	m_AnimFloatList[0].Set(fAll);
	m_AnimFloatList[1].Set(fAll);
}

void HyAnimVec2::Set(int32 iX, int32 iY)
{
	m_AnimFloatList[0] = static_cast<float>(iX);
	m_AnimFloatList[1] = static_cast<float>(iY);
}

void HyAnimVec2::Set(float fX, float fY)
{
	m_AnimFloatList[0] = fX;
	m_AnimFloatList[1] = fY;
}

void HyAnimVec2::Set(const glm::vec2 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
}

void HyAnimVec2::Set(const glm::vec3 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
}

void HyAnimVec2::Set(const glm::ivec2 &srcVec)
{
	m_AnimFloatList[0].Set(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Set(static_cast<float>(srcVec[1]));
}

void HyAnimVec2::Set(const HyAnimVec2 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
}

void HyAnimVec2::Offset(float fX, float fY)
{
	m_AnimFloatList[0] += fX;
	m_AnimFloatList[1] += fY;
}

void HyAnimVec2::Offset(const glm::vec2 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
}

void HyAnimVec2::Offset(const glm::ivec2 &srcVec)
{
	m_AnimFloatList[0].Offset(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Offset(static_cast<float>(srcVec[1]));
}

void HyAnimVec2::Offset(const HyAnimVec2 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
}

void HyAnimVec2::Tween(int32 iX, int32 iY, float fSeconds, HyTweenFunc fpEase /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	Tween(static_cast<float>(iX), static_cast<float>(iY), fSeconds, fpEase, fpFinishedCallback);
}

void HyAnimVec2::Tween(float fX, float fY, float fSeconds, HyTweenFunc fpEase /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].Tween(fX, fSeconds, fpEase, fpFinishedCallback);
	m_AnimFloatList[1].Tween(fY, fSeconds, fpEase);
}

void HyAnimVec2::TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenFunc fpEase /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].TweenOffset(fOffsetX, fSeconds, fpEase, fpFinishedCallback);
	m_AnimFloatList[1].TweenOffset(fOffsetY, fSeconds, fpEase);
}

void HyAnimVec2::Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, float fSeconds, HyAnimFinishedCallback fpFinishedCallback /* = HyAnimFloat::NullTweenCallback */)
{
	// Quadratic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.x; }, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.y; });
}

void HyAnimVec2::Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, const glm::vec2 &pt4, float fSeconds, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	// Cubic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.x; }, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.y; });
}

bool HyAnimVec2::IsAnimating()
{
	return (m_AnimFloatList[0].IsAnimating() || m_AnimFloatList[1].IsAnimating());
}

void HyAnimVec2::StopAnim()
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1].StopAnim();
}

glm::vec2 HyAnimVec2::GetTweenDestination() const
{
	return glm::vec2(m_AnimFloatList[0].GetTweenDestination(), m_AnimFloatList[1].GetTweenDestination());
}

float HyAnimVec2::GetTweenRemainingDuration() const
{
	return m_AnimFloatList[0].GetTweenRemainingDuration();
}

HyAnimVec2 &HyAnimVec2::operator+=(float rhs)
{
	m_AnimFloatList[0] += rhs;
	m_AnimFloatList[1] += rhs;

	return *this;
}

HyAnimVec2 &HyAnimVec2::operator-=(float rhs)
{
	m_AnimFloatList[0] -= rhs;
	m_AnimFloatList[1] -= rhs;
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator*=(float rhs)
{ 
	m_AnimFloatList[0] *= rhs;
	m_AnimFloatList[1] *= rhs;
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator/=(float rhs)
{
	m_AnimFloatList[0] /= rhs;
	m_AnimFloatList[1] /= rhs;
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator+=(const HyAnimVec2 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator-=(const HyAnimVec2 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1];
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator*=(const HyAnimVec2 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1];
	
	return *this;
}
HyAnimVec2 &HyAnimVec2::operator/=(const HyAnimVec2 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1];
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator+=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator-=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1]; 
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator*=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1]; 
	
	return *this;
}

HyAnimVec2 &HyAnimVec2::operator/=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1]; 
	
	return *this;
}

float HyAnimVec2::operator[](int i) const
{
	return m_vValue[i];
}
