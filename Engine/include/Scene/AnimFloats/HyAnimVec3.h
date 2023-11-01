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
	HyAnimFloat *				m_AnimFloatList;

public:
	HyAnimVec3(IHyNode &ownerRef, uint32 uiDirtyFlags);
	HyAnimVec3(const HyAnimVec3 &) = delete;
	HyAnimVec3(HyAnimVec3 &&) = delete;
	~HyAnimVec3();

	const glm::vec3 &Get() const;
	HyAnimFloat &GetAnimFloat(uint32 uiIndex);
	glm::vec3 Extrapolate(float fExtrapolatePercent) const;

	float X() const;
	float X(float fNewValue);
	float X(int32 iNewValue);
	float GetX() const;
	void SetX(float fNewValue);
	void SetX(int32 iNewValue);

	float Y() const;
	float Y(float fNewValue);
	float Y(int32 iNewValue);
	float GetY() const;
	void SetY(float fNewValue);
	void SetY(int32 iNewValue);

	float Z() const;
	float Z(float fNewValue);
	float Z(int32 iNewValue);
	float GetZ() const;
	void SetZ(float fNewValue);
	void SetZ(int32 iNewValue);

	void Set(float fAll);
	void Set(float fX, float fY, float fZ);
	void Set(const glm::vec3 &srcVec);
	void Set(const glm::ivec3 &srcVec);
	void Set(const HyAnimVec3 &srcVec);

	void Offset(float fX, float fY, float fZ);
	void Offset(const glm::vec3 &srcVec);
	void Offset(const glm::ivec3 &srcVec);
	void Offset(const HyAnimVec3 &srcVec);

	void Tween(float fX, float fY, float fZ, float fSeconds, HyTweenFunc fpEase = HyTween::Linear, float fDeferStart = 0.0f, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenFunc fpEase = HyTween::Linear, float fDeferStart = 0.0f, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	void Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);
	void Bezier(const glm::vec3 &pt1, const glm::vec3 &pt2, const glm::vec3 &pt3, const glm::vec3 &pt4, float fSeconds, HyTweenFunc fpTween = HyTween::Linear, float fDeferStart = 0.0f, HyAnimFinishedCallback fpFinishedCallback = HyAnimFloat::NullFinishedCallback);

	void Displace(float fX, float fY, float fZ);
	void Displace(const glm::vec3 &srcVec);
	void Displace(const glm::ivec3 &srcVec);
	void Displace(const HyAnimVec3 &srcVec);

	bool IsAnimating();
	void StopAnim();

	glm::vec3 GetAnimDestination() const;
	float GetAnimRemainingDuration() const;

	HyAnimVec3 &operator+=(float rhs);
	HyAnimVec3 &operator-=(float rhs);
	HyAnimVec3 &operator*=(float rhs);
	HyAnimVec3 &operator/=(float rhs);

	HyAnimVec3 &operator=(const HyAnimVec3 &rhs);
	HyAnimVec3 &operator+=(const HyAnimVec3 &rhs);
	HyAnimVec3 &operator-=(const HyAnimVec3 &rhs);
	HyAnimVec3 &operator*=(const HyAnimVec3 &rhs);
	HyAnimVec3 &operator/=(const HyAnimVec3 &rhs);

	HyAnimVec3 &operator=(const glm::vec3 &rhs);
	HyAnimVec3 &operator+=(const glm::vec3 &rhs);
	HyAnimVec3 &operator-=(const glm::vec3 &rhs);
	HyAnimVec3 &operator*=(const glm::vec3 &rhs);
	HyAnimVec3 &operator/=(const glm::vec3 &rhs);

	float operator[](int i) const;
};

#endif /* HyAnimVec3_h__ */
