/**************************************************************************
*	IHyDraw2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Draws/IHyDraw2d.h"
#include "Scene/Nodes/Draws/Entities/HyEntity2d.h"
#include "Renderer/IHyRenderer.h"

IHyDraw2d::IHyDraw2d(HyType eNodeType, HyEntity2d *pParent) :	IHyNode2d(eNodeType, pParent),
																m_fAlpha(1.0f),
																m_fCachedAlpha(1.0f),
																m_pScissor(nullptr),
																m_hStencil(HY_UNUSED_HANDLE),
																m_iCoordinateSystem(-1),
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

IHyDraw2d::IHyDraw2d(const IHyDraw2d &copyRef) :	IHyNode2d(copyRef),
													m_fAlpha(copyRef.m_fAlpha),
													m_pScissor(nullptr),
													m_hStencil(copyRef.m_hStencil),
													m_iCoordinateSystem(copyRef.m_iCoordinateSystem),
													m_iDisplayOrder(copyRef.m_iDisplayOrder),
													topColor(*this, DIRTY_Color),
													botColor(*this, DIRTY_Color),
													alpha(m_fAlpha, *this, DIRTY_Color)

{
	if(copyRef.m_pScissor)
	{
		m_pScissor = HY_NEW ScissorRect();
		m_pScissor->m_LocalScissorRect = copyRef.m_pScissor->m_LocalScissorRect;
		GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}

	topColor.Set(copyRef.topColor.Get());
	botColor.Set(copyRef.botColor.Get());
	alpha.Set(copyRef.alpha.Get());

	CalculateColor();
}

IHyDraw2d::~IHyDraw2d()
{
	delete m_pScissor;
}

const IHyDraw2d &IHyDraw2d::operator=(const IHyDraw2d &rhs)
{
	IHyNode2d::operator=(rhs);

	m_fAlpha = rhs.m_fAlpha;

	delete m_pScissor;
	m_pScissor = nullptr;
	if(rhs.m_pScissor)
	{
		m_pScissor = HY_NEW ScissorRect();
		m_pScissor->m_LocalScissorRect = rhs.m_pScissor->m_LocalScissorRect;
		GetWorldScissor(m_pScissor->m_WorldScissorRect);
	}

	m_hStencil = rhs.m_hStencil;
	m_iCoordinateSystem = rhs.m_iCoordinateSystem;
	m_iDisplayOrder = rhs.m_iDisplayOrder;

	topColor.Set(rhs.topColor.Get());
	botColor.Set(rhs.botColor.Get());
	alpha.Set(rhs.alpha.Get());

	CalculateColor();

	return *this;
}

void IHyDraw2d::SetTint(float fR, float fG, float fB)
{
	topColor.Set(fR, fG, fB);
	botColor.Set(fR, fG, fB);
}

void IHyDraw2d::SetTint(uint32 uiColor)
{
	SetTint(((uiColor >> 16) & 0xFF) / 255.0f,
			((uiColor >> 8) & 0xFF) / 255.0f,
			(uiColor & 0xFF) / 255.0f);
}

float IHyDraw2d::CalculateAlpha()
{
	CalculateColor();
	return m_fCachedAlpha;
}

const glm::vec3 &IHyDraw2d::CalculateTopTint()
{
	CalculateColor();
	return m_CachedTopColor;
}

const glm::vec3 &IHyDraw2d::CalculateBotTint()
{
	CalculateColor();
	return m_CachedBotColor;
}

bool IHyDraw2d::IsScissorSet() const
{
	return m_pScissor != nullptr;
}

void IHyDraw2d::GetLocalScissor(HyScreenRect<int32> &scissorOut) const
{
	if(m_pScissor == nullptr)
		return;

	scissorOut = m_pScissor->m_LocalScissorRect;
}

void IHyDraw2d::GetWorldScissor(HyScreenRect<int32> &scissorOut)
{
	if(m_pScissor == nullptr)
		return;

	if(IsDirty(DIRTY_Scissor))
	{
		if((m_uiExplicitFlags & EXPLICIT_Scissor) == 0 && m_pParent)
			m_pParent->GetWorldScissor(m_pScissor->m_WorldScissorRect);
		else
		{
			if(m_pScissor->m_LocalScissorRect.iTag == SCISSORTAG_Enabled)
			{
				glm::mat4 mtx;
				GetWorldTransform(mtx);

				m_pScissor->m_WorldScissorRect.x = static_cast<int32>(mtx[3].x + m_pScissor->m_LocalScissorRect.x);
				m_pScissor->m_WorldScissorRect.y = static_cast<int32>(mtx[3].y + m_pScissor->m_LocalScissorRect.y);
				m_pScissor->m_WorldScissorRect.width = static_cast<uint32>(mtx[0].x * m_pScissor->m_LocalScissorRect.width);
				m_pScissor->m_WorldScissorRect.height = static_cast<uint32>(mtx[1].y * m_pScissor->m_LocalScissorRect.height);
				m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Enabled;
			}
			else
			{
				m_pScissor->m_WorldScissorRect.iTag = SCISSORTAG_Disabled;
			}
		}

		ClearDirty(DIRTY_Scissor);
	}

	scissorOut = m_pScissor->m_WorldScissorRect;
}

bool IHyDraw2d::IsStencilSet() const
{
	return m_hStencil != HY_UNUSED_HANDLE;
}

HyStencil *IHyDraw2d::GetStencil() const
{
	return IHyRenderer::FindStencil(m_hStencil);
}

int32 IHyDraw2d::GetCoordinateSystem() const
{
	return m_iCoordinateSystem;
}

int32 IHyDraw2d::GetDisplayOrder() const
{
	return m_iDisplayOrder;
}

void IHyDraw2d::CalculateColor()
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
