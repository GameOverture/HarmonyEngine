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
#include "Assets/Nodes/Objects/HySpriteData.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySpine2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyTexturedQuad2d.h"

// Constructs a 'BoundingVolume' panel with 0 width/height
HyPanelInit::HyPanelInit() :
	m_eNodeType(HYTYPE_Unknown),
	m_uiWidth(0),
	m_uiHeight(0),
	m_NodePath(),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0, 0, 0, 0)),
	m_FrameColor(HyColor(0, 0, 0, 0)),
	m_TertiaryColor(HyColor(0, 0, 0, 0))
{
}

// Constructs a 'BoundingVolume' panel
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight) :
	m_eNodeType(HYTYPE_Unknown),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0, 0, 0, 0)),
	m_FrameColor(HyColor(0, 0, 0, 0)),
	m_TertiaryColor(HyColor(0, 0, 0, 0))
{
}

// Constructs a 'NodeItem' panel
HyPanelInit::HyPanelInit(HyType eNodeType, const HyNodePath &nodePath) :
	m_eNodeType(eNodeType),
	m_uiWidth(0), // TBD by loading the sprite
	m_uiHeight(0),// TBD by loading the sprite
	m_NodePath(nodePath),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0, 0, 0, 0)),
	m_FrameColor(HyColor(0, 0, 0, 0)),
	m_TertiaryColor(HyColor(0, 0, 0, 0))
{
	HyAssert(m_eNodeType != HYTYPE_Entity, "HyPanelInit::HyPanelInit(eNodeType, nodePath) 'NodeItem' panels cannot be of type 'Entity'");
}

// Constructs an 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor /*= HyColor(0,0,0,0)*/, HyColor frameColor /*= HyColor(0,0,0,0)*/, HyColor tertiaryColor /*= HyColor(0, 0, 0, 0)*/) :
	m_eNodeType(HYTYPE_Entity),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(),
	m_uiFrameSize(uiFrameSize),
	m_PanelColor(panelColor),
	m_FrameColor(frameColor),
	m_TertiaryColor(tertiaryColor)
{ }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyPanel::HyPanel(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_pPrimParts(nullptr),
	m_pNodeItem(nullptr)
{
}

/*virtual*/ HyPanel::~HyPanel()
{
	delete m_pPrimParts;
	delete m_pNodeItem;
	//ParentDetach(); // This avoids a crash when SetEntityLoaded() propagates to parents and invokes this->IsValid() after being deleted
}

void HyPanel::Setup(const HyPanelInit &initRef)
{
	switch(initRef.m_eNodeType)
	{
	case HYTYPE_Unknown: // 'BoundingVolume' panel
		HySetVec(m_vSize, initRef.m_uiWidth, initRef.m_uiHeight);
		delete m_pPrimParts;
		m_pPrimParts = nullptr;
		delete m_pNodeItem;
		m_pNodeItem = nullptr;
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
		HySetVec(m_vSize, 0, 0);
		delete m_pPrimParts;
		m_pPrimParts = nullptr;
		m_pNodeItem = HY_NEW HySprite2d(initRef.m_NodePath, this);
		break;
	case HYTYPE_Spine: // 'NodeItem' panel
		HySetVec(m_vSize, 0, 0);
		delete m_pPrimParts;
		m_pPrimParts = nullptr;
		m_pNodeItem = HY_NEW HySpine2d(initRef.m_NodePath, this);
		break;
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		HySetVec(m_vSize, 0, 0);
		delete m_pPrimParts;
		m_pPrimParts = nullptr;
		m_pNodeItem = HY_NEW HyTexturedQuad2d(initRef.m_NodePath, this);
		break;

	case HYTYPE_Entity: // 'Primitive' panel
		HySetVec(m_vSize, initRef.m_uiWidth, initRef.m_uiHeight);
		m_pPrimParts = HY_NEW PrimParts(initRef, this);
		delete m_pNodeItem;
		m_pNodeItem = nullptr;

		// If a PrimPart's color's alpha is 0, then reassign it to a default color
		if(m_pPrimParts->m_PanelColor.GetAlpha() == 0)
			m_pPrimParts->m_PanelColor = m_pPrimParts->m_bIsContainer ? HyColor::ContainerPanel : HyColor::WidgetPanel;
		if(m_pPrimParts->m_FrameColor.GetAlpha() == 0)
			m_pPrimParts->m_FrameColor = m_pPrimParts->m_bIsContainer ? HyColor::ContainerFrame : HyColor::WidgetFrame;
		if(m_pPrimParts->m_TertiaryColor.GetAlpha() == 0)
			m_pPrimParts->m_TertiaryColor = HyColor::Orange;

		ConstructPrimitives();
		m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor);
		m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor);
		m_pPrimParts->m_Frame2.SetTint(m_pPrimParts->m_TertiaryColor);
		SetState(HYPANELSTATE_Idle);
		break;

	default:
		HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << initRef.m_eNodeType);
		break;
	}
}

