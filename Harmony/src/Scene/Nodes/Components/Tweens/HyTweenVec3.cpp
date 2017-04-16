/**************************************************************************
*	HyTweenVec3.cpp
*
*	Harmony Engine
*	Copyright (c) 2016 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Components/Tweens/HyTweenVec3.h"

HyTweenVec3::HyTweenVec3(IHyNode &ownerRef, HyNodeDirtyType eDirtyType)
{
	m_AnimFloatList.push_back(HyTweenFloat(m_vValue[0], ownerRef, eDirtyType));
	m_AnimFloatList.push_back(HyTweenFloat(m_vValue[1], ownerRef, eDirtyType));
	m_AnimFloatList.push_back(HyTweenFloat(m_vValue[2], ownerRef, eDirtyType));
}

HyTweenVec3::~HyTweenVec3()
{ }

const glm::vec3 &HyTweenVec3::Get() const
{
	return m_vValue;
}

float HyTweenVec3::X() const
{
	return m_AnimFloatList[0].Get();
}

void HyTweenVec3::X(float fValue)
{
	m_AnimFloatList[0] = fValue;
	m_AnimFloatList[1].StopTween();
	m_AnimFloatList[2].StopTween();
}

float HyTweenVec3::Y() const
{
	return m_AnimFloatList[1].Get();
}

void HyTweenVec3::Y(float fValue)
{
	m_AnimFloatList[0].StopTween();
	m_AnimFloatList[1] = fValue;
	m_AnimFloatList[2].StopTween();
}

float HyTweenVec3::Z() const
{
	return m_AnimFloatList[2].Get();
}

void HyTweenVec3::Z(float fValue)
{
	m_AnimFloatList[0].StopTween();
	m_AnimFloatList[1].StopTween();
	m_AnimFloatList[2] = fValue;
}

void HyTweenVec3::Set(float fAll)
{
	m_AnimFloatList[0].Set(fAll);
	m_AnimFloatList[1].Set(fAll);
	m_AnimFloatList[2].Set(fAll);
}

void HyTweenVec3::Set(float fX, float fY, float fZ)
{
	m_AnimFloatList[0] = fX;
	m_AnimFloatList[1] = fY;
	m_AnimFloatList[2] = fZ;
}

void HyTweenVec3::Set(const glm::vec3 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
	m_AnimFloatList[2].Set(srcVec[2]);
}

void HyTweenVec3::Set(const glm::ivec3 &srcVec)
{
	m_AnimFloatList[0].Set(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Set(static_cast<float>(srcVec[1]));
	m_AnimFloatList[2].Set(static_cast<float>(srcVec[2]));
}

void HyTweenVec3::Set(const HyTweenVec3 &srcVec)
{
	m_AnimFloatList[0].Set(srcVec[0]);
	m_AnimFloatList[1].Set(srcVec[1]);
	m_AnimFloatList[2].Set(srcVec[2]);
}

void HyTweenVec3::Offset(float fX, float fY, float fZ)
{
	m_AnimFloatList[0] += fX;
	m_AnimFloatList[1] += fY;
	m_AnimFloatList[2] += fZ;
}

void HyTweenVec3::Offset(const glm::vec3 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
	m_AnimFloatList[2].Offset(srcVec[2]);
}

void HyTweenVec3::Offset(const glm::ivec3 &srcVec)
{
	m_AnimFloatList[0].Offset(static_cast<float>(srcVec[0]));
	m_AnimFloatList[1].Offset(static_cast<float>(srcVec[1]));
	m_AnimFloatList[2].Offset(static_cast<float>(srcVec[2]));
}

void HyTweenVec3::Offset(const HyTweenVec3 &srcVec)
{
	m_AnimFloatList[0].Offset(srcVec[0]);
	m_AnimFloatList[1].Offset(srcVec[1]);
	m_AnimFloatList[2].Offset(srcVec[2]);
}

void HyTweenVec3::Tween(float fX, float fY, float fZ, float fSeconds, HyTweenUpdateFunc fpEase /*= HyTween::Linear*/, HyTweenFinishedCallback tweenFinishedCallback /*= HyTween::NullTweenCallback*/)
{
	m_AnimFloatList[0].Tween(fX, fSeconds, fpEase, tweenFinishedCallback);
	m_AnimFloatList[1].Tween(fY, fSeconds, fpEase);
	m_AnimFloatList[2].Tween(fZ, fSeconds, fpEase);
}

void HyTweenVec3::TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenUpdateFunc fpEase /*= HyTween::Linear*/, HyTweenFinishedCallback tweenFinishedCallback /*= HyTween::NullTweenCallback*/)
{
	m_AnimFloatList[0].TweenOffset(fOffsetX, fSeconds, fpEase, tweenFinishedCallback);
	m_AnimFloatList[1].TweenOffset(fOffsetY, fSeconds, fpEase);
	m_AnimFloatList[2].TweenOffset(fOffsetZ, fSeconds, fpEase);
}

HyTweenVec3 &HyTweenVec3::operator+=(float rhs)
{
	m_AnimFloatList[0] += rhs;
	m_AnimFloatList[1] += rhs;
	m_AnimFloatList[2] += rhs;
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator-=(float rhs)
{
	m_AnimFloatList[0] -= rhs;
	m_AnimFloatList[1] -= rhs;
	m_AnimFloatList[2] -= rhs;
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator*=(float rhs)
{
	m_AnimFloatList[0] *= rhs;
	m_AnimFloatList[1] *= rhs;
	m_AnimFloatList[2] *= rhs;
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator/=(float rhs)
{
	m_AnimFloatList[0] /= rhs;
	m_AnimFloatList[1] /= rhs;
	m_AnimFloatList[2] /= rhs;
	
	return *this;
}


HyTweenVec3 &HyTweenVec3::operator+=(const HyTweenVec3 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	m_AnimFloatList[2] += rhs[2];
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator-=(const HyTweenVec3 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1];
	m_AnimFloatList[2] -= rhs[2];
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator*=(const HyTweenVec3 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1];
	m_AnimFloatList[2] *= rhs[2];
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator/=(const HyTweenVec3 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1];
	m_AnimFloatList[2] /= rhs[2];
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator+=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] += rhs[0];
	m_AnimFloatList[1] += rhs[1];
	m_AnimFloatList[2] += rhs[2];
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator-=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] -= rhs[0];
	m_AnimFloatList[1] -= rhs[1];
	m_AnimFloatList[2] -= rhs[2];
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator*=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] *= rhs[0];
	m_AnimFloatList[1] *= rhs[1];
	m_AnimFloatList[2] *= rhs[2];
	
	return *this;
}

HyTweenVec3 &HyTweenVec3::operator/=(const glm::vec3 &rhs)
{
	m_AnimFloatList[0] /= rhs[0];
	m_AnimFloatList[1] /= rhs[1];
	m_AnimFloatList[2] /= rhs[2];
	
	return *this;
}

float HyTweenVec3::operator[](int i) const
{
	return m_vValue[i];
}
