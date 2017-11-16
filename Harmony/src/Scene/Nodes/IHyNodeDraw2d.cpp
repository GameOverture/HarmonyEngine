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
#include "Renderer/IHyRenderer.h"

IHyNodeDraw2d::IHyNodeDraw2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent),
																		m_fAlpha(1.0f),
																		m_fCachedAlpha(1.0f),
																		m_hStencil(HY_UNUSED_HANDLE),
																		m_iDisplayOrder(0),
																		topColor(*this, DIRTY_Color),
																		botColor(*this, DIRTY_Color),
																		alpha(m_fAlpha, *this, DIRTY_Color)
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

bool IHyNodeDraw2d::IsScissorSet() const
{
	return (m_LocalScissorRect.iTag == 1);	// Tag indicates whether it's used
}

const HyScreenRect<int32> &IHyNodeDraw2d::GetScissor() const
{
	return m_LocalScissorRect;
}

bool IHyNodeDraw2d::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyNodeDraw2d::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

int32 IHyNodeDraw2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

void IHyNodeDraw2d::Calculate()
{
	if(IsDirty(DIRTY_Color))
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

		ClearDirty(DIRTY_Color);
	}
}
