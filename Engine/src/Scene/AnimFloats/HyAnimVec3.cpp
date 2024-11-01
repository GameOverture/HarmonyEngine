/**************************************************************************
*	HyAnimVec3.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimVec3.h"

HyAnimVec3::HyAnimVec3(IHyNode &ownerRef, uint32 uiDirtyFlags)
{
	m_AnimFloatList = reinterpret_cast<HyAnimFloat *>(HY_NEW uint8[sizeof(HyAnimFloat) * 3]);
	HyAnimFloat *pAnimFloatWriteLocation = m_AnimFloatList;

	for(uint32 i = 0; i < 3; ++i, ++pAnimFloatWriteLocation)
		new (pAnimFloatWriteLocation) HyAnimFloat(m_vValue[i], ownerRef, uiDirtyFlags);
}

HyAnimVec3::~HyAnimVec3()
{
	for(uint32 i = 0; i < 3; ++i)
		m_AnimFloatList[i].~HyAnimFloat();

	uint8 *pAnimFloatBuffer = reinterpret_cast<uint8 *>(m_AnimFloatList);
	delete[] pAnimFloatBuffer;
}

const glm::vec3 &HyAnimVec3::Get() const
{
	return m_vValue;
}

HyAnimFloat &HyAnimVec3::GetAnimFloat(uint32 uiIndex)
{
	return m_AnimFloatList[uiIndex];
}

glm::vec3 HyAnimVec3::Extrapolate(float fExtrapolatePercent) const
{
	return glm::vec3(m_AnimFloatList[0].Extrapolate(fExtrapolatePercent),
					 m_AnimFloatList[1].Extrapolate(fExtrapolatePercent),
					 m_AnimFloatList[2].Extrapolate(fExtrapolatePercent));
}

float HyAnimVec3::X() const
{
	return m_AnimFloatList[0].Get();
}

float HyAnimVec3::X(float fNewValue)
{
	m_AnimFloatList[0] = fNewValue;
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2].StopAnim();
	return m_AnimFloatList[0].Get();
}

float HyAnimVec3::X(int32 iNewValue)
{
	m_AnimFloatList[0] = static_cast<float>(iNewValue);
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2].StopAnim();
	return m_AnimFloatList[0].Get();
}

float HyAnimVec3::GetX() const
{
	return m_AnimFloatList[0].Get();
}

void HyAnimVec3::SetX(float fNewValue)
{
	m_AnimFloatList[0] = fNewValue;
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2].StopAnim();
}

void HyAnimVec3::SetX(int32 iNewValue)
{
	m_AnimFloatList[0] = static_cast<float>(iNewValue);
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2].StopAnim();
}

float HyAnimVec3::Y() const
{
	return m_AnimFloatList[1].Get();
}

float HyAnimVec3::Y(float fNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = fNewValue;
	m_AnimFloatList[2].StopAnim();
	return m_AnimFloatList[1].Get();
}

float HyAnimVec3::Y(int32 iNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = static_cast<float>(iNewValue);
	m_AnimFloatList[2].StopAnim();
	return m_AnimFloatList[1].Get();
}

float HyAnimVec3::GetY() const
{
	return m_AnimFloatList[1].Get();
}

void HyAnimVec3::SetY(float fNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = fNewValue;
	m_AnimFloatList[2].StopAnim();
}

void HyAnimVec3::SetY(int32 iNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = static_cast<float>(iNewValue);
	m_AnimFloatList[2].StopAnim();
}

float HyAnimVec3::Z() const
{
	return m_AnimFloatList[2].Get();
}

float HyAnimVec3::Z(float fNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2] = fNewValue;
	return m_AnimFloatList[2].Get();
}

float HyAnimVec3::Z(int32 iNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2] = static_cast<float>(iNewValue);
	return m_AnimFloatList[2].Get();
}

float HyAnimVec3::GetZ() const
{
	return m_AnimFloatList[2].Get();
}

void HyAnimVec3::SetZ(float fNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2] = fNewValue;
}

void HyAnimVec3::SetZ(int32 iNewValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2] = static_cast<float>(iNewValue);
}

void HyAnimVec3::SetAll(float fAll)
{
	m_AnimFloatList[0].Set(fAll);
	m_AnimFloatList[1].Set(fAll);
	m_AnimFloatList[2].Set(fAll);
}

void HyAnimVec3::Set(float fX, float fY, float fZ)
{
	m_AnimFloatList[0] = fX;
	m_AnimFloatList[1] = fY;
	m_AnimFloatList[2] = fZ;
}

void HyAnimVec3::Set(int32 iX, int32 iY, int32 iZ)
{
	m_AnimFloatList[0] = static_cast<float>(iX);
	m_AnimFloatList[1] = static_cast<float>(iY);
	m_AnimFloatList[2] = static_cast<float>(iZ);
}

void HyAnimVec3::Set(const glm::vec3 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
	m_AnimFloatList[2].Set(srcVec[2]);
}

void HyAnimVec3::Set(const glm::ivec3 &srcVec)
{
	m_AnimFloatList[0].Set(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Set(static_cast<float>(srcVec[1]));
	m_AnimFloatList[2].Set(static_cast<float>(srcVec[2]));
}

void HyAnimVec3::Set(const HyAnimVec3 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
	m_AnimFloatList[2].Set(srcVec[2]);
}

void HyAnimVec3::Offset(float fX, float fY, float fZ)
{
	m_AnimFloatList[0] += fX;
	m_AnimFloatList[1] += fY;
	m_AnimFloatList[2] += fZ;
}

void HyAnimVec3::Offset(int32 iX, int32 iY, int32 iZ)
{
	m_AnimFloatList[0] += static_cast<float>(iX);
	m_AnimFloatList[1] += static_cast<float>(iY);
	m_AnimFloatList[2] += static_cast<float>(iZ);
}

void HyAnimVec3::Offset(const glm::vec3 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
	m_AnimFloatList[2].Offset(srcVec[2]);
}

void HyAnimVec3::Offset(const glm::ivec3 &srcVec)
{
	m_AnimFloatList[0].Offset(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Offset(static_cast<float>(srcVec[1]));
	m_AnimFloatList[2].Offset(static_cast<float>(srcVec[2]));
}

void HyAnimVec3::Offset(const HyAnimVec3 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
	m_AnimFloatList[2].Offset(srcVec[2]);
}

void HyAnimVec3::Tween(float fX, float fY, float fZ, float fSeconds, HyTweenFunc fpEase /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, std::function<void(IHyNode *)> fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].Tween(fX, fSeconds, fpEase, fDeferStart, fpFinishedCallback);
	m_AnimFloatList[1].Tween(fY, fSeconds, fpEase, fDeferStart);
	m_AnimFloatList[2].Tween(fZ, fSeconds, fpEase, fDeferStart);
}

void HyAnimVec3::TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenFunc fpEase /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, std::function<void(IHyNode *)> fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].TweenOffset(fOffsetX, fSeconds, fpEase, fDeferStart, fpFinishedCallback);
	m_AnimFloatList[1].TweenOffset(fOffsetY, fSeconds, fpEase, fDeferStart);
	m_AnimFloatList[2].TweenOffset(fOffsetZ, fSeconds, fpEase, fDeferStart);
}

void HyAnimVec3::Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, std::function<void(IHyNode *)> fpFinishedCallback /*= HyAnimFloat::NullFinishedCallback*/)
{
	// Quadratic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio) { fRatio = fpTween(fRatio); auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.x; }, fDeferStart, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio) { fRatio = fpTween(fRatio); auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.y; }, fDeferStart);
	m_AnimFloatList[2].Proc(fSeconds, [=](float fRatio) { fRatio = fpTween(fRatio); auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.z; }, fDeferStart);
}

