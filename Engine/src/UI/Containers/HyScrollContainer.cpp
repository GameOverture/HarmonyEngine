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

HyScrollContainer::HyScrollContainer(HyLayoutType eRootLayout, HyEntity2d *pParent /*= nullptr*/) :
	HyContainer(eRootLayout, pParent),
	m_uiScrollFlags(USE_VERT),
	m_VertBar(HYORIEN_Vertical, 20, this),
	m_HorzBar(HYORIEN_Horizontal, 20, this)
{
	if((m_uiScrollFlags & USE_VERT) == 0)
		m_VertBar.alpha.Set(0.0f);
	if((m_uiScrollFlags & USE_HORZ) == 0)
		m_HorzBar.alpha.Set(0.0f);
	m_VertBar.SetOnScrollCallback(OnScroll, this);
	m_HorzBar.SetOnScrollCallback(OnScroll, this);
}

HyScrollContainer::HyScrollContainer(HyLayoutType eRootLayout, const HyPrimitivePanelInit &initRef, uint32 uiScrollBarDiameter, HyEntity2d *pParent /*= nullptr*/) :
	HyContainer(eRootLayout, initRef, pParent),
	m_uiScrollFlags(USE_VERT),
	m_uiScrollBarDiameter(uiScrollBarDiameter),
	m_VertBar(HYORIEN_Vertical, m_uiScrollBarDiameter, this),
	m_HorzBar(HYORIEN_Horizontal, m_uiScrollBarDiameter, this)
{
	if((m_uiScrollFlags & USE_VERT) == 0)
		m_VertBar.alpha.Set(0.0f);
	if((m_uiScrollFlags & USE_HORZ) == 0)
		m_HorzBar.alpha.Set(0.0f);
	m_VertBar.SetOnScrollCallback(OnScroll, this);
	m_HorzBar.SetOnScrollCallback(OnScroll, this);

	SetSize(initRef.m_uiWidth, initRef.m_uiHeight);
	SetScrollBarColor(m_pPrimPanel->GetBgColor());
}

/*virtual*/ HyScrollContainer::~HyScrollContainer()
{
}

/*virtual*/ glm::ivec2 HyScrollContainer::GetSize() /*override*/
{
	return m_vShownSize;
}

/*virtual*/ void HyScrollContainer::SetSize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
	HySetVec(m_vShownSize, iNewWidth, iNewHeight);
	if(m_pPrimPanel)
		m_pPrimPanel->SetSize(m_vShownSize.x, m_vShownSize.y);

	SetScissor(0, 0, m_vShownSize.x - ((m_uiScrollFlags & USE_VERT) * m_VertBar.GetDiameter()), m_vShownSize.y - (((m_uiScrollFlags & USE_HORZ) >> 1) * m_HorzBar.GetDiameter()));
	m_pPrimPanel->ClearScissor(false);
	m_VertBar.ClearScissor(false);
	m_HorzBar.ClearScissor(false);

	// '0' indicates to the layout that it should use its sizehint for that dimension
	if(m_uiScrollFlags & USE_VERT)
		iNewHeight = 0;
	if(m_uiScrollFlags & USE_HORZ)
		iNewWidth = 0;

	if(iNewWidth != 0)
	{
		iNewWidth -= (m_pRootLayout->GetMargins().left + m_pRootLayout->GetMargins().right);
		if((m_uiScrollFlags & USE_VERT) != 0)
			iNewWidth -= m_VertBar.GetDiameter();
	}
	if(iNewHeight != 0)
	{
		iNewHeight -= (m_pRootLayout->GetMargins().top + m_pRootLayout->GetMargins().bottom);
		if((m_uiScrollFlags & USE_HORZ) != 0)
			iNewHeight -= m_HorzBar.GetDiameter();
	}

	HyInternal_LayoutSetSize(*m_pRootLayout, iNewWidth, iNewHeight);
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
	m_VertBar.DoLineScroll(vScroll.y);
	m_HorzBar.DoLineScroll(vScroll.x);
}

/*virtual*/ void HyScrollContainer::OnSetLayoutItems() /*override*/
{
	HyContainer::OnSetLayoutItems();

	glm::ivec2 vSizeHint = m_pRootLayout->GetSizeHint();

	switch(m_uiScrollFlags)
	{
	case USE_VERT:
		m_VertBar.SetMetrics(GetSize().y, m_uiScrollBarDiameter, vSizeHint.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_uiScrollBarDiameter, 0);
		m_VertBar.alpha.Set(1.0f);
		m_HorzBar.alpha.Set(0.0f);
		break;

	case USE_HORZ:
		m_HorzBar.SetMetrics(GetSize().x, m_uiScrollBarDiameter, vSizeHint.x, GetSize().x);
		m_HorzBar.pos.Set(0, 0);
		m_HorzBar.alpha.Set(1.0f);
		m_VertBar.alpha.Set(0.0f);
		break;

	case USE_BOTH:
		m_VertBar.SetMetrics(GetSize().y - m_uiScrollBarDiameter, m_uiScrollBarDiameter, vSizeHint.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_uiScrollBarDiameter, static_cast<int32>(m_uiScrollBarDiameter));
		m_VertBar.alpha.Set(1.0f);

		m_HorzBar.SetMetrics(GetSize().x - m_uiScrollBarDiameter, m_uiScrollBarDiameter, vSizeHint.x, GetSize().x);
		m_HorzBar.pos.Set(0, 0);
		m_HorzBar.alpha.Set(1.0f);
		break;
	}
}

/*static*/ void HyScrollContainer::OnScroll(HyScrollBar *pSelf, uint32 uiNewPosition, void *pData)
{
	HyScrollContainer *pThis = static_cast<HyScrollContainer *>(pData);

	if(pSelf->GetOrientation() == HYORIEN_Vertical)
		pThis->m_pRootLayout->pos.SetY(-static_cast<float>(uiNewPosition));
	else
		pThis->m_pRootLayout->pos.SetX(static_cast<float>(uiNewPosition));
}
