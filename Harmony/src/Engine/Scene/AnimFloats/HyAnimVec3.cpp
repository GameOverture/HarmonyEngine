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
	m_AnimFloatList.push_back(HyAnimFloat(m_vValue[0], ownerRef, uiDirtyFlags));
	m_AnimFloatList.push_back(HyAnimFloat(m_vValue[1], ownerRef, uiDirtyFlags));
	m_AnimFloatList.push_back(HyAnimFloat(m_vValue[2], ownerRef, uiDirtyFlags));
}

HyAnimVec3::~HyAnimVec3()
{ }

const glm::vec3 &HyAnimVec3::Get() const
{
	return m_vValue;
}

float HyAnimVec3::X() const
{
	return m_AnimFloatList[0].Get();
}

void HyAnimVec3::X(float fValue)
{
	m_AnimFloatList[0] = fValue;
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2].StopAnim();
}

float HyAnimVec3::Y() const
{
	return m_AnimFloatList[1].Get();
}

void HyAnimVec3::Y(float fValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1] = fValue;
	m_AnimFloatList[2].StopAnim();
}

float HyAnimVec3::Z() const
{
	return m_AnimFloatList[2].Get();
}

void HyAnimVec3::Z(float fValue)
{
	m_AnimFloatList[0].StopAnim();
	m_AnimFloatList[1].StopAnim();
	m_AnimFloatList[2] = fValue;
}

void HyAnimVec3::Set(float fAll)
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

void HyAnimVec3::Tween(float fX, float fY, float fZ, float fSeconds, HyTweenFunc fpEase /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].Tween(fX, fSeconds, fpEase, fpFinishedCallback);
	m_AnimFloatList[1].Tween(fY, fSeconds, fpEase);
	m_AnimFloatList[2].Tween(fZ, fSeconds, fpEase);
}

void HyAnimVec3::TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenFunc fpEase /*= HyTween::Linear*/, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullTweenCallback*/)
{
	m_AnimFloatList[0].TweenOffset(fOffsetX, fSeconds, fpEase, fpFinishedCallback);
	m_AnimFloatList[1].TweenOffset(fOffsetY, fSeconds, fpEase);
	m_AnimFloatList[2].TweenOffset(fOffsetZ, fSeconds, fpEase);
}

void HyAnimVec3::Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, float fSeconds, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullFinishedCallback*/)
{
	// Quadratic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.x; }, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.y; });
	m_AnimFloatList[2].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio)) * pt1 + 2 * (1-fRatio) * fRatio * pt2 + fRatio * fRatio * pt3; return p.z; });
}

void HyAnimVec3::Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, const glm::vec3 &pt4, float fSeconds, HyAnimFinishedCallback fpFinishedCallback /*= HyAnimFloat::NullFinishedCallback*/)
{
	// Cubic Bezier
	m_AnimFloatList[0].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.x; }, fpFinishedCallback);
	m_AnimFloatList[1].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.y; });
	m_AnimFloatList[2].Proc(fSeconds, [=](float fRatio) { auto p = ((1-fRatio) * (1-fRatio) * (1-fRatio)) * pt1 + 3 * fRatio * ((1-fRatio) * (1-fRatio)) * pt2 + 3 * (fRatio * fRatio) * (1-fRatio) * pt3 + (fRatio * fRatio * fRatio) * pt4; return p.z; });
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