/*virtual*/ bool HyPanel::SetState(uint32 uiStateIndex) /*override*/
{
	m_uiState = uiStateIndex;

	if(IsNode())
	{
		if(uiStateIndex < m_pNodeItem->GetNumStates())
			return m_pNodeItem->SetState(uiStateIndex);
	}
	else if(IsPrimitive())
	{
		switch(static_cast<HyPanelState>(uiStateIndex))
		{
		case HYPANELSTATE_Idle:
			m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor);
			m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor);
			m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Down:
			m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor.Darken());
			m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor);
			m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Hover:
			m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor.Lighten());
			m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor);
			m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Highlighted:
			m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor);
			m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor.Lighten());
			m_pPrimParts->m_Frame2.SetVisible(true);
			break;
		case HYPANELSTATE_HighlightedDown:
			m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor.Darken());
			m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor.Lighten());
			m_pPrimParts->m_Frame2.SetVisible(true);
			break;
		case HYPANELSTATE_HighlightedHover:
			m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor.Lighten());
			m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor.Lighten());
			m_pPrimParts->m_Frame2.SetVisible(true);
			break;

		default:
			HyError("HyPanel::SetState() - Unknown HYPANELSTATE: " << uiStateIndex);
			return false;
		}

		return true;
	}

	return false;
}

/*virtual*/ uint32 HyPanel::GetNumStates() /*override*/
{
	if(IsNode())
		return m_pNodeItem->GetNumStates();
	else if(IsPrimitive())
		return HYNUM_PANELSTATES;

	return 0;
}

//bool HyPanel::SetNodeState(uint32 uiStateIndex)
//{
//	if(IsNode())
//		return SetState(uiStateIndex);
//
//	return false;
//}

/*virtual*/ float HyPanel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode())
		return m_pNodeItem->GetWidth(m_pNodeItem->scale.X()) * fPercent;
	
	return m_vSize.x * fPercent;
}

/*virtual*/ float HyPanel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode())
		return m_pNodeItem->GetHeight(m_pNodeItem->scale.Y()) * fPercent;

	return m_vSize.y * fPercent;
}

float HyPanel::GetSizeDimension(int32 iDimensionIndex, float fPercent /*= 1.0f*/)
{
	if(iDimensionIndex == HYORIEN_Horizontal)
		return GetWidth(fPercent);

	return GetHeight(fPercent);
}

glm::ivec2 HyPanel::GetSizeHint() const
{
	if(IsNode())
		return glm::ivec2(m_pNodeItem->GetWidth(1.0f), m_pNodeItem->GetHeight(1.0f));

	return m_vSize;
}

void HyPanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	HySetVec(m_vSize, uiWidth, uiHeight);

	if(IsNode())
	{
		float fCurWidth = m_pNodeItem->GetWidth(m_pNodeItem->scale.X());
		float fCurHeight = m_pNodeItem->GetHeight(m_pNodeItem->scale.Y());

		m_pNodeItem->scale.Set(uiWidth / fCurWidth, uiHeight / fCurHeight);
	}
	else if(IsPrimitive())
		ConstructPrimitives();
}

