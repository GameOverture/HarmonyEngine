/**************************************************************************
*	IHyNodeDraw2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/IHyNodeDraw2d.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"

IHyNodeDraw2d::IHyNodeDraw2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent),
																		m_fAlpha(1.0f),
																		m_fCachedAlpha(1.0f),
																		m_iDisplayOrder(0),
																		topColor(*this, HYNODEDIRTY_Color),
																		botColor(*this, HYNODEDIRTY_Color),
																		alpha(m_fAlpha, *this, HYNODEDIRTY_Color)
{
	topColor.Set(1.0f);
	botColor.Set(1.0f);
	m_CachedTopColor = topColor.Get();
	m_CachedBotColor = botColor.Get();
}

IHyNodeDraw2d::~IHyNodeDraw2d()
{
}

void IHyNodeDraw2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyNodeDraw2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyNodeDraw2d::CalculateAlpha()
{
	Calculate();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyNodeDraw2d::CalculateTopTint()
{
	Calculate();
	return m_CachedTopColor;
}

const glm::vec3 &IHyNodeDraw2d::CalculateBotTint()
{
	Calculate();
	return m_CachedBotColor;
}

void IHyNodeDraw2d::Calculate()
{
	if(IsDirty(HYNODEDIRTY_Color))
	{
		m_fCachedAlpha = alpha.Get();
		m_CachedTopColor = topColor.Get();
		m_CachedBotColor = botColor.Get();

		if(m_pParent)
		{
			m_fCachedAlpha *= m_pParent->CalculateAlpha();
			m_CachedTopColor *= m_pParent->CalculateTopTint();
			m_CachedBotColor *= m_pParent->CalculateTopTint();
		}

		ClearDirty(HYNODEDIRTY_Color);
	}
}
