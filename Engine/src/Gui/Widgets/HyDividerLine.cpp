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
	m_fLength(1.0f),
	m_fThickness(1.0f)
{
}

HyDividerLine::HyDividerLine(HyOrientation eOrientation, float fThickness, HyColor eColor, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Line(this),
	m_fLength(1.0f),
	m_fThickness(fThickness)
{
	m_Line.SetLayerColor(0, eColor);
	SetOrientation(eOrientation);
}

/*virtual*/ HyDividerLine::~HyDividerLine()
{
}

/*virtual*/ float HyDividerLine::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	return (GetOrientation() == HYORIENT_Horizontal ? m_fLength : m_fThickness) * fPercent;
}

/*virtual*/ float HyDividerLine::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	return (GetOrientation() == HYORIENT_Vertical ? m_fLength : m_fThickness) * fPercent;
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
	{
		m_Line.SetAsLineSegment(0, glm::vec2(-static_cast<float>(uiNewWidth), 0.0f), glm::vec2(uiNewWidth, 0.0f), uiNewHeight);
		m_fLength = static_cast<float>(uiNewWidth);
	}
	else
	{
		m_Line.SetAsLineSegment(0, glm::vec2(0.0f, -static_cast<float>(uiNewHeight)), glm::vec2(0.0f, uiNewHeight), uiNewWidth);
		m_fLength = static_cast<float>(uiNewHeight);
	}

	return glm::ivec2(uiNewWidth, uiNewHeight);
}