void HyAnimVec3::Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, const glm::vec3 &pt4, float fSeconds, HyTweenFunc fpTween /*= HyTween::Linear*/, float fDeferStart /*= 0.0f*/, std::function<void(IHyNode *)> fpFinishedCallback /*= HyAnimFloat::NullFinishedCallback*/)
{
	// Cubic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio) { fRatio = fpTween(fRatio); auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.x; }, fDeferStart, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio) { fRatio = fpTween(fRatio); auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.y; }, fDeferStart);
	m_AnimFloatList[2].Proc(fSeconds, [=](float fRatio) { fRatio = fpTween(fRatio); auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.z; }, fDeferStart);
}

void HyAnimVec3::Displace(float fX, float fY, float fZ)
{
	m_AnimFloatList[0].Displace(fX);
	m_AnimFloatList[1].Displace(fY);
	m_AnimFloatList[2].Displace(fZ);
}

void HyAnimVec3::Displace(const glm::vec3 &srcVec)
{
	m_AnimFloatList[0].Displace(srcVec[0]);
	m_AnimFloatList[1].Displace(srcVec[1]);
	m_AnimFloatList[2].Displace(srcVec[2]);
}

void HyAnimVec3::Displace(const glm::ivec3 &srcVec)
{
	m_AnimFloatList[0].Displace(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Displace(static_cast<float>(srcVec[1]));
	m_AnimFloatList[2].Displace(static_cast<float>(srcVec[2]));
}

void HyAnimVec3::Displace(const HyAnimVec3 &srcVec)
{
	m_AnimFloatList[0].Displace(srcVec[0]);
	m_AnimFloatList[1].Displace(srcVec[1]);
	m_AnimFloatList[2].Displace(srcVec[2]);
}

void HyAnimVec3::StopAnim()
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2].StopAnim();
}

