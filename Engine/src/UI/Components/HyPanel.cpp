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
	m_eNodeType(HYTYPE_Entity),
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
	m_eNodeType(HYTYPE_Entity),
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
	HyAssert(m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive, "HyPanelInit::HyPanelInit(eNodeType, nodePath) 'NodeItem' panels cannot be of type 'Entity'");
}

HyPanelInit::HyPanelInit(HyType eNodeType, const HyNodePath &nodePath, uint32 uiWidth, uint32 uiHeight) :
	m_eNodeType(eNodeType),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(nodePath),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE, 0xEF))
{
	HyAssert(m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive, "HyPanelInit::HyPanelInit(eNodeType, nodePath, uint32 uiWidth, uint32 uiHeight) 'NodeItem' panels cannot be of type 'Entity'");
}

// Constructs an 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
HyPanelInit::HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor /*= HyColor(0,0,0,0)*/, HyColor frameColor /*= HyColor(0,0,0,0)*/, HyColor tertiaryColor /*= HyColor(0, 0, 0, 0)*/) :
	m_eNodeType(HYTYPE_Primitive),
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
	m_eNodeType(HYTYPE_Unknown),
	m_PanelData()
{
}

/*virtual*/ HyPanel::~HyPanel()
{
	DeleteData();
}

void HyPanel::Setup(const HyPanelInit &initRef)
{
	DeleteData(); // Delete any previous data
	
	HySetVec(m_vSizeHint, initRef.m_uiWidth, initRef.m_uiHeight);
	m_vSizeActual = m_vSizeHint;

	m_eNodeType = initRef.m_eNodeType;
	switch(m_eNodeType)
	{
	case HYTYPE_Entity: // 'BoundingVolume' panel
		if(IsAutoSize() == false)
		{
			m_PanelData.m_pBoundingVolumeShape = HY_NEW HyShape2d();
			m_PanelData.m_pBoundingVolumeShape->SetAsBox(m_vSizeHint.x, m_vSizeHint.y);
		}
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HySprite2d(initRef.m_NodePath, this);
		if(m_uiState != 0)
		{
			if(m_PanelData.m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}
		if(IsAutoSize())
			HySetVec(m_vSizeActual, static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_uiState, 1.0f), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_uiState, 1.0f));
		else
			SetSize(m_vSizeHint.x, m_vSizeHint.y);
		break;
	
	case HYTYPE_Spine: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HySpine2d(initRef.m_NodePath, this);
		if(m_uiState != 0)
		{
			if(m_PanelData.m_pNodeItem->SetState(m_uiState) == false)
				m_uiState = 0;
		}
		if(initRef.m_uiWidth != 0 && initRef.m_uiHeight != 0)
			m_PanelData.m_pNodeItem->scale.Set(initRef.m_uiWidth / m_PanelData.m_pNodeItem->GetWidth(), initRef.m_uiHeight / m_PanelData.m_pNodeItem->GetHeight());
		break;
	
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HyTexturedQuad2d(initRef.m_NodePath, this);
		if(initRef.m_uiWidth != 0 && initRef.m_uiHeight != 0)
			m_PanelData.m_pNodeItem->scale.Set(initRef.m_uiWidth / m_PanelData.m_pNodeItem->GetWidth(), initRef.m_uiHeight / m_PanelData.m_pNodeItem->GetHeight());
		break;

	case HYTYPE_Primitive: // 'Primitive' panel
		m_PanelData.m_pPrimParts = HY_NEW PrimParts(initRef, this);

		// If a PrimPart's color's RGBA is 0xDEADBEEF, then reassign it to a default color
		if(m_PanelData.m_pPrimParts->m_PanelColor.GetAsRGBA() == 0xDEADBEEF)
			m_PanelData.m_pPrimParts->m_PanelColor = m_PanelData.m_pPrimParts->m_bIsContainer ? HyColor::PanelContainer : HyColor::PanelWidget;
		if(m_PanelData.m_pPrimParts->m_FrameColor.GetAsRGBA() == 0xDEADBEEF)
			m_PanelData.m_pPrimParts->m_FrameColor = m_PanelData.m_pPrimParts->m_bIsContainer ? HyColor::FrameContainer : HyColor::FrameWidget;
		if(m_PanelData.m_pPrimParts->m_TertiaryColor.GetAsRGBA() == 0xDEADBEEF)
			m_PanelData.m_pPrimParts->m_TertiaryColor = HyColor::Orange;

		ConstructPrimitives();
		m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor);
		m_PanelData.m_pPrimParts->m_Body.alpha.Set(m_PanelData.m_pPrimParts->m_PanelColor.GetAlphaF());
		m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
		m_PanelData.m_pPrimParts->m_Frame1.alpha.Set(m_PanelData.m_pPrimParts->m_FrameColor.GetAlphaF());
		m_PanelData.m_pPrimParts->m_Frame2.SetTint(m_PanelData.m_pPrimParts->m_TertiaryColor);
		m_PanelData.m_pPrimParts->m_Frame2.alpha.Set(m_PanelData.m_pPrimParts->m_TertiaryColor.GetAlphaF());
		SetState(HYPANELSTATE_Idle);
		break;

	default:
		HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << m_eNodeType);
		break;
	}
}

