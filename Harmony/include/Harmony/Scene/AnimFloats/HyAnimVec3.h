/**************************************************************************
 *	HyAnimVec3.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAnimVec3_h__
#define HyAnimVec3_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimFloat.h"

class HyAnimVec3
{
	glm::vec3					m_vValue;
	std::vector<HyAnimFloat>	m_AnimFloatList;

public:
	HyAnimVec3(IHyNode &ownerRef, uint32 uiDirtyFlags);
	~HyAnimVec3();

	const glm::vec3 &Get() const;

	float X() const;
	void X(float fValue);

	float Y() const;
	void Y(float fValue);

	float Z() const;
	void Z(float fValue);

	void Set(float fAll);
	void Set(float fX, float fY, float fZ);
	void Set(const glm::vec3 &srcVec);
	void Set(const glm::ivec3 &srcVec);
	void Set(const HyAnimVec3 &srcVec);

	void Offset(float fX, float fY, float fZ);
	void Offset(const glm::vec3 &srcVec);
	void Offset(const glm::ivec3 &srcVec);
	void Offset(const HyAnimVec3 &srcVec);

	void Tween(float fX, float fY, float fZ, float fSeconds, HyTweenFunc fpEase = HyTween::Linear, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenFunc fpEase = HyTween::Linear, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	void Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, float fSeconds, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, const glm::vec3 &pt4, float fSeconds, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	bool IsAnimating();
	void StopAnim();

	HyAnimVec3 &operator+=(float rhs);
	HyAnimVec3 &operator-=(float rhs);
	HyAnimVec3 &operator*=(float rhs);
	HyAnimVec3 &operator/=(float rhs);

	HyAnimVec3 &operator+=(const HyAnimVec3 &rhs);
	HyAnimVec3 &operator-=(const HyAnimVec3 &rhs);
	HyAnimVec3 &operator*=(const HyAnimVec3 &rhs);
	HyAnimVec3 &operator/=(const HyAnimVec3 &rhs);

	HyAnimVec3 &operator+=(const glm::vec3 &rhs);
	HyAnimVec3 &operator-=(const glm::vec3 &rhs);
	HyAnimVec3 &operator*=(const glm::vec3 &rhs);
	HyAnimVec3 &operator/=(const glm::vec3 &rhs);

	float operator[](int i) const;
};

#endif /* HyAnimVec3_h__ */
