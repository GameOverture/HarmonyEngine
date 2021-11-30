/**************************************************************************
*	HyPanel.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyPanel.h"

HyPanelInit::HyPanelInit() :
	m_uiWidth(0),
	m_uiHeight(0),
	m_uiBorderSize(0),
	m_BgColor(HyColor(0x252526)),
	m_BorderColor(HyColor(0x3F3F41)),
	m_ePanelType(PANELTYPE_Null)
{
}

HyPanelInit::HyPanelInit(std::string sSpritePrefix, std::string sSpriteName) :
	m_sSpritePrefix(sSpritePrefix),
	m_sSpriteName(sSpriteName),
	m_uiWidth(0),
	m_uiHeight(0),
	m_uiBorderSize(0),
	m_BgColor(HyColor(0x252526)),
	m_BorderColor(HyColor(0x3F3F41)),
	m_ePanelType(PANELTYPE_Sprite)
{
}

HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiBorderSize /*= 4*/, HyColor backgroundColor /*= HyColor(0x252526)*/, HyColor borderColor /*= HyColor(0x3F3F41)*/) :
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_uiBorderSize(uiBorderSize),
	m_BgColor(backgroundColor),
	m_BorderColor(borderColor),
	m_ePanelType(PANELTYPE_Primitive)
{ }

HyPanel::HyPanel(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_Stroke(this),
	m_Border(this),
	m_BG(this)
{
	Setup(HyPanelInit());
}

HyPanel::HyPanel(const HyPanelInit &initRef, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_Stroke(this),
	m_Border(this),
	m_BG(this)
{
	Setup(initRef);
}

/*virtual*/ HyPanel::~HyPanel()
{
}

void HyPanel::Setup(const HyPanelInit &initRef)
{
	m_ePanelType = initRef.m_ePanelType;

	switch(m_ePanelType)
	{
	case HyPanelInit::PANELTYPE_Null:
		SetAsNull();
		break;

	case HyPanelInit::PANELTYPE_Sprite:
		m_SpritePanel.Init(initRef.m_sSpritePrefix, initRef.m_sSpriteName, this);
		if(m_SpritePanel.IsLoadDataValid() == false)
			SetAsNull();
		break;

	case HyPanelInit::PANELTYPE_Primitive:
		m_SpritePanel.Uninit();
		
		HySetVec(m_vSize, initRef.m_uiWidth, initRef.m_uiHeight);
		m_uiBorderSize = initRef.m_uiBorderSize;
		ConstructPrimitives();
		SetBgColor(initRef.m_BgColor);
		SetBorderColor(initRef.m_BorderColor);
		break;
	}
}

void HyPanel::SetAsNull()
{
	m_SpritePanel.Uninit();

	m_Border.SetAsNothing();
	m_BG.SetAsNothing();
	m_Stroke.SetAsNothing();

	m_ePanelType = HyPanelInit::PANELTYPE_Null;
}

bool HyPanel::IsValid()
{
	return m_ePanelType != HyPanelInit::PANELTYPE_Null;
}

bool HyPanel::IsPrimitive() const
{
	return m_ePanelType == HyPanelInit::PANELTYPE_Primitive;
}

bool HyPanel::IsSprite()
{
	return m_ePanelType == HyPanelInit::PANELTYPE_Sprite;
}

HySprite2d &HyPanel::GetSprite()
{
	return m_SpritePanel;
}

uint32 HyPanel::GetSpriteState() const
{
	return m_SpritePanel.GetState();
}

void HyPanel::SetSpriteState(uint32 uiStateIndex)
{
	m_SpritePanel.SetState(uiStateIndex);
}

glm::ivec2 HyPanel::GetSizeHint()
{
	if(IsPrimitive())
		return GetSize();
	else if(IsSprite())
	{
		return glm::ivec2(m_SpritePanel.GetStateMaxWidth(m_SpritePanel.GetState(), false),
						  m_SpritePanel.GetStateMaxHeight(m_SpritePanel.GetState(), false));
	}
	
	return m_vSize;
}

uint32 HyPanel::GetWidth()
{
	if(IsPrimitive())
		return m_BG.GetSceneWidth();
	else if(IsSprite())
		return m_SpritePanel.GetStateMaxWidth(m_SpritePanel.GetState(), true);
	else
		return m_vSize.x;
}

uint32 HyPanel::GetHeight()
{
	if(IsPrimitive())
		return m_BG.GetSceneHeight();
	else if(IsSprite())
		return m_SpritePanel.GetStateMaxHeight(m_SpritePanel.GetState(), true);
	else
		return m_vSize.y;
}

glm::ivec2 HyPanel::GetSize()
{
	return glm::ivec2(GetWidth(), GetHeight());
}

void HyPanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	HySetVec(m_vSize, uiWidth, uiHeight);

	if(IsPrimitive())
		ConstructPrimitives();
	else if(IsSprite())
	{
		auto vUiSizeHint = GetSizeHint();
		scale.X(static_cast<float>(uiWidth) / vUiSizeHint.x);
		scale.Y(static_cast<float>(uiHeight) / vUiSizeHint.y);
	}
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(IsPrimitive())
	{
		//HySetVec(vPanelOffset, m_pPrimPanel->GetBorderSize(), m_pPrimPanel->GetBorderSize());
		return glm::vec2(0.0f, 0.0f);
	}
	else if(m_SpritePanel.IsLoadDataValid())
	{
		glm::vec2 vPanelDimensions = GetSize();

		const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_SpritePanel.AcquireData());
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_SpritePanel.GetState(), m_SpritePanel.GetFrame());

		auto vUiSizeHint = GetSizeHint();
		return -glm::vec2(frameRef.vOFFSET.x * (vPanelDimensions.x / vUiSizeHint.x), frameRef.vOFFSET.y * (vPanelDimensions.y / vUiSizeHint.y));
	}

	return glm::vec2(0.0f, 0.0f);
}

HyColor HyPanel::GetBgColor() const
{
	return HyColor(m_BG.topColor.X(), m_BG.topColor.Y(), m_BG.topColor.Z());
}

void HyPanel::SetBgColor(HyColor color)
{
	m_BG.SetTint(color);
}

HyColor HyPanel::GetBorderColor() const
{
	return HyColor(m_Border.topColor.X(), m_Border.topColor.Y(), m_Border.topColor.Z());
}

void HyPanel::SetBorderColor(HyColor color)
{
	m_Border.SetTint(color);
	m_Stroke.SetTint(color.Lighten());
}

uint32 HyPanel::GetBorderSize() const
{
	return m_uiBorderSize;
}

void HyPanel::ConstructPrimitives()
{
	m_BG.SetAsBox(m_vSize.x, m_vSize.y);

	m_Stroke.SetWireframe(true);
	m_Stroke.SetAsBox(m_vSize.x + 1, m_vSize.y + 1);

	m_Border.SetAsBox(m_vSize.x + (m_uiBorderSize * 2), m_vSize.y + (m_uiBorderSize * 2));
	m_Border.pos.Set(-static_cast<int32>(m_uiBorderSize), -static_cast<int32>(m_uiBorderSize));
}
