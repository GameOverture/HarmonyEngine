/**************************************************************************
*	HyPrimitivePanel.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyPrimitivePanel.h"

HyPrimitivePanel::HyPrimitivePanel(uint32 uiWidth, uint32 uiHeight, uint32 uiBorderSize, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_Stroke(this),
	m_Border(this),
	m_BG(this),
	m_vSize(uiWidth, uiHeight),
	m_uiBorderSize(uiBorderSize)
{
	m_Stroke.SetWireframe(true);

	SetSize(m_vSize.x, m_vSize.y);

	SetBgColor(0x252526);
	SetBorderColor(0x3F3F41);

	Load();
}

/*virtual*/ HyPrimitivePanel::~HyPrimitivePanel()
{
}

HyAnimVec3 &HyPrimitivePanel::BgColorTop()
{
	return m_BG.topColor;
}

HyAnimVec3 &HyPrimitivePanel::BgColorBot()
{
	return m_BG.botColor;
}

void HyPrimitivePanel::SetBgColor(float fR, float fG, float fB)
{
	m_BG.SetTint(fR, fG, fB);
}

void HyPrimitivePanel::SetBgColor(uint32 uiColor)
{
	m_BG.SetTint(uiColor);
}

HyAnimVec3 &HyPrimitivePanel::BorderColorTop()
{
	return m_Border.topColor;
}

HyAnimVec3 &HyPrimitivePanel::BorderColorBot()
{
	return m_Border.botColor;
}

// TODO: Stroke color
//void HyPrimitivePanel::SetBorderColor(float fR, float fG, float fB)
//{
//	m_Border.SetTint(fR, fG, fB);
//	//uint32 uiColorStroke = (uiColorFill & 0xfefefe) >> 1;
//}

void HyPrimitivePanel::SetBorderColor(uint32 uiColor)
{
	m_Border.SetTint(uiColor);
	m_Stroke.SetTint((uiColor & 0xfefefe) << 1);
}

glm::ivec2 HyPrimitivePanel::GetSize() const
{
	return m_vSize;
}

void HyPrimitivePanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	HySetVec(m_vSize, uiWidth, uiHeight);
	m_BG.SetAsBox(m_vSize.x, m_vSize.y);
	m_Stroke.SetAsBox(m_vSize.x, m_vSize.y);

	m_Border.SetAsBox(m_vSize.x + (m_uiBorderSize * 2), m_vSize.y + (m_uiBorderSize * 2));
	m_Border.pos.Set(-static_cast<int32>(m_uiBorderSize), -static_cast<int32>(m_uiBorderSize));
}

uint32 HyPrimitivePanel::GetBorderSize() const
{
	return m_uiBorderSize;
}

void HyPrimitivePanel::SetBorderSize(uint32 uiBorderSize)
{
	// Round all border sizes upwards to an even number
	if(uiBorderSize & 1)
		uiBorderSize++;

	m_uiBorderSize = uiBorderSize;

	m_Border.SetAsBox(m_vSize.x + (m_uiBorderSize * 2), m_vSize.y + (m_uiBorderSize * 2));
	m_Border.pos.Set(m_uiBorderSize * -0.5f, m_uiBorderSize * -0.5f);
}
