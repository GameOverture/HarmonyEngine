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
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF))
{
}

// Constructs a 'BoundingVolume' panel
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight) :
	m_eNodeType(HYTYPE_Unknown),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF))
{
}

// Constructs a 'NodeItem' panel
HyPanelInit::HyPanelInit(HyType eNodeType, const HyNodePath &nodePath) :
	m_eNodeType(eNodeType),
	m_uiWidth(0), // TBD by loading the node
	m_uiHeight(0),// TBD by loading the node
	m_NodePath(nodePath),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF))
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
	delete m_pPrimParts;
	m_pPrimParts = nullptr;

	delete m_pNodeItem;
	m_pNodeItem = nullptr;

	switch(initRef.m_eNodeType)
	{
	case HYTYPE_Unknown: // 'BoundingVolume' panel
		HySetVec(m_vSizeHint, initRef.m_uiWidth, initRef.m_uiHeight);
		m_vSizeActual = m_vSizeHint;
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
		m_pNodeItem = HY_NEW HySprite2d(initRef.m_NodePath, this);
		if(m_uiState != 0)
		{
			if(m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}

		HySetVec(m_vSizeHint, static_cast<HySprite2d *>(m_pNodeItem)->GetStateWidth(m_uiState, 1.0f), static_cast<HySprite2d *>(m_pNodeItem)->GetStateHeight(m_uiState, 1.0f));
		m_vSizeActual = m_vSizeHint;
		break;
	
	case HYTYPE_Spine: // 'NodeItem' panel
		m_pNodeItem = HY_NEW HySpine2d(initRef.m_NodePath, this);
		if(m_uiState != 0)
		{
			if(m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}

		HySetVec(m_vSizeHint, m_pNodeItem->GetWidth(1.0f), m_pNodeItem->GetHeight(1.0f));
		m_vSizeActual = m_vSizeHint;
		break;
	
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		m_pNodeItem = HY_NEW HyTexturedQuad2d(initRef.m_NodePath, this);

		HySetVec(m_vSizeHint, m_pNodeItem->GetWidth(1.0f), m_pNodeItem->GetHeight(1.0f));
		m_vSizeActual = m_vSizeHint;
		break;

	case HYTYPE_Entity: // 'Primitive' panel
		HySetVec(m_vSizeHint, initRef.m_uiWidth, initRef.m_uiHeight);
		m_vSizeActual = m_vSizeHint;

		m_pPrimParts = HY_NEW PrimParts(initRef, this);

		// If a PrimPart's color's RGBA is 0xDEADBEEF, then reassign it to a default color
		if(m_pPrimParts->m_PanelColor.GetAsRGBA() == 0xDEADBEEF)
			m_pPrimParts->m_PanelColor = m_pPrimParts->m_bIsContainer ? HyColor::PanelContainer : HyColor::PanelWidget;
		if(m_pPrimParts->m_FrameColor.GetAsRGBA() == 0xDEADBEEF)
			m_pPrimParts->m_FrameColor = m_pPrimParts->m_bIsContainer ? HyColor::FrameContainer : HyColor::FrameWidget;
		if(m_pPrimParts->m_TertiaryColor.GetAsRGBA() == 0xDEADBEEF)
			m_pPrimParts->m_TertiaryColor = HyColor::Orange;

		ConstructPrimitives();
		m_pPrimParts->m_Body.SetTint(m_pPrimParts->m_PanelColor);
		m_pPrimParts->m_Body.alpha.Set(m_pPrimParts->m_PanelColor.GetAlphaF());
		m_pPrimParts->m_Frame1.SetTint(m_pPrimParts->m_FrameColor);
		m_pPrimParts->m_Frame1.alpha.Set(m_pPrimParts->m_FrameColor.GetAlphaF());
		m_pPrimParts->m_Frame2.SetTint(m_pPrimParts->m_TertiaryColor);
		m_pPrimParts->m_Frame2.alpha.Set(m_pPrimParts->m_TertiaryColor.GetAlphaF());
		SetState(HYPANELSTATE_Idle);
		break;

	default:
		HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << initRef.m_eNodeType);
		break;
	}
}

