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
	m_AnimFloatList = reinterpret_cast<HyAnimFloat *>(HY_NEW uint8[sizeof(HyAnimFloat) * 2]);
	HyAnimFloat *pAnimFloatWriteLocation = m_AnimFloatList;

	for(uint32 i = 0; i < 2; ++i, ++pAnimFloatWriteLocation)
		new (pAnimFloatWriteLocation) HyAnimFloat(m_vValue[i], ownerRef, uiDirtyFlags);
}

HyAnimVec2::~HyAnimVec2()
{
	for(uint32 i = 0; i < 2; ++i)
		m_AnimFloatList[i].~HyAnimFloat();

	uint8 *pAnimFloatBuffer = reinterpret_cast<uint8 *>(m_AnimFloatList);
	delete[] pAnimFloatBuffer;
}

const glm::vec2 &HyAnimVec2::Get() const
{
	return m_vValue;
}

HyAnimFloat &HyAnimVec2::GetAnimFloat(uint32 uiIndex)
{
	return m_AnimFloatList[uiIndex];
}

glm::vec3 HyAnimVec2::Extrapolate(float fExtrapolatePercent) const
{
	return glm::vec3(m_AnimFloatList[0].Extrapolate(fExtrapolatePercent), m_AnimFloatList[1].Extrapolate(fExtrapolatePercent), 0.0f);
}

float HyAnimVec2::X() const
{
	return m_AnimFloatList[0].Get();
}

float HyAnimVec2::X(float fNewValue)
{
	m_AnimFloatList[0] = fNewValue;
	m_AnimFloatList[1].StopAnim();
	return m_AnimFloatList[0].Get();
}

float HyAnimVec2::X(int32 iNewValue)
{
	m_AnimFloatList[0] = static_cast<float>(iNewValue);
	m_AnimFloatList[1].StopAnim();
	return m_AnimFloatList[0].Get();
}

void HyAnimVec2::SetX(float fNewValue)
{
	m_AnimFloatList[0] = fNewValue;
	m_AnimFloatList[1].StopAnim();
}

void HyAnimVec2::SetX(int32 iNewValue)
{
	m_AnimFloatList[0] = static_cast<float>(iNewValue);
	m_AnimFloatList[1].StopAnim();
}

float HyAnimVec2::Y() const
{
	return m_AnimFloatList[1].Get();
}

float HyAnimVec2::Y(float fNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = fNewValue;
	return m_AnimFloatList[1].Get();
}

float HyAnimVec2::Y(int32 iNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = static_cast<float>(iNewValue);
	return m_AnimFloatList[1].Get();
}

void HyAnimVec2::SetY(float fNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = fNewValue;
}

void HyAnimVec2::SetY(int32 iNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = static_cast<float>(iNewValue);
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

void HyAnimVec2::Tween(int32 iX, int32 iY, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	Tween(static_cast<float>(iX), static_cast<float>(iY), fSeconds, fpTween, fpFinishedCallback);
}

void HyAnimVec2::Tween(float fX, float fY, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].Tween(fX, fSeconds, fpTween, fpFinishedCallback);
	m_AnimFloatList[1].Tween(fY, fSeconds, fpTween);
}

void HyAnimVec2::TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].TweenOffset(fOffsetX, fSeconds, fpTween, fpFinishedCallback);
	m_AnimFloatList[1].TweenOffset(fOffsetY, fSeconds, fpTween);
}

void HyAnimVec2::Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /* = HyAnimFloat::NullTweenCallback */)
{
	// Quadratic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio)
		{
			fRatio = fpTween(fRatio);
			auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3;
			return p.x;
		}, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio)
		{
			fRatio = fpTween(fRatio);
			auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3;
			return p.y;
		});
}

void HyAnimVec2::Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, const glm::vec2 &pt4, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	// Cubic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio)
		{
			fRatio = fpTween(fRatio);
			auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4;
			return p.x;
		}, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio)
		{
			fRatio = fpTween(fRatio);
			auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4;
			return p.y;
		});
}

void HyAnimVec2::Displace(float fX, float fY)
{
	m_AnimFloatList[0].Displace(fX);
	m_AnimFloatList[1].Displace(fY);
}

void HyAnimVec2::Displace(const glm::vec2 &srcVec)
{
	m_AnimFloatList[0].Displace(srcVec[0]);
	m_AnimFloatList[1].Displace(srcVec[1]);
}

void HyAnimVec2::Displace(const glm::ivec2 &srcVec)
{
	m_AnimFloatList[0].Displace(srcVec[0]);
	m_AnimFloatList[1].Displace(srcVec[1]);
}

void HyAnimVec2::Displace(const HyAnimVec2 &srcVec)
{
	m_AnimFloatList[0].Displace(srcVec[0]);
	m_AnimFloatList[1].Displace(srcVec[1]);
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

glm::vec2 HyAnimVec2::GetAnimDestination() const
{
	return glm::vec2(m_AnimFloatList[0].GetAnimDestination(), m_AnimFloatList[1].GetAnimDestination());
}

float HyAnimVec2::GetAnimRemainingDuration() const
{
	return m_AnimFloatList[0].GetAnimRemainingDuration();
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

HyAnimVec2 &HyAnimVec2::operator=(const HyAnimVec2 &rhs)
{
	m_AnimFloatList[0] = rhs[0];
	m_AnimFloatList[1] = rhs[1];

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

HyAnimVec2 &HyAnimVec2::operator=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] = rhs[0];
	m_AnimFloatList[1] = rhs[1];

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
