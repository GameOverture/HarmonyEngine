/**************************************************************************
*	HyScrollContainer.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Containers/HyScrollContainer.h"
#include "HyEngine.h"

HyScrollContainer::HyScrollContainer(HyLayoutType eRootLayout, const HyPanelInit &initRef, uint32 uiScrollBarDiameter, bool bUseVert, bool bUseHorz, HyEntity2d *pParent /*= nullptr*/) :
	HyContainer(eRootLayout, initRef, pParent),
	m_bUseVertBar(bUseVert),
	m_bUseHorzBar(bUseHorz),
	m_VertBar(HYORIEN_Vertical, uiScrollBarDiameter, this),
	m_HorzBar(HYORIEN_Horizontal, uiScrollBarDiameter, this)
{
	m_VertBar.SetOnScrollCallback(OnScroll, this);
	m_HorzBar.SetOnScrollCallback(OnScroll, this);
	SetScrollBarColor(m_Panel.GetPanelColor());
	EnableScrollBars(m_bUseVertBar, m_bUseHorzBar);
	
	SetSize(initRef.m_uiWidth, initRef.m_uiHeight);
}

/*virtual*/ HyScrollContainer::~HyScrollContainer()
{
}

/*virtual*/ void HyScrollContainer::SetSize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
	HyContainer::SetSize(iNewWidth, iNewHeight);
}

void HyScrollContainer::EnableScrollBars(bool bUseVert, bool bUseHorz)
{
	m_bUseVertBar = bUseVert;
	m_bUseHorzBar = bUseHorz;

	// Use alpha because *this container may want to SetVisible() and it should then propagate to the scroll bars
	m_VertBar.alpha.Set(static_cast<int32>(m_bUseVertBar) * 1.0f);
	m_HorzBar.alpha.Set(static_cast<int32>(m_bUseHorzBar) * 1.0f);

	m_RootLayout.SetLayoutDirty();
}

void HyScrollContainer::SetScrollBarColor(HyColor color)
{
	m_VertBar.SetColor(color);
	m_HorzBar.SetColor(color);
}

void HyScrollContainer::SetLineScrollAmt(float fLineScrollAmt)
{
	m_VertBar.SetLineScrollAmt(fLineScrollAmt);
	m_HorzBar.SetLineScrollAmt(fLineScrollAmt);
}

/*virtual*/ void HyScrollContainer::OnContainerUpdate() /*override*/
{
	glm::ivec2 vScroll = HyEngine::Input().GetMouseScroll();
	m_VertBar.DoLineScroll(-vScroll.y);
	m_HorzBar.DoLineScroll(vScroll.x);
}

/*virtual*/ void HyScrollContainer::OnRootLayoutUpdate() /*override*/
{
	int32 iNewWidth = m_Panel.GetWidth();
	int32 iNewHeight = m_Panel.GetHeight();

	int32 iScissorMargin = 0;
	if(m_Panel.IsPrimitive())
		iScissorMargin = m_Panel.GetFrameSize();

	SetScissor(iScissorMargin, iScissorMargin,
		iNewWidth - (iScissorMargin * 2) - (static_cast<int32>(m_bUseVertBar) * m_VertBar.GetDiameter()),
		iNewHeight - (iScissorMargin * 2) - (static_cast<int32>(m_bUseHorzBar) * m_HorzBar.GetDiameter()));
	m_Panel.ClearScissor(false);
	m_VertBar.ClearScissor(false);
	m_HorzBar.ClearScissor(false);

	// If scrolling, then use '0' for that dimension, to indicate to the layout use the exact amount it needs.
	// NOTE: Using the layout's size hint (instead of '0') may be inaccurate if other dimension is being resized
	if(m_bUseHorzBar)
		iNewWidth = 0;
	else if(m_bUseVertBar)
	{
		iNewWidth -= (m_RootLayout.GetMargins().left + m_RootLayout.GetMargins().right);
		iNewWidth -= m_VertBar.GetDiameter();
	}

	if(m_bUseVertBar)
		iNewHeight = 0;
	else if(m_bUseHorzBar)
	{
		iNewHeight -= (m_RootLayout.GetMargins().top + m_RootLayout.GetMargins().bottom);
		iNewHeight -= m_HorzBar.GetDiameter();
	}

	glm::ivec2 vActualSize = m_RootLayout.Resize(iNewWidth, iNewHeight);

	if(m_bUseVertBar && m_bUseHorzBar == false)
	{
		m_VertBar.SetMetrics(GetSize().y - (iScissorMargin * 2), vActualSize.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_VertBar.GetDiameter() - iScissorMargin, iScissorMargin);
		m_VertBar.alpha.Set(1.0f);
		m_HorzBar.alpha.Set(0.0f);
	}
	else if(m_bUseVertBar == false && m_bUseHorzBar)
	{
		m_HorzBar.SetMetrics(GetSize().x - (iScissorMargin * 2), vActualSize.x, GetSize().x);
		m_HorzBar.pos.Set(iScissorMargin, iScissorMargin);
		m_HorzBar.alpha.Set(1.0f);
		m_VertBar.alpha.Set(0.0f);
	}
	else if(m_bUseVertBar && m_bUseHorzBar)
	{
		m_VertBar.SetMetrics(GetSize().y - (iScissorMargin * 2) - m_HorzBar.GetDiameter(), vActualSize.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_VertBar.GetDiameter() - iScissorMargin, iScissorMargin + static_cast<int32>(m_HorzBar.GetDiameter()));
		m_VertBar.alpha.Set(1.0f);

		m_HorzBar.SetMetrics(GetSize().x - (iScissorMargin * 2) - m_VertBar.GetDiameter(), vActualSize.x, GetSize().x);
		m_HorzBar.pos.Set(iScissorMargin, iScissorMargin);
		m_HorzBar.alpha.Set(1.0f);
	}
}

/*static*/ void HyScrollContainer::OnScroll(HyScrollBar *pSelf, float fNewPosition, float fTotalRange, void *pData)
{
	HyScrollContainer *pThis = static_cast<HyScrollContainer *>(pData);

	if(pSelf->GetOrientation() == HYORIEN_Vertical)
		pThis->m_RootLayout.pos.SetY(fNewPosition - fTotalRange);
	else
		pThis->m_RootLayout.pos.SetX(fNewPosition);
}