HyPanelInit HyPanel::CloneInit()
{
	HyPanelInit init;
	if(IsBoundingVolume())
	{
		init.m_eNodeType = HYTYPE_Unknown;
	}
	else if(IsNode())
	{
		init.m_eNodeType = m_pNodeItem->GetType();
		init.m_NodePath = m_pNodeItem->GetPath();
	}
	else if(IsPrimitive())
	{
		init.m_eNodeType = HYTYPE_Entity;
		init.m_uiFrameSize = m_pPrimParts->m_uiFrameSize;
		init.m_PanelColor = m_pPrimParts->m_PanelColor;
		init.m_FrameColor = m_pPrimParts->m_FrameColor;
		init.m_TertiaryColor = m_pPrimParts->m_TertiaryColor;
	}

	init.m_uiWidth = static_cast<uint32>(m_vSizeActual.x);
	init.m_uiHeight = static_cast<uint32>(m_vSizeActual.y);

	return init;
}

/*virtual*/ bool HyPanel::SetState(uint32 uiStateIndex) /*override*/
{
	if(IsNode())
	{
		if(m_pNodeItem->SetState(uiStateIndex))
		{
			// Update size hint in case changing the state affected the size
			if(m_pNodeItem->GetType() == HYTYPE_Sprite)
				HySetVec(m_vSizeHint, static_cast<HySprite2d *>(m_pNodeItem)->GetStateWidth(m_pNodeItem->GetState(), 1.0f), static_cast<HySprite2d *>(m_pNodeItem)->GetStateHeight(m_pNodeItem->GetState(), 1.0f));
			else
				HySetVec(m_vSizeHint, m_pNodeItem->GetWidth(1.0f), m_pNodeItem->GetHeight(1.0f));

			m_uiState = uiStateIndex;
			return true;
		}

		return false;
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

		m_uiState = uiStateIndex;
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

/*virtual*/ float HyPanel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode())
		return m_pNodeItem->GetWidth(m_pNodeItem->scale.X()) * fPercent;
	
	return m_vSizeActual.x * fPercent;
}

/*virtual*/ float HyPanel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode())
		return m_pNodeItem->GetHeight(m_pNodeItem->scale.Y()) * fPercent;

	return m_vSizeActual.y * fPercent;
}

float HyPanel::GetSizeDimension(int32 iDimensionIndex, float fPercent /*= 1.0f*/)
{
	if(iDimensionIndex == HYORIENT_Horizontal)
		return GetWidth(fPercent);

	return GetHeight(fPercent);
}

bool HyPanel::IsAutoSize() const
{
	return m_vSizeHint.x <= 0.0f || m_vSizeHint.y <= 0.0f;
}

glm::ivec2 HyPanel::GetPanelSizeHint() const
{
	if(IsNode())
		return glm::ivec2(m_pNodeItem->GetWidth(1.0f), m_pNodeItem->GetHeight(1.0f));

	return m_vSizeHint;
}

void HyPanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	HySetVec(m_vSizeActual, uiWidth, uiHeight);

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
	m_vSizeActual[iDimensionIndex] = uiSize;
	SetSize(m_vSizeActual.x, m_vSizeActual.y);
}

bool HyPanel::IsBoundingVolume() const
{
	return m_pPrimParts == nullptr && m_pNodeItem == nullptr;
}

bool HyPanel::IsNode() const
{
	return m_pNodeItem != nullptr;
}

IHyBody2d *HyPanel::GetNode()
{
	return m_pNodeItem;
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
	if(m_vSizeActual.x <= 0.0f || m_vSizeActual.y <= 0.0f)
	{
		m_pPrimParts->m_Frame1.SetAsNothing();
		m_pPrimParts->m_Frame2.SetAsNothing();
		m_pPrimParts->m_Body.SetAsNothing();
		return;
	}

	if(m_pPrimParts->m_uiFrameSize > 0)
	{
		m_pPrimParts->m_Frame1.SetAsBox(static_cast<float>(m_vSizeActual.x), static_cast<float>(m_vSizeActual.y));

		if(m_pPrimParts->m_uiFrameSize > 1)
		{
			m_pPrimParts->m_Frame2.SetAsBox(m_vSizeActual.x - (m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f, m_vSizeActual.y - (m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f);
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

	m_pPrimParts->m_Body.SetAsBox(m_vSizeActual.x - (m_pPrimParts->m_uiFrameSize * 2.0f), m_vSizeActual.y - (m_pPrimParts->m_uiFrameSize * 2.0f));
	m_pPrimParts->m_Body.pos.Set(static_cast<int32>(m_pPrimParts->m_uiFrameSize), static_cast<int32>(m_pPrimParts->m_uiFrameSize));
}
