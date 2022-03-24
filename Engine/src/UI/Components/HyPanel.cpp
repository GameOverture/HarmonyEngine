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
#include "UI/Components/HyPanel.h"

// Constructs a 'BoundingVolume' panel with 0 width/height
HyPanelInit::HyPanelInit() :
	m_uiWidth(0),
	m_uiHeight(0),
	m_uiFrameSize(0),
	m_PanelColor(HyColor()),
	m_FrameColor(HyColor()),
	m_ePanelType(PANELTYPE_BoundingVolume)
{
}

// Constructs a 'BoundingVolume' panel
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight) :
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_uiFrameSize(0),
	m_PanelColor(HyColor()),
	m_FrameColor(HyColor()),
	m_ePanelType(PANELTYPE_BoundingVolume)
{
}

// Constructs a 'Sprite' panel
HyPanelInit::HyPanelInit(std::string sSpritePrefix, std::string sSpriteName, uint32 uiFrameSize /*= 0*/) :
	m_sSpritePrefix(sSpritePrefix),
	m_sSpriteName(sSpriteName),
	m_uiWidth(0), // TBD by loading the sprite
	m_uiHeight(0),// TBD by loading the sprite
	m_uiFrameSize(uiFrameSize),
	m_PanelColor(HyColor()),
	m_FrameColor(HyColor()),
	m_ePanelType(PANELTYPE_Sprite)
{
}

// Constructs a 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor /*= HyColor(0,0,0,0)*/, HyColor frameColor /*= HyColor(0,0,0,0)*/) :
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_uiFrameSize(uiFrameSize),
	m_PanelColor(panelColor),
	m_FrameColor(frameColor),
	m_ePanelType(PANELTYPE_Primitive)
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyPanel::HyPanel(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_Panel(this),
	m_uiFrameSize(0),
	m_Frame1(this),
	m_Frame2(this),
	size(*this, DIRTY_Size | DIRTY_BoundingVolume | DIRTY_SceneAABB)
{
	Setup(HyPanelInit(), false);
}

HyPanel::HyPanel(const HyPanelInit &initRef, bool bIsContainer, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_bIsContainer(bIsContainer),
	m_Panel(this),
	m_uiFrameSize(0),
	m_Frame1(this),
	m_Frame2(this),
	size(*this, DIRTY_Size | DIRTY_BoundingVolume | DIRTY_SceneAABB)
{
	Setup(initRef, m_bIsContainer);
}

/*virtual*/ HyPanel::~HyPanel()
{
}

void HyPanel::Setup(const HyPanelInit &initRef, bool bIsContainer)
{
	m_bIsContainer = bIsContainer;

	m_SpritePanel.Uninit();
	m_Frame1.shape.SetAsNothing();
	m_Frame2.shape.SetAsNothing();
	m_Panel.shape.SetAsNothing();
	
	m_uiFrameSize = initRef.m_uiFrameSize;
	m_ePanelType = initRef.m_ePanelType;
	switch(m_ePanelType)
	{
	case HyPanelInit::PANELTYPE_BoundingVolume:
		size.Set(static_cast<int32>(initRef.m_uiWidth), static_cast<int32>(initRef.m_uiHeight));
		break;

	case HyPanelInit::PANELTYPE_Sprite:
		m_SpritePanel.Init(initRef.m_sSpritePrefix, initRef.m_sSpriteName, this);
		size.Set(m_SpritePanel.GetStateWidth(m_SpritePanel.GetState()), m_SpritePanel.GetStateHeight(m_SpritePanel.GetState()));
		break;

	case HyPanelInit::PANELTYPE_Primitive:
		size.Set(static_cast<int32>(initRef.m_uiWidth), static_cast<int32>(initRef.m_uiHeight));
		ConstructPrimitives();

		if(initRef.m_PanelColor.GetAlpha() == 0)
			SetPanelColor(m_bIsContainer ? HyColor::ContainerPanel : HyColor::WidgetPanel);
		else
			SetPanelColor(initRef.m_PanelColor);

		if(initRef.m_FrameColor.GetAlpha() == 0)
			SetFrameColor(m_bIsContainer ? HyColor::ContainerFrame : HyColor::WidgetFrame);
		else
			SetFrameColor(initRef.m_FrameColor);
		break;
	}
}

bool HyPanel::IsValid()
{
	if(m_ePanelType == HyPanelInit::PANELTYPE_Sprite)
		return m_SpritePanel.IsLoadDataValid();
	else
		return size.X() > 0.0f && size.Y() > 0.0f;
}

bool HyPanel::IsBoundingVolume() const
{
	return m_ePanelType == HyPanelInit::PANELTYPE_BoundingVolume;
}

bool HyPanel::IsPrimitive() const
{
	return m_ePanelType == HyPanelInit::PANELTYPE_Primitive;
}

bool HyPanel::IsSprite() const
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
	if(uiStateIndex < m_SpritePanel.GetNumStates())
		m_SpritePanel.SetState(uiStateIndex);
	
	if(IsSprite())
		size.Set(m_SpritePanel.GetStateWidth(m_SpritePanel.GetState()), m_SpritePanel.GetStateHeight(m_SpritePanel.GetState()));
}

glm::ivec2 HyPanel::GetSizeHint()
{
	if(IsSprite())
	{
		return glm::ivec2(m_SpritePanel.GetStateWidth(m_SpritePanel.GetState()),
						  m_SpritePanel.GetStateHeight(m_SpritePanel.GetState()));
	}
	
	return size.Get();
}

//uint32 HyPanel::GetWidth()
//{
//	if(IsSprite())
//		return m_SpritePanel.GetStateWidth(m_SpritePanel.GetState()) * scale.X();
//	else
//		return size.X() * scale.X();
//}
//
//uint32 HyPanel::GetHeight()
//{
//	if(IsSprite())
//		return m_SpritePanel.GetStateHeight(m_SpritePanel.GetState()) * scale.Y();
//	else
//		return m_vSize.y * scale.Y();
//}
//
//void HyPanel::SetSize(uint32 uiWidth, uint32 uiHeight)
//{
//	HySetVec(m_vSize, uiWidth, uiHeight);
//
//	if(IsPrimitive())
//		ConstructPrimitives();
//	else if(IsSprite())
//	{
//		auto vUiSizeHint = GetSizeHint();
//		scale.X(static_cast<float>(uiWidth) / vUiSizeHint.x);
//		scale.Y(static_cast<float>(uiHeight) / vUiSizeHint.y);
//	}
//}

uint32 HyPanel::GetFrameSize() const
{
	return m_uiFrameSize;
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(m_SpritePanel.IsLoadDataValid())
	{
		glm::vec2 vPanelDimensions = size.Get();

		const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_SpritePanel.AcquireData());
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_SpritePanel.GetState(), m_SpritePanel.GetFrame());

		auto vUiSizeHint = GetSizeHint();
		return -glm::vec2(frameRef.vOFFSET.x * (vPanelDimensions.x / vUiSizeHint.x), frameRef.vOFFSET.y * (vPanelDimensions.y / vUiSizeHint.y));
	}

	return glm::vec2(0.0f, 0.0f);
}

