/**************************************************************************
 *	HyTweenVec3.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyTweenVec3_h__
#define HyTweenVec3_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Tweens/HyTweenFloat.h"

#include <vector>

class HyTweenVec3
{
	glm::vec3					m_vValue;
	std::vector<HyTweenFloat>	m_AnimFloatList;

public:
	HyTweenVec3(IHyNode &ownerRef, uint32 uiDirtyFlags);
	~HyTweenVec3();

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
	void Set(const HyTweenVec3 &srcVec);

	void Offset(float fX, float fY, float fZ);
	void Offset(const glm::vec3 &srcVec);
	void Offset(const glm::ivec3 &srcVec);
	void Offset(const HyTweenVec3 &srcVec);

	void Tween(float fX, float fY, float fZ, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);
	void TweenOffset(float fOffsetX, float fOffsetY, float fOffsetZ, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::NullTweenCallback);

	HyTweenVec3 &operator+=(float rhs);
	HyTweenVec3 &operator-=(float rhs);
	HyTweenVec3 &operator*=(float rhs);
	HyTweenVec3 &operator/=(float rhs);

	HyTweenVec3 &operator+=(const HyTweenVec3 &rhs);
	HyTweenVec3 &operator-=(const HyTweenVec3 &rhs);
	HyTweenVec3 &operator*=(const HyTweenVec3 &rhs);
	HyTweenVec3 &operator/=(const HyTweenVec3 &rhs);

	HyTweenVec3 &operator+=(const glm::vec3 &rhs);
	HyTweenVec3 &operator-=(const glm::vec3 &rhs);
	HyTweenVec3 &operator*=(const glm::vec3 &rhs);
	HyTweenVec3 &operator/=(const glm::vec3 &rhs);

	float operator[](int i) const;
};

#endif /* HyTweenVec3_h__ */