bool HyAnimVec3::IsAnimating()
{
	return (m_AnimFloatList[0].IsAnimating() || m_AnimFloatList[1].IsAnimating() || m_AnimFloatList[2].IsAnimating());
}

glm::vec3 HyAnimVec3::GetAnimDestination() const
{
	return glm::vec3(m_AnimFloatList[0].GetAnimDestination(), m_AnimFloatList[1].GetAnimDestination(), m_AnimFloatList[2].GetAnimDestination());
}

float HyAnimVec3::GetAnimRemainingDuration() const
{
	return m_AnimFloatList[0].GetAnimRemainingDuration();
}

HyAnimVec3 &HyAnimVec3::operator+=(float rhs)
{
	m_AnimFloatList[0] += rhs;
	m_AnimFloatList[1] += rhs;
	m_AnimFloatList[2] += rhs;
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator-=(float rhs)
{
	m_AnimFloatList[0] -= rhs;
	m_AnimFloatList[1] -= rhs;
	m_AnimFloatList[2] -= rhs;
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator*=(float rhs)
{
	m_AnimFloatList[0] *= rhs;
	m_AnimFloatList[1] *= rhs;
	m_AnimFloatList[2] *= rhs;
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator/=(float rhs)
{
	m_AnimFloatList[0] /= rhs;
	m_AnimFloatList[1] /= rhs;
	m_AnimFloatList[2] /= rhs;
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator=(const HyAnimVec3 &rhs)
{
	m_AnimFloatList[0] = rhs[0];
	m_AnimFloatList[1] = rhs[1];
	m_AnimFloatList[2] = rhs[2];

	return *this;
}

HyAnimVec3 &HyAnimVec3::operator+=(const HyAnimVec3 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	m_AnimFloatList[2] += rhs[2];
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator-=(const HyAnimVec3 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1];
	m_AnimFloatList[2] -= rhs[2];
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator*=(const HyAnimVec3 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1];
	m_AnimFloatList[2] *= rhs[2];
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator/=(const HyAnimVec3 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1];
	m_AnimFloatList[2] /= rhs[2];
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] = rhs[0];
	m_AnimFloatList[1] = rhs[1];
	m_AnimFloatList[2] = rhs[2];

	return *this;
}

HyAnimVec3 &HyAnimVec3::operator+=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	m_AnimFloatList[2] += rhs[2];
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator-=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1];
	m_AnimFloatList[2] -= rhs[2];
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator*=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1];
	m_AnimFloatList[2] *= rhs[2];
	
	return *this;
}

HyAnimVec3 &HyAnimVec3::operator/=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1];
	m_AnimFloatList[2] /= rhs[2];
	
	return *this;
}

float HyAnimVec3::operator[](int i) const
{
	return m_vValue[i];
}
