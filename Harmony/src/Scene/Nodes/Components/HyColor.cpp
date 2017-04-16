/**************************************************************************
*	HyColor.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Components/HyColor.h"
#include "Scene/Nodes/Entities/HyEntity2d.h"
#include "Scene/Nodes/IHyNode2d.h"

HyColor::HyColor(IHyNode2d &ownerRef) :	m_OwnerRef(ownerRef),
										m_fAlpha(1.0f),
										topColor(m_OwnerRef, HYNODEDIRTY_Color),
										botColor(m_OwnerRef, HYNODEDIRTY_Color),
										alpha(m_fAlpha, m_OwnerRef, HYNODEDIRTY_Color)
{
	topColor.Set(1.0f);
	botColor.Set(1.0f);
}

HyColor::~HyColor()
{
}

void HyColor::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void HyColor::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float HyColor::CalculateAlpha()
{
	Calculate();
	return m_fCachedAlpha;
}

const glm::vec3 &HyColor::CalculateTopTint()
{
	Calculate();
	return m_CachedTopColor;
}

const glm::vec3 &HyColor::CalculateBotTint()
{
	Calculate();
	return m_CachedBotColor;
}

void HyColor::Calculate()
{
	if(m_OwnerRef.IsDirty(HYNODEDIRTY_Color))
	{
		m_fCachedAlpha = alpha.Get();
		m_CachedTopColor = topColor.Get();
		m_CachedBotColor = botColor.Get();

		if(m_OwnerRef.m_pParent)
		{
			m_fCachedAlpha *= m_OwnerRef.m_pParent->color.CalculateAlpha();
			m_CachedTopColor *= m_OwnerRef.m_pParent->color.CalculateTopTint();
			m_CachedBotColor *= m_OwnerRef.m_pParent->color.CalculateTopTint();
		}

		m_OwnerRef.ClearDirty(HYNODEDIRTY_Color);
	}
}
