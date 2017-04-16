/**************************************************************************
*	HyTweenVec2.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Tweens/HyTweenVec2.h"

HyTweenVec2::HyTweenVec2(IHyNode &ownerRef, HyNodeDirtyType eDirtyType)
{
	m_AnimFloatList.push_back(HyTweenFloat(m_vValue[0], ownerRef, eDirtyType));
	m_AnimFloatList.push_back(HyTweenFloat(m_vValue[1], ownerRef, eDirtyType));
}

HyTweenVec2::~HyTweenVec2()
{ }

const glm::vec2 &HyTweenVec2::Get() const
{
	return m_vValue;
}

float HyTweenVec2::X() const
{
	return m_AnimFloatList[0].Get();
}

void HyTweenVec2::X(float fValue)
{
	m_AnimFloatList[0] = fValue;
	m_AnimFloatList[1].StopTween();
}

void HyTweenVec2::X(int32 iValue)
{
	m_AnimFloatList[0] = static_cast<float>(iValue);
	m_AnimFloatList[1].StopTween();
}

float HyTweenVec2::Y() const
{
	return m_AnimFloatList[1].Get();
}

void HyTweenVec2::Y(float fValue)
{
	m_AnimFloatList[0].StopTween();
	m_AnimFloatList[1] = fValue;
}

void HyTweenVec2::Y(int32 iValue)
{
	m_AnimFloatList[0].StopTween();
	m_AnimFloatList[1] = static_cast<float>(iValue);
}

void HyTweenVec2::Set(float fAll)
{
	m_AnimFloatList[0].Set(fAll);
	m_AnimFloatList[1].Set(fAll);
}

void HyTweenVec2::Set(int32 iX, int32 iY)
{
	m_AnimFloatList[0] = static_cast<float>(iX);
	m_AnimFloatList[1] = static_cast<float>(iY);
}

void HyTweenVec2::Set(float fX, float fY)
{
	m_AnimFloatList[0] = fX;
	m_AnimFloatList[1] = fY;
}

void HyTweenVec2::Set(const glm::vec2 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
}

void HyTweenVec2::Set(const glm::ivec2 &srcVec)
{
	m_AnimFloatList[0].Set(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Set(static_cast<float>(srcVec[1]));
}

void HyTweenVec2::Set(const HyTweenVec2 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
}

void HyTweenVec2::Offset(float fX, float fY)
{
	m_AnimFloatList[0] += fX;
	m_AnimFloatList[1] += fY;
}

void HyTweenVec2::Offset(const glm::vec2 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
}

void HyTweenVec2::Offset(const glm::ivec2 &srcVec)
{
	m_AnimFloatList[0].Offset(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Offset(static_cast<float>(srcVec[1]));
}

void HyTweenVec2::Offset(const HyTweenVec2 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
}

void HyTweenVec2::Tween(int32 iX, int32 iY, float fSeconds, HyTweenUpdateFunc fpEase /*= HyTween::Linear*/, HyTweenFinishedCallback tweenFinishedCallback /*= HyTween::NullTweenCallback*/)
{
	Tween(static_cast<float>(iX), static_cast<float>(iY), fSeconds, fpEase, tweenFinishedCallback);
}

void HyTweenVec2::Tween(float fX, float fY, float fSeconds, HyTweenUpdateFunc fpEase /*= HyTween::Linear*/, HyTweenFinishedCallback tweenFinishedCallback /*= HyTween::NullTweenCallback*/)
{
	m_AnimFloatList[0].Tween(fX, fSeconds, fpEase, tweenFinishedCallback);
	m_AnimFloatList[1].Tween(fY, fSeconds, fpEase);
}

void HyTweenVec2::TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenUpdateFunc fpEase /*= HyTween::Linear*/, HyTweenFinishedCallback tweenFinishedCallback /*= HyTween::NullTweenCallback*/)
{
	m_AnimFloatList[0].TweenOffset(fOffsetX, fSeconds, fpEase, tweenFinishedCallback);
	m_AnimFloatList[1].TweenOffset(fOffsetY, fSeconds, fpEase);
}

bool HyTweenVec2::IsTweening()
{
	return (m_AnimFloatList[0].IsTweening() || m_AnimFloatList[1].IsTweening());
}

HyTweenVec2 &HyTweenVec2::operator+=(float rhs)
{
	m_AnimFloatList[0] += rhs;
	m_AnimFloatList[1] += rhs;

	return *this;
}

HyTweenVec2 &HyTweenVec2::operator-=(float rhs)
{
	m_AnimFloatList[0] -= rhs;
	m_AnimFloatList[1] -= rhs;
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator*=(float rhs)
{ 
	m_AnimFloatList[0] *= rhs;
	m_AnimFloatList[1] *= rhs;
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator/=(float rhs)
{
	m_AnimFloatList[0] /= rhs;
	m_AnimFloatList[1] /= rhs;
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator+=(const HyTweenVec2 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator-=(const HyTweenVec2 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1];
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator*=(const HyTweenVec2 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1];
	
	return *this;
}
HyTweenVec2 &HyTweenVec2::operator/=(const HyTweenVec2 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1];
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator+=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator-=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1]; 
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator*=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1]; 
	
	return *this;
}

HyTweenVec2 &HyTweenVec2::operator/=(const glm::vec2 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1]; 
	
	return *this;
}

float HyTweenVec2::operator[](int i) const
{
	return m_vValue[i];
}