HyColor HyPanel::GetPanelColor() const
{
	return HyColor(m_Panel.topColor.X(), m_Panel.topColor.Y(), m_Panel.topColor.Z());
}

void HyPanel::SetPanelColor(HyColor color)
{
	m_Panel.SetTint(color);
}

HyColor HyPanel::GetFrameColor() const
{
	return HyColor(m_Frame1.topColor.X(), m_Frame1.topColor.Y(), m_Frame1.topColor.Z());
}

void HyPanel::SetFrameColor(HyColor color)
{
	m_Frame1.SetTint(color);
	if(color.IsDark())
		m_Frame2.SetTint(color.Lighten());
	else
		m_Frame2.SetTint(color.Darken());
}

/*virtual*/ void HyPanel::SetDirty(uint32 uiDirtyFlags) /*override*/
{
	if(uiDirtyFlags & IHyNode::DIRTY_Size)
	{
		if(IsPrimitive())
			ConstructPrimitives();
		else if(IsSprite())
		{
			auto vUiSizeHint = GetSizeHint();
			scale.X(size.X() / vUiSizeHint.x);
			scale.Y(size.Y() / vUiSizeHint.y);
		}
		uiDirtyFlags &= ~IHyNode::DIRTY_Size;
	}

	HyEntity2d::SetDirty(uiDirtyFlags);
}

void HyPanel::ConstructPrimitives()
{
	m_Panel.shape.SetAsBox(size.X() - (m_uiFrameSize * 2), size.Y() - (m_uiFrameSize * 2));
	m_Panel.pos.Set(static_cast<int32>(m_uiFrameSize), static_cast<int32>(m_uiFrameSize));

	if(m_uiFrameSize > 0)
	{
		m_Frame1.shape.SetAsBox(size.X(), size.Y());

		uint32 uiHalfFrameSize = m_uiFrameSize / 3;
		if(uiHalfFrameSize > 0)
		{
			m_Frame2.shape.SetAsBox(size.X() - (m_uiFrameSize * 2) + (uiHalfFrameSize * 2), size.Y() - (m_uiFrameSize * 2) + (uiHalfFrameSize * 2));
			m_Frame2.pos.Set(static_cast<int32>(m_uiFrameSize - uiHalfFrameSize), static_cast<int32>(m_uiFrameSize - uiHalfFrameSize));
		}
		else
			m_Frame2.shape.SetAsNothing();
	}
	else
	{
		m_Frame1.shape.SetAsNothing();
		m_Frame2.shape.SetAsNothing();
	}
}
