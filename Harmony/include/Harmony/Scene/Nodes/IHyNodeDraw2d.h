/**************************************************************************
*	IHyNodeDraw2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNodeDraw2d_h__
#define IHyNodeDraw2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Tweens/HyTweenVec3.h"

class IHyNodeDraw2d : public IHyNode2d
{
protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

	HyScreenRect<int32>				m_LocalScissorRect;
	HyScreenRect<int32>				m_WorldScissorRect;

	int32							m_iDisplayOrder;	// Higher values are displayed front-most

public:
	HyTweenVec3						topColor;
	HyTweenVec3						botColor;
	HyTweenFloat					alpha;

public:
	IHyNodeDraw2d(HyType eNodeType, HyEntity2d *pParent);
	virtual ~IHyNodeDraw2d();

	void SetTint(float fR, float fG, float fB);
	void SetTint(uint32 uiColor);

	float CalculateAlpha();
	const glm::vec3 &CalculateTopTint();
	const glm::vec3 &CalculateBotTint();

	bool IsScissorSet();
	const HyScreenRect<int32> &GetScissor();
	void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	void ClearScissor();

protected:
	virtual void NodeUpdate() = 0;

private:
	void Calculate();
};

#endif /* IHyNodeDraw2d_h__ */