HyPanelInit HyPanel::CloneInit()
{
	HyPanelInit init;
	if(IsBoundingVolume())
	{
		init.m_eNodeType = HYTYPE_Entity;
	}
	else if(IsNode())
	{
		init.m_eNodeType = m_PanelData.m_pNodeItem->GetType();
		init.m_NodePath = m_PanelData.m_pNodeItem->GetPath();
	}
	else if(IsPrimitive())
	{
		init.m_eNodeType = HYTYPE_Primitive;
		init.m_uiFrameSize = m_PanelData.m_pPrimParts->m_uiFrameSize;
		init.m_PanelColor = m_PanelData.m_pPrimParts->m_PanelColor;
		init.m_FrameColor = m_PanelData.m_pPrimParts->m_FrameColor;
		init.m_TertiaryColor = m_PanelData.m_pPrimParts->m_TertiaryColor;
	}

	init.m_uiWidth = static_cast<uint32>(m_vSizeActual.x);
	init.m_uiHeight = static_cast<uint32>(m_vSizeActual.y);

	return init;
}

/*virtual*/ bool HyPanel::SetState(uint32 uiStateIndex) /*override*/
{
	if(IsNode())
	{
		if(m_PanelData.m_pNodeItem->SetState(uiStateIndex))
		{
			if(IsAutoSize())
			{
				if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
					HySetVec(m_vSizeActual, static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_PanelData.m_pNodeItem->GetState(), 1.0f), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_PanelData.m_pNodeItem->GetState(), 1.0f));
				else
					HySetVec(m_vSizeActual, m_PanelData.m_pNodeItem->GetWidth(1.0f), m_PanelData.m_pNodeItem->GetHeight(1.0f));
			}
			else
				SetSize(m_vSizeHint.x, m_vSizeHint.y);

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
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor);
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Down:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Darken());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Hover:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(false);
			break;
		case HYPANELSTATE_Highlighted:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor);
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(true);
			break;
		case HYPANELSTATE_HighlightedDown:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Darken());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(true);
			break;
		case HYPANELSTATE_HighlightedHover:
			m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor.Lighten());
			m_PanelData.m_pPrimParts->m_Frame2.SetVisible(true);
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
		return m_PanelData.m_pNodeItem->GetNumStates();
	else if(IsPrimitive())
		return HYNUM_PANELSTATES;

	return 0;
}

/*virtual*/ float HyPanel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode() && IsAutoSize())
		return m_PanelData.m_pNodeItem->GetWidth(m_PanelData.m_pNodeItem->scale.X()) * fPercent;
	
	return m_vSizeActual.x * fPercent;
}

/*virtual*/ float HyPanel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(IsNode() && IsAutoSize())
		return m_PanelData.m_pNodeItem->GetHeight(m_PanelData.m_pNodeItem->scale.Y()) * fPercent;

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
	if(IsNode() && IsAutoSize())
		return glm::ivec2(m_PanelData.m_pNodeItem->GetWidth(1.0f), m_PanelData.m_pNodeItem->GetHeight(1.0f));

	return m_vSizeHint;
}

