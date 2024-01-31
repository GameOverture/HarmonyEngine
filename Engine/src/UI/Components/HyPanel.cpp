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
#include "Diagnostics/Console/IHyConsole.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h"

// Constructs a 'BoundingVolume' panel with 0 width/height
HyPanelInit::HyPanelInit() :
	m_ePanelType(PANELTYPE_BoundingVolume),
	m_uiWidth(0),
	m_uiHeight(0),
	m_eBodyType(HYTYPE_Unknown),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0, 0, 0, 0)),
	m_FrameColor(HyColor(0, 0, 0, 0)),
	m_TertiaryColor(HyColor(0, 0, 0, 0))
{
}

// Constructs a 'BoundingVolume' panel
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight) :
	m_ePanelType(PANELTYPE_BoundingVolume),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_eBodyType(HYTYPE_Unknown),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0, 0, 0, 0)),
	m_FrameColor(HyColor(0, 0, 0, 0)),
	m_TertiaryColor(HyColor(0, 0, 0, 0))
{
}

// Constructs a 'HyBody' panel
HyPanelInit::HyPanelInit(HyType eBodyType, const HyNodePath &nodePath) :
	m_ePanelType(PANELTYPE_HyBody),
	m_eBodyType(eBodyType),
	m_HyBodyPath(nodePath),
	m_uiWidth(0), // TBD by loading the sprite
	m_uiHeight(0),// TBD by loading the sprite
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0, 0, 0, 0)),
	m_FrameColor(HyColor(0, 0, 0, 0)),
	m_TertiaryColor(HyColor(0, 0, 0, 0))
{
}

// Constructs a 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor /*= HyColor(0,0,0,0)*/, HyColor frameColor /*= HyColor(0,0,0,0)*/, HyColor tertiaryColor /*= HyColor(0, 0, 0, 0)*/) :
	m_ePanelType(PANELTYPE_Primitive),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_uiFrameSize(uiFrameSize),
	m_PanelColor(panelColor),
	m_FrameColor(frameColor),
	m_TertiaryColor(tertiaryColor)
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyPanel::HyPanel(const HyPanelInit &initRef, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_ePanelType(HyPanelInit::PANELTYPE_Invalid),
	m_pHyBody(nullptr),
	m_pPrimitive(nullptr),
	size(*this, DIRTY_Size | DIRTY_SceneAABB)
{
	Setup(initRef);
}

/*virtual*/ HyPanel::~HyPanel()
{
	ParentDetach(); // This avoids a crash when SetEntityLoaded() propagates to parents and invokes this->IsValid() after being deleted
}

void HyPanel::Setup(const HyPanelInit &initRef)
{
	m_pHyBody;
	m_pPrimitive;
	size;
	
	m_ePanelType = initRef.m_ePanelType;
	switch(m_ePanelType)
	{
	case HyPanelInit::PANELTYPE_BoundingVolume:
		delete m_pHyBody;
		m_pHyBody = nullptr;
		delete m_pPrimitive;
		m_pPrimitive = nullptr;
		size.Set(static_cast<int32>(initRef.m_uiWidth), static_cast<int32>(initRef.m_uiHeight));
		break;

	case HyPanelInit::PANELTYPE_HyBody:
		if(false == (m_pHyBody && m_pHyBody->GetType() == initRef.m_eBodyType && m_pHyBody->GetPath() == initRef.m_HyBodyPath))
		{
			delete m_pHyBody;

			switch(initRef.m_eBodyType)
			{
			case HYTYPE_Sprite:
				m_pHyBody = HY_NEW HySprite2d(initRef.m_HyBodyPath, this);
				break;
			case HYTYPE_Spine:
				m_pHyBody = HY_NEW HySpine2d(initRef.m_HyBodyPath, this);
				break;
			case HYTYPE_TexturedQuad:
				m_pHyBody = HY_NEW HyTexturedQuad2d(initRef.m_HyBodyPath, this);
				break;

			default:
				HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << initRef.m_eBodyType);
				break;
			}
			m_pHyBody = HY_NEW 
		}

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

bool HyPanel::IsHyBody() const
{
	return m_ePanelType == HyPanelInit::PANELTYPE_HyBody;
}

bool HyPanel::IsPrimitive() const
{
	return m_ePanelType == HyPanelInit::PANELTYPE_Primitive;
}

void HyPanel::ApplyPanelState(HyPanelState ePanelState)
{
	if(uiStateIndex < m_SpritePanel.GetNumStates())
		m_SpritePanel.SetState(uiStateIndex);

	if(IsSprite())
		size.Set(m_SpritePanel.GetStateWidth(m_SpritePanel.GetState()), m_SpritePanel.GetStateHeight(m_SpritePanel.GetState()));
}

IHyBody2d *HyPanel::GetHyBody()
{
	return m_pHyBody;
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

uint32 HyPanel::GetFrameStrokeSize() const
{
	return m_uiFrameSize;
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(m_SpritePanel.IsLoadDataValid())
	{
		glm::vec2 vPanelDimensions = size.Get();

		const HySpriteData *pPanelData = static_cast<const HySpriteData *>(m_SpritePanel.AcquireData());
		const HySpriteFrame &frameRef = pPanelData->GetFrame(m_SpritePanel.GetState(), m_SpritePanel.GetFrame());

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

/*virtual*/ void HyPanel::OnUpdate() /*override*/
{
	if(IsDirty(IHyNode::DIRTY_Size))
	{
		if(IsPrimitive())
			ConstructPrimitives();
		else if(IsSprite())
		{
			auto vUiSizeHint = GetSizeHint();
			scale.X(size.X() / vUiSizeHint.x);
			scale.Y(size.Y() / vUiSizeHint.y);
		}

		ClearDirty(IHyNode::DIRTY_Size);
	}
}

void HyPanel::ConstructPrimitives()
{
	m_Panel.SetAsBox(size.X() - (m_uiFrameSize * 2), size.Y() - (m_uiFrameSize * 2));
	m_Panel.pos.Set(static_cast<int32>(m_uiFrameSize), static_cast<int32>(m_uiFrameSize));

	if(m_uiFrameSize > 0)
	{
		m_Frame1.SetAsBox(size.X(), size.Y());

		uint32 uiHalfFrameSize = m_uiFrameSize / 3;
		if(uiHalfFrameSize > 0)
		{
			m_Frame2.SetAsBox(size.X() - (m_uiFrameSize * 2) + (uiHalfFrameSize * 2), size.Y() - (m_uiFrameSize * 2) + (uiHalfFrameSize * 2));
			m_Frame2.pos.Set(static_cast<int32>(m_uiFrameSize - uiHalfFrameSize), static_cast<int32>(m_uiFrameSize - uiHalfFrameSize));
		}
		else
			m_Frame2.SetAsNothing();
	}
	else
	{
		m_Frame1.SetAsNothing();
		m_Frame2.SetAsNothing();
	}
}
