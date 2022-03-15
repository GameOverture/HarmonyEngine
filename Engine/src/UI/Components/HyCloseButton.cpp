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
#include "UI/Components/HyCloseButton.h"
#include "HyEngine.h"

HyCloseButton::HyCloseButton(uint32 uiDiameter, HyColor panelColor, HyColor frameColor, HyEntity2d *pParent) :
	HyButton(pParent),
	m_uiDiameter(uiDiameter),
	m_CircleStroke(this),
	m_Circle(this),
	m_X1(this),
	m_X2(this)
{
	m_PanelColor = panelColor;
	m_FrameColor = frameColor;

	m_CircleStroke.shape.SetAsCircle((m_uiDiameter + 4) * 0.5f);
	m_Circle.shape.SetAsCircle(m_uiDiameter * 0.5f);

	m_Circle.SetNumCircleSegments(20);
	m_CircleStroke.SetNumCircleSegments(20);

	int32 iXDiameter = static_cast<int32>(m_uiDiameter / 4);
	m_X1.shape.SetAsLineSegment(glm::vec2(-iXDiameter, -iXDiameter), glm::vec2(iXDiameter, iXDiameter));
	m_X2.shape.SetAsLineSegment(glm::vec2(-iXDiameter, iXDiameter), glm::vec2(iXDiameter, -iXDiameter));

	SetColor(m_PanelColor, m_FrameColor);
}

void HyCloseButton::SetColor(HyColor panelColor, HyColor frameColor)
{
	m_CircleStroke.SetTint(frameColor);
	m_Circle.SetTint(panelColor);

	if(panelColor.IsLight())
	{
		m_X1.SetTint(panelColor.Darken());
		m_X2.SetTint(panelColor.Darken());
	}
	else
	{
		m_X1.SetTint(panelColor.Lighten());
		m_X2.SetTint(panelColor.Lighten());
	}
}

/*virtual*/ void HyCloseButton::OnUpdate() /*override*/
{
	if(HyEngine::Input().IsMouseBtnDown(HYMOUSE_Btn1) == false)
		SetColor(m_PanelColor, m_FrameColor);
}

/*virtual*/ void HyCloseButton::OnUiMouseDown() /*override*/
{
	HyButton::OnUiMouseDown();
	SetColor(m_PanelColor.Darken(), m_FrameColor.Darken());
}

/*virtual*/ glm::vec2 HyCloseButton::GetPosOffset() /*override*/
{
	return glm::vec2(m_uiDiameter * 0.5f, m_uiDiameter * 0.5f);
}

/*virtual*/ void HyCloseButton::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, m_uiDiameter, m_uiDiameter);
}

/*virtual*/ glm::ivec2 HyCloseButton::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	OnSetSizeHint();
	return m_vSizeHint;
}