void HyPanel::SetSize(uint32 uiWidth, uint32 uiHeight)
{
	HySetVec(m_vSizeActual, uiWidth, uiHeight);

	if(IsNode())
	{
		if(IsAutoSize())
		{
			float fCurWidth = m_PanelData.m_pNodeItem->GetWidth(1.0f);
			float fCurHeight = m_PanelData.m_pNodeItem->GetHeight(1.0f);

			m_PanelData.m_pNodeItem->scale.Set(uiWidth / fCurWidth, uiHeight / fCurHeight);
		}
		else
		{
			m_PanelData.m_pNodeItem->scale.SetAll(1.0f);
			float fScale = std::min(uiWidth / m_PanelData.m_pNodeItem->GetWidth(), uiHeight / m_PanelData.m_pNodeItem->GetHeight());
			m_PanelData.m_pNodeItem->scale.SetAll(fScale);

			if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
			{
				glm::ivec2 vOffset = static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateOffset(m_PanelData.m_pNodeItem->GetState());
				//m_pNodeItem->pos.SetX(-vOffset.x + ((m_vSizeHint.x - m_pNodeItem->GetWidth(m_pNodeItem->scale.GetX())) * 0.5f));
				//		return glm::vec2(-(vOffset.x * m_pNodeItem->scale.GetX()), -(vOffset.y * m_pNodeItem->scale.GetY()));
				//		//m_pNodeItem->pos.Set(-vOffset);
			}
		}
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
	return m_eNodeType == HYTYPE_Entity;
}

HyShape2d *HyPanel::GetBvShape() const
{
	if(IsBoundingVolume())
		return m_PanelData.m_pBoundingVolumeShape;
	return nullptr;
}

bool HyPanel::IsNode() const
{
	return m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive && m_eNodeType != HYTYPE_Unknown;
}

IHyBody2d *HyPanel::GetNode()
{
	if(IsNode())
		return m_PanelData.m_pNodeItem;
	return nullptr;
}

bool HyPanel::IsPrimitive() const
{
	return m_eNodeType == HYTYPE_Primitive;
}

uint32 HyPanel::GetFrameStrokeSize() const
{
	if(IsPrimitive())
		return m_PanelData.m_pPrimParts->m_uiFrameSize;
	else
		return 0;
}

HyColor HyPanel::GetPanelColor() const
{
	if(IsPrimitive())
		return HyColor(m_PanelData.m_pPrimParts->m_Body.topColor.X(), m_PanelData.m_pPrimParts->m_Body.topColor.Y(), m_PanelData.m_pPrimParts->m_Body.topColor.Z());
	else
		return HyColor::Transparent;
}

HyColor HyPanel::GetFrameColor() const
{
	if(IsPrimitive())
		return HyColor(m_PanelData.m_pPrimParts->m_Frame1.topColor.X(), m_PanelData.m_pPrimParts->m_Frame1.topColor.Y(), m_PanelData.m_pPrimParts->m_Frame1.topColor.Z());
	else
		return HyColor::Transparent;
}

HyColor HyPanel::GetTertiaryColor() const
{
	if(IsPrimitive())
		return m_PanelData.m_pPrimParts->m_TertiaryColor;
	else
		return HyColor::Transparent;
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(IsNode() && m_PanelData.m_pNodeItem->IsLoadDataValid() && m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
	{
		//glm::vec2 vPanelDimensions = size.Get();

		const HySpriteData *pPanelData = static_cast<const HySpriteData *>(m_PanelData.m_pNodeItem->AcquireData());
		const HySpriteFrame *pFrameRef = pPanelData->GetFrame(m_PanelData.m_pNodeItem->GetState(), static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetFrame());

		return -glm::vec2(pFrameRef->vOFFSET.x * m_PanelData.m_pNodeItem->scale.X(), pFrameRef->vOFFSET.y * m_PanelData.m_pNodeItem->scale.Y());
		//return -glm::vec2(pFrameRef->vOFFSET.x * (vPanelDimensions.x / vUiSizeHint.x), pFrameRef->vOFFSET.y * (vPanelDimensions.y / vUiSizeHint.y));
	}
	return glm::vec2(0.0f, 0.0f);
}

void HyPanel::ConstructPrimitives()
{
	if(m_vSizeActual.x <= 0.0f || m_vSizeActual.y <= 0.0f)
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Frame2.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Body.SetAsNothing();
		return;
	}

	if(m_PanelData.m_pPrimParts->m_uiFrameSize > 0)
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsBox(static_cast<float>(m_vSizeActual.x), static_cast<float>(m_vSizeActual.y));

		if(m_PanelData.m_pPrimParts->m_uiFrameSize > 1)
		{
			m_PanelData.m_pPrimParts->m_Frame2.SetAsBox(m_vSizeActual.x - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f, m_vSizeActual.y - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f);
			m_PanelData.m_pPrimParts->m_Frame2.pos.Set(static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize) - 1.0f, static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize) - 1.0f);
		}
		else
			m_PanelData.m_pPrimParts->m_Frame2.SetAsNothing();
	}
	else
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Frame2.SetAsNothing();
	}

	m_PanelData.m_pPrimParts->m_Body.SetAsBox(m_vSizeActual.x - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f), m_vSizeActual.y - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f));
	m_PanelData.m_pPrimParts->m_Body.pos.Set(static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize), static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize));
}

void HyPanel::DeleteData()
{
	switch(m_eNodeType)
	{
	case HYTYPE_Unknown:
		break;

	case HYTYPE_Entity:
		delete m_PanelData.m_pBoundingVolumeShape;
		m_PanelData.m_pBoundingVolumeShape = nullptr;
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
	case HYTYPE_Spine: // 'NodeItem' panel
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		delete m_PanelData.m_pNodeItem;
		m_PanelData.m_pNodeItem = nullptr;
		break;

	case HYTYPE_Primitive: // 'Primitive' panel
		delete m_PanelData.m_pPrimParts;
		m_PanelData.m_pPrimParts = nullptr;
		break;

	default:
		HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << m_eNodeType);
		break;
	}
}
