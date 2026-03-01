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
#include "Gui/Widgets/HyDividerLine.h"

HyDividerLine::HyDividerLine(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Line(this),
	m_fThickness(1.0f)
{
}

HyDividerLine::HyDividerLine(HyOrientation eOrientation, float fThickness, HyColor eColor, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Line(this),
	m_fThickness(fThickness)
{
	m_Line.SetLayerColor(0, eColor);
	SetOrientation(eOrientation);
}

/*virtual*/ HyDividerLine::~HyDividerLine()
{
}

HyOrientation HyDividerLine::GetOrientation() const
{
	return m_uiEntityAttribs & DIVIDERLINEATTRIB_IsHorizontal ? HYORIENT_Horizontal : HYORIENT_Vertical;
}

void HyDividerLine::SetOrientation(HyOrientation eOrientation)
{
	if(eOrientation == HYORIENT_Horizontal)
	{
		m_uiEntityAttribs |= DIVIDERLINEATTRIB_IsHorizontal;
		SetSizePolicy(HYSIZEPOLICY_Expanding, HYSIZEPOLICY_Fixed);
	}
	else
	{
		m_uiEntityAttribs &= ~DIVIDERLINEATTRIB_IsHorizontal;
		SetSizePolicy(HYSIZEPOLICY_Fixed, HYSIZEPOLICY_Expanding);
	}
}

float HyDividerLine::GetThickness() const
{
	return m_fThickness;
}

void HyDividerLine::SetThickness(float fThickness)
{
	m_fThickness = fThickness;
}

/*virtual*/ glm::ivec2 HyDividerLine::OnCalcPreferredSize() /*override*/
{
	if(GetOrientation() == HYORIENT_Horizontal)
		return glm::ivec2(static_cast<int32>(m_fThickness), static_cast<int32>(m_fThickness));
	else
		return glm::ivec2(static_cast<int32>(m_fThickness), static_cast<int32>(m_fThickness));
}

/*virtual*/ glm::ivec2 HyDividerLine::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	if(GetOrientation() == HYORIENT_Horizontal)
		m_Line.SetAsLineSegment(0, glm::vec2(0.0f), glm::vec2((float)uiNewWidth, 0.0f), uiNewHeight);
	else
		m_Line.SetAsLineSegment(0, glm::vec2(0.0f), glm::vec2(0.0f, (float)uiNewHeight), uiNewWidth);

	return glm::ivec2(uiNewWidth, uiNewHeight);
}
