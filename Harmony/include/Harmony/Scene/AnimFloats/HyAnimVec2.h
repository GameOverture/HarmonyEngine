/**************************************************************************
 *	HyAnimVec2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAnimVec2_h__
#define HyAnimVec2_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimFloat.h"

class HyAnimVec2
{
	glm::vec2					m_vValue;
	std::vector<HyAnimFloat>	m_AnimFloatList;

public:
	HyAnimVec2(IHyNode &ownerRef, uint32 uiDirtyFlags);
	~HyAnimVec2();

	const glm::vec2 &Get() const;
	const glm::vec3 Extrapolate() const;

	float X() const;
	void X(float fValue);
	void X(int32 iValue);

	float Y() const;
	void Y(float fValue);
	void Y(int32 iValue);

	void Set(float fAll);
	void Set(int32 iX, int32 iY);
	void Set(float fX, float fY);
	void Set(const glm::vec2 &srcVec);
	void Set(const glm::vec3 &srcVec);
	void Set(const glm::ivec2 &srcVec);
	void Set(const HyAnimVec2 &srcVec);

	void Offset(float fX, float fY);
	void Offset(const glm::vec2 &srcVec);
	void Offset(const glm::ivec2 &srcVec);
	void Offset(const HyAnimVec2 &srcVec);

	void Tween(int32 iX, int32 iY, float fSeconds, HyTweenFunc fpEase = HyTween::Linear, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void Tween(float fX, float fY, float fSeconds, HyTweenFunc fpEase = HyTween::Linear, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenFunc fpEase = HyTween::Linear, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	void Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, float fSeconds, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void Bezier(const glm::vec2 &pt1, const glm::vec2 &pt2, const glm::vec2 &pt3, const glm::vec2 &pt4, float fSeconds, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	
	bool IsAnimating();
	void StopAnim();

	glm::vec2 GetTweenDestination() const;
	float GetTweenRemainingDuration() const;

	HyAnimVec2 &operator+=(float rhs);
	HyAnimVec2 &operator-=(float rhs);
	HyAnimVec2 &operator*=(float rhs);
	HyAnimVec2 &operator/=(float rhs);

	HyAnimVec2 &operator+=(const HyAnimVec2 &rhs);
	HyAnimVec2 &operator-=(const HyAnimVec2 &rhs);
	HyAnimVec2 &operator*=(const HyAnimVec2 &rhs);
	HyAnimVec2 &operator/=(const HyAnimVec2 &rhs);

	HyAnimVec2 &operator+=(const glm::vec2 &rhs);
	HyAnimVec2 &operator-=(const glm::vec2 &rhs);
	HyAnimVec2 &operator*=(const glm::vec2 &rhs);
	HyAnimVec2 &operator/=(const glm::vec2 &rhs);

	float operator [](int i) const;
};

#endif /* HyAnimVec2_h__ */
