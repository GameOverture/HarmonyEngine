/**************************************************************************
*	HyDividerLine.cpp
*
*	Harmony Engine
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyDividerLine.h"

HyDividerLine::HyDividerLine(HyEntity2d *pParent /*= nullptr*/)
{
}

HyDividerLine::HyDividerLine(HyOrientation eOrientation, float fThickness, HyColor eColor, HyEntity2d *pParent /*= nullptr*/)
{
	SetOrientation(eOrientation);
	if(GetOrientation() == HYORIENT_Horizontal)
		m_Panel.Setup(HyPanelInit(0, 1, fThickness, eColor, eColor));
	else
		m_Panel.Setup(HyPanelInit(1, 0, fThickness, eColor, eColor));

	m_SizePolicies[GetOrientation()] = HYSIZEPOLICY_Expanding;
}

/*virtual*/ HyDividerLine::~HyDividerLine()
{
}

HyOrientation HyDividerLine::GetOrientation() const
{
	return m_uiAttribs & DIVIDERLINEATTRIB_IsHorizontal ? HYORIENT_Horizontal : HYORIENT_Vertical;
}

void HyDividerLine::SetOrientation(HyOrientation eOrientation)
{
	if(eOrientation == HYORIENT_Horizontal)
		m_uiAttribs |= DIVIDERLINEATTRIB_IsHorizontal;
	else
		m_uiAttribs &= ~DIVIDERLINEATTRIB_IsHorizontal;
}

float HyDividerLine::GetThickness() const
{
	return m_Panel.GetFrameStrokeSize();
}

void HyDividerLine::SetThickness(float fThickness)
{
	//m_Panel.Set SetFrameStrokeSize(fThickness);
}

/*virtual*/ void HyDividerLine::OnAssemble() /*override*/
{
}

/*virtual*/ glm::vec2 HyDividerLine::GetPosOffset() /*override*/
{
	return glm::vec2(0.0f);
}

/*virtual*/ void HyDividerLine::OnSetSizeHint() /*override*/
{
	m_vSizeHint = m_Panel.GetPanelSizeHint();
}

/*virtual*/ glm::ivec2 HyDividerLine::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	m_Panel.SetSize(uiNewWidth, uiNewHeight);
	return glm::ivec2(uiNewWidth, uiNewHeight);
}
