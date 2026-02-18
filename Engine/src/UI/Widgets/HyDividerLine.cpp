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

HyDividerLine::HyDividerLine(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Line(this)
{
}

HyDividerLine::HyDividerLine(HyOrientation eOrientation, float fThickness, HyColor eColor, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Line(this)
{
	m_Line.SetLineThickness(fThickness);
	m_Line.SetTint(eColor);
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
	return m_Line.GetLineThickness();
}

void HyDividerLine::SetThickness(float fThickness)
{
	//m_Panel.Set SetFrameStrokeSize(fThickness);
}

/*virtual*/ void HyDividerLine::OnAssemble() /*override*/
{
}

/*virtual*/ glm::vec2 HyDividerLine::GetBotLeftOffset() /*override*/
{
	return glm::vec2(0.0f);
}

/*virtual*/ glm::ivec2 HyDividerLine::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	if(GetOrientation() == HYORIENT_Horizontal)
	{
		m_Line.SetAsLineSegment(glm::vec2(0.0f), glm::vec2((float)uiNewWidth, 0.0f));
		m_Line.SetLineThickness(uiNewHeight);
	}
	else
	{
		m_Line.SetAsLineSegment(glm::vec2(0.0f), glm::vec2(0.0f, (float)uiNewHeight));
		m_Line.SetLineThickness(uiNewWidth);
	}

	return glm::ivec2(uiNewWidth, uiNewHeight);
}
