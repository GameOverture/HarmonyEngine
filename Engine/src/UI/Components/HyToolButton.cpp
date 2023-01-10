/**************************************************************************
*	HyCloseButton.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Components/HyToolButton.h"
#include "HyEngine.h"

HyToolButton::HyToolButton(HyToolButtonType eToolBtnType, uint32 uiDiameter, HyColor panelColor, HyColor frameColor, HyColor glyphColor, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent),
	m_uiDiameter(uiDiameter),
	m_CircleStroke(this),
	m_Circle(this),
	m_X1(this),
	m_X2(this)
{
	m_PanelColor = panelColor;
	m_FrameColor = frameColor;
	m_GlyphColor = glyphColor;

	m_CircleStroke.SetAsCircle((m_uiDiameter + 4) * 0.5f);
	m_Circle.SetAsCircle(m_uiDiameter * 0.5f);

	m_Circle.SetNumCircleSegments(20);
	m_CircleStroke.SetNumCircleSegments(20);

	int32 iXDiameter = static_cast<int32>(m_uiDiameter / 4);
	m_X1.SetAsLineSegment(glm::vec2(-iXDiameter, -iXDiameter), glm::vec2(iXDiameter, iXDiameter));
	m_X1.SetLineThickness(3);
	
	m_X2.SetAsLineSegment(glm::vec2(-iXDiameter, iXDiameter), glm::vec2(iXDiameter, -iXDiameter));
	m_X2.SetLineThickness(3);

	SetColor(m_PanelColor, m_FrameColor, m_GlyphColor);
}

/*virtual*/ HyToolButton::~HyToolButton()
{
}

void HyToolButton::SetColor(HyColor panelColor, HyColor frameColor, HyColor glyphColor)
{
	m_CircleStroke.SetTint(frameColor);
	m_Circle.SetTint(panelColor);

	m_X1.SetTint(glyphColor);
	m_X2.SetTint(glyphColor);
}

/*virtual*/ void HyToolButton::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, m_uiDiameter, m_uiDiameter);
}

/*virtual*/ glm::vec2 HyToolButton::GetPosOffset() /*override*/
{
	return glm::vec2(m_uiDiameter * 0.5f, m_uiDiameter * 0.5f);
}

/*virtual*/ glm::ivec2 HyToolButton::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	OnSetSizeHint();
	return m_vSizeHint;
}

/*virtual*/ void HyToolButton::OnBtnStateChange(HyButtonState eNewState) /*override*/
{
	switch(eNewState)
	{
	case HYBUTTONSTATE_Idle:
	case HYBUTTONSTATE_Highlighted:
		SetColor(m_PanelColor, m_FrameColor, m_GlyphColor);
		break;

	case HYBUTTONSTATE_Down:
	case HYBUTTONSTATE_HighlightedDown:
		SetColor(m_PanelColor.Darken(), m_FrameColor.Darken(), m_GlyphColor);
		break;

	case HYBUTTONSTATE_Hover:
	case HYBUTTONSTATE_HighlightedHover:
		SetColor(m_PanelColor.Lighten(), m_FrameColor.Lighten(), m_GlyphColor);
		break;
	}
}

