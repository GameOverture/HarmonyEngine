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

HyPrimitivePanelInit::HyPrimitivePanelInit() :
	m_uiWidth(0),
	m_uiHeight(0),
	m_uiBorderSize(0),
	m_BgColor(HyColor(0x252526)),
	m_BorderColor(HyColor(0x3F3F41))
{
}

HyPrimitivePanelInit::HyPrimitivePanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiBorderSize /*= 4*/, HyColor backgroundColor /*= HyColor(0x252526)*/, HyColor borderColor /*= HyColor(0x3F3F41)*/) :
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_uiBorderSize(uiBorderSize),
	m_BgColor(backgroundColor),
	m_BorderColor(borderColor)
{ }

HyPrimitivePanel::HyPrimitivePanel(const HyPrimitivePanelInit &initRef, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_Stroke(this),
	m_Border(this),
	m_BG(this),
	m_Data(initRef)
{
	m_Stroke.SetWireframe(true);

	SetSize(m_Data.m_uiWidth, m_Data.m_uiHeight);
	SetBgColor(m_Data.m_BgColor);
	SetBorderColor(m_Data.m_BorderColor);

	// TODO: Refactor so Primitives should not require Load()
	Load();
}

/*virtual*/ HyPrimitivePanel::~HyPrimitivePanel()
{
}

uint32 HyPrimitivePanel::GetWidth() const
{
	return m_Data.m_uiWidth;
}

uint32 HyPrimitivePanel::GetHeight() const
{
	return m_Data.m_uiHeight;
}

HyColor HyPrimitivePanel::GetBgColor() const
{
	return HyColor(m_BG.topColor.X(), m_BG.topColor.Y(), m_BG.topColor.Z());
}

void HyPrimitivePanel::SetBgColor(HyColor color)
{
	m_BG.SetTint(color);
}

HyColor HyPrimitivePanel::GetBorderColor() const
{
	return HyColor(m_Border.topColor.X(), m_Border.topColor.Y(), m_Border.topColor.Z());
}

void HyPrimitivePanel::SetBorderColor(HyColor color)
{
	m_Border.SetTint(color);
	m_Stroke.SetTint(color.Lighten());
}

glm::ivec2 HyPrimitivePanel::GetSize() const
{
	return glm::ivec2(m_Data.m_uiWidth, m_Data.m_uiHeight);
}

void HyPrimitivePanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	m_Data.m_uiWidth = uiWidth;
	m_Data.m_uiHeight = uiHeight;

	m_BG.SetAsBox(m_Data.m_uiWidth, m_Data.m_uiHeight);
	m_Stroke.SetAsBox(m_Data.m_uiWidth, m_Data.m_uiHeight);

	m_Border.SetAsBox(m_Data.m_uiWidth + (m_Data.m_uiBorderSize * 2), m_Data.m_uiHeight + (m_Data.m_uiBorderSize * 2));
	m_Border.pos.Set(-static_cast<int32>(m_Data.m_uiBorderSize), -static_cast<int32>(m_Data.m_uiBorderSize));
}

uint32 HyPrimitivePanel::GetBorderSize() const
{
	return m_Data.m_uiBorderSize;
}

void HyPrimitivePanel::SetBorderSize(uint32 uiBorderSize)
{
	// Round all border sizes upwards to an even number
	if(uiBorderSize & 1)
		uiBorderSize++;

	m_Data.m_uiBorderSize = uiBorderSize;

	m_Border.SetAsBox(m_Data.m_uiWidth + (m_Data.m_uiBorderSize * 2), m_Data.m_uiHeight + (m_Data.m_uiBorderSize * 2));
	m_Border.pos.Set(m_Data.m_uiBorderSize * -0.5f, m_Data.m_uiBorderSize * -0.5f);
}