void HyPanel::SetSizeDimension(int32 iDimensionIndex, uint32 uiSize)
{
	m_vSize[iDimensionIndex] = uiSize;
	SetSize(m_vSize.x, m_vSize.y);
}

bool HyPanel::IsValid()
{
	if(IsNode())
		return m_pNodeItem->IsLoadDataValid();
	else
		return m_vSize.x > 0.0f && m_vSize.y > 0.0f;
}

bool HyPanel::IsBoundingVolume() const
{
	return m_pPrimParts == nullptr && m_pNodeItem == nullptr;
}

bool HyPanel::IsNode() const
{
	return m_pNodeItem != nullptr;
}

bool HyPanel::IsPrimitive() const
{
	return m_pPrimParts != nullptr;
}

uint32 HyPanel::GetFrameStrokeSize() const
{
	if(IsPrimitive())
		return m_pPrimParts->m_uiFrameSize;
	else
		return 0;
}

HyColor HyPanel::GetPanelColor() const
{
	if(IsPrimitive())
		return HyColor(m_pPrimParts->m_Body.topColor.X(), m_pPrimParts->m_Body.topColor.Y(), m_pPrimParts->m_Body.topColor.Z());
	else
		return HyColor::Transparent;
}

HyColor HyPanel::GetFrameColor() const
{
	if(IsPrimitive())
		return HyColor(m_pPrimParts->m_Frame1.topColor.X(), m_pPrimParts->m_Frame1.topColor.Y(), m_pPrimParts->m_Frame1.topColor.Z());
	else
		return HyColor::Transparent;
}

HyColor HyPanel::GetTertiaryColor() const
{
	if(IsPrimitive())
		return m_pPrimParts->m_TertiaryColor;
	else
		return HyColor::Transparent;
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(IsNode() && m_pNodeItem->IsLoadDataValid() && m_pNodeItem->GetType() == HYTYPE_Sprite)
	{
		//glm::vec2 vPanelDimensions = size.Get();

		const HySpriteData *pPanelData = static_cast<const HySpriteData *>(m_pNodeItem->AcquireData());
		const HySpriteFrame &frameRef = pPanelData->GetFrame(m_pNodeItem->GetState(), static_cast<HySprite2d *>(m_pNodeItem)->GetFrame());

		return -glm::vec2(frameRef.vOFFSET.x * m_pNodeItem->scale.X(), frameRef.vOFFSET.y * m_pNodeItem->scale.Y());
		//return -glm::vec2(frameRef.vOFFSET.x * (vPanelDimensions.x / vUiSizeHint.x), frameRef.vOFFSET.y * (vPanelDimensions.y / vUiSizeHint.y));
	}
	return glm::vec2(0.0f, 0.0f);
}

void HyPanel::ConstructPrimitives()
{
	if(m_pPrimParts->m_uiFrameSize > 0)
	{
		m_pPrimParts->m_Frame1.SetAsBox(m_vSize.x, m_vSize.y);

		if(m_pPrimParts->m_uiFrameSize > 1)
		{
			m_pPrimParts->m_Frame2.SetAsBox(m_vSize.x - (m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f, m_vSize.y - (m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f);
			m_pPrimParts->m_Frame2.pos.Set(static_cast<int32>(m_pPrimParts->m_uiFrameSize) - 1.0f, static_cast<int32>(m_pPrimParts->m_uiFrameSize) - 1.0f);
		}
		else
			m_pPrimParts->m_Frame2.SetAsNothing();
	}
	else
	{
		m_pPrimParts->m_Frame1.SetAsNothing();
		m_pPrimParts->m_Frame2.SetAsNothing();
	}

	m_pPrimParts->m_Body.SetAsBox(m_vSize.x - (m_pPrimParts->m_uiFrameSize * 2.0f), m_vSize.y - (m_pPrimParts->m_uiFrameSize * 2.0f));
	m_pPrimParts->m_Body.pos.Set(static_cast<int32>(m_pPrimParts->m_uiFrameSize), static_cast<int32>(m_pPrimParts->m_uiFrameSize));
}
