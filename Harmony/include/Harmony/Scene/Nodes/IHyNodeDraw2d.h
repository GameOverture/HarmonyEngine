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
#include "Scene/Tweens/HyTweenVec3.h"

class IHyNodeDraw2d : public IHyNode2d
{
protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

	HyScreenRect<int32>				m_LocalScissorRect;
	HyStencil *						m_pStencil;

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

	// NOTE: Below accessors return the data declared in this class. Respective derived classes have the corresponding mutators (whether it's a "leaf" or "entity")

	bool IsScissorSet() const;
	const HyScreenRect<int32> &GetScissor() const;

	bool IsStencilSet() const;
	HyStencil *GetStencil() const;

	int32 GetDisplayOrder() const;

protected:
	virtual void NodeUpdate() = 0;

private:
	void Calculate();
};

#endif /* IHyNodeDraw2d_h__ */
