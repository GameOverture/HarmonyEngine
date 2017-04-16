/**************************************************************************
*	HyColor.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyColor_h__
#define __HyColor_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Components/Tweens/HyTweenVec3.h"

class IHyNode2d;

class HyColor
{
	IHyNode2d &						m_OwnerRef;

	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

public:
	HyTweenVec3						topColor;
	HyTweenVec3						botColor;
	HyTweenFloat					alpha;

	HyColor(IHyNode2d &ownerRef);
	~HyColor();

	void SetTint(float fR, float fG, float fB);
	void SetTint(uint32 uiColor);

	float CalculateAlpha();
	const glm::vec3 &CalculateTopTint();
	const glm::vec3 &CalculateBotTint();

private:
	void Calculate();
};

#endif /* __HyColor_h__ */
