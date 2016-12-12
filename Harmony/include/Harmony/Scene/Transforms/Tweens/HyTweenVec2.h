/**************************************************************************
 *	HyTweenVec2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyTweenVec2_h__
#define __HyTweenVec2_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Transforms/Tweens/HyTweenFloat.h"

#include <vector>

class HyTweenVec2
{
	glm::vec2					m_vValue;
	std::vector<HyTweenFloat>	m_AnimFloatList;

public:
	HyTweenVec2(IHyTransformNode &ownerRef);
	~HyTweenVec2();

	const glm::vec2 &Get() const;

	float X() const;
	void X(float fValue);

	float Y() const;
	void Y(float fValue);

	void Set(float fAll);
	void Set(float fX, float fY);
	void Set(const glm::vec2 &srcVec);
	void Set(const HyTweenVec2 &srcVec);

	void Offset(float fX, float fY);
	void Offset(const glm::vec2 &srcVec);
	void Offset(const HyTweenVec2 &srcVec);

	void Tween(float fX, float fY, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback);
	void TweenOffset(float fOffsetX, float fOffsetY, float fSeconds, HyTweenUpdateFunc fpEase = HyTween::Linear, HyTweenFinishedCallback tweenFinishedCallback = HyTween::_NullTweenCallback);
	
	bool IsTweening();

	HyTweenVec2 &operator+=(float rhs);
	HyTweenVec2 &operator-=(float rhs);
	HyTweenVec2 &operator*=(float rhs);
	HyTweenVec2 &operator/=(float rhs);

	HyTweenVec2 &operator+=(const HyTweenVec2 &rhs);
	HyTweenVec2 &operator-=(const HyTweenVec2 &rhs);
	HyTweenVec2 &operator*=(const HyTweenVec2 &rhs);
	HyTweenVec2 &operator/=(const HyTweenVec2 &rhs);

	HyTweenVec2 &operator+=(const glm::vec2 &rhs);
	HyTweenVec2 &operator-=(const glm::vec2 &rhs);
	HyTweenVec2 &operator*=(const glm::vec2 &rhs);
	HyTweenVec2 &operator/=(const glm::vec2 &rhs);

	float operator [](int i) const;
};

#endif /* __HyTweenVec2_h__ */