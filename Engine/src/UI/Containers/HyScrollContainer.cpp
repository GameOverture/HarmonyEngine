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

HyScrollContainer::HyScrollContainer(HyLayoutType eRootLayout, HyEntity2d *pParent /*= nullptr*/) :
	HyContainer(eRootLayout, pParent),
	m_uiScrollFlags(USE_VERT),
	m_VertBar(HYORIEN_Vertical, 20, this),
	m_HorzBar(HYORIEN_Horizontal, 20, this)
{
	m_VertBar.SetVisible(m_uiScrollFlags & USE_VERT);
	m_HorzBar.SetVisible(m_uiScrollFlags & USE_HORZ);
}

HyScrollContainer::HyScrollContainer(HyLayoutType eRootLayout, int32 iWidth, int32 iHeight, int32 iStroke, uint32 uiScrollBarDiameter, HyEntity2d *pParent /*= nullptr*/) :
	HyContainer(eRootLayout, iWidth, iHeight, iStroke, pParent),
	m_uiScrollFlags(USE_VERT),
	m_uiScrollBarDiameter(uiScrollBarDiameter),
	m_VertBar(HYORIEN_Vertical, m_uiScrollBarDiameter, this),
	m_HorzBar(HYORIEN_Horizontal, m_uiScrollBarDiameter, this)
{
	m_VertBar.SetVisible(m_uiScrollFlags & USE_VERT);
	m_HorzBar.SetVisible(m_uiScrollFlags & USE_HORZ);

	SetSize(iWidth, iHeight);
}

/*virtual*/ HyScrollContainer::~HyScrollContainer()
{
}

/*virtual*/ void HyScrollContainer::OnSetLayoutItems()
{
	glm::ivec2 vSizeHint = m_pRootLayout->GetSizeHint();
	switch(m_uiScrollFlags)
	{
	case USE_VERT:
		m_VertBar.SetMetrics(GetSize().y, m_uiScrollBarDiameter, vSizeHint.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_uiScrollBarDiameter, 0);
		m_VertBar.SetVisible(m_VertBar.IsValidMetrics());
		m_HorzBar.SetVisible(false);
		break;

	case USE_HORZ:
		m_HorzBar.SetMetrics(GetSize().x, m_uiScrollBarDiameter, vSizeHint.x, GetSize().x);
		m_HorzBar.pos.Set(0, 0);
		m_HorzBar.SetVisible(true);
		m_VertBar.SetVisible(false);
		break;

	case USE_BOTH:
		m_VertBar.SetMetrics(GetSize().y - m_uiScrollBarDiameter, m_uiScrollBarDiameter, vSizeHint.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_uiScrollBarDiameter, static_cast<int32>(m_uiScrollBarDiameter));
		m_VertBar.SetVisible(true);

		m_HorzBar.SetMetrics(GetSize().x - m_uiScrollBarDiameter, m_uiScrollBarDiameter, vSizeHint.x, GetSize().x);
		m_HorzBar.pos.Set(0, 0);
		m_HorzBar.SetVisible(true);
		break;
	}

	//HyInternal_LayoutSetSize(*m_pRootLayout, vSizeHint.x, vSizeHint.y);
}
