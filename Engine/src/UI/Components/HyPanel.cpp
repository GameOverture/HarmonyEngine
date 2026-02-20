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
HyUiPanelInit::HyUiPanelInit() :
	m_eNodeType(HYTYPE_Entity),
	m_uiWidth(0),
	m_uiHeight(0),
	m_NodePath(),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
}

// Constructs a 'BoundingVolume' panel
HyUiPanelInit::HyUiPanelInit(uint32 uiWidth, uint32 uiHeight) :
	m_eNodeType(HYTYPE_Entity),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
}

// Constructs a 'NodeItem' panel
HyUiPanelInit::HyUiPanelInit(HyType eNodeType, const HyNodePath &nodePath) :
	m_eNodeType(eNodeType),
	m_uiWidth(0),
	m_uiHeight(0),
	m_NodePath(nodePath),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
	HyAssert(m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive, "HyUiPanelInit::HyUiPanelInit(eNodeType, nodePath) 'NodeItem' panels cannot be of type 'Entity'");
}

HyUiPanelInit::HyUiPanelInit(HyType eNodeType, const HyNodePath &nodePath, uint32 uiWidth, uint32 uiHeight) :
	m_eNodeType(eNodeType),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(nodePath),
	m_uiFrameSize(0),
	m_PanelColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_FrameColor(HyColor(0xDE, 0xAD, 0xBE)),
	m_TertiaryColor(HyColor(0xDE, 0xAD, 0xBE))
{
	HyAssert(m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive, "HyPanelInit::HyUiPanelInit(eNodeType, nodePath, uint32 uiWidth, uint32 uiHeight) 'NodeItem' panels cannot be of type 'Entity'");
}

// Constructs an 'Primitive' panel. Colors of HyColor(0,0,0,0) will be set to a default color determined by the panel's usage
HyUiPanelInit::HyUiPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor /*= HyColor(0,0,0,0)*/, HyColor frameColor /*= HyColor(0,0,0,0)*/, HyColor tertiaryColor /*= HyColor(0, 0, 0, 0)*/) :
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
HyPanel::HyPanel() :
	m_eNodeType(HYTYPE_Unknown),
	m_PanelData()
{
}

HyPanel::HyPanel(const HyUiPanelInit &initRef, HyEntity2d *pParent) :
	m_eNodeType(HYTYPE_Unknown),
	m_PanelData()
{
	Setup(initRef, pParent);
}

HyPanel::~HyPanel()
{
	DeleteData();
}

void HyPanel::Setup(const HyUiPanelInit &initRef, HyEntity2d *pParent)
{
	DeleteData(); // Delete any previous data

	m_eNodeType = initRef.m_eNodeType;
	switch(m_eNodeType)
	{
	case HYTYPE_Entity: // 'BoundingVolume' panel (aka no visual panel)
		m_PanelData.m_BoundingVolumeSize = glm::vec2(static_cast<float>(initRef.m_uiWidth), static_cast<float>(initRef.m_uiHeight));
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HySprite2d(initRef.m_NodePath, pParent);
		static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->SetAllBoundsIncludeAlphaCrop(true);
		break;
	case HYTYPE_Spine: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HySpine2d(initRef.m_NodePath, pParent);
		break;
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		m_PanelData.m_pNodeItem = HY_NEW HyTexturedQuad2d(initRef.m_NodePath, pParent);
		break;

	case HYTYPE_Primitive: // 'Primitive' panel
		m_PanelData.m_pPrimParts = HY_NEW PrimParts(initRef, pParent);

		// If a PrimPart's color's RGBA is 0xDEADBEEF, then reassign it to a default color
		if(m_PanelData.m_pPrimParts->m_PanelColor == HyColor::_InternalUse)
			m_PanelData.m_pPrimParts->m_PanelColor = m_PanelData.m_pPrimParts->m_bIsContainer ? HyColor::PanelContainer : HyColor::PanelWidget;
		if(m_PanelData.m_pPrimParts->m_FrameColor == HyColor::_InternalUse)
			m_PanelData.m_pPrimParts->m_FrameColor = m_PanelData.m_pPrimParts->m_bIsContainer ? HyColor::FrameContainer : HyColor::FrameWidget;
		if(m_PanelData.m_pPrimParts->m_TertiaryColor == HyColor::_InternalUse)
			m_PanelData.m_pPrimParts->m_TertiaryColor = HyColor::Orange;

		m_PanelData.m_pPrimParts->m_Body.SetTint(m_PanelData.m_pPrimParts->m_PanelColor);
		m_PanelData.m_pPrimParts->m_Frame1.SetTint(m_PanelData.m_pPrimParts->m_FrameColor);
		m_PanelData.m_pPrimParts->m_Frame2.SetTint(m_PanelData.m_pPrimParts->m_TertiaryColor);
		SetState(HYPANELSTATE_Idle);
		break;

	default:
		HyLogError("HyPanel::Setup() - Unhandled m_eNodeType: " << m_eNodeType);
		break;
	}

	SetSize(initRef.m_uiWidth, initRef.m_uiHeight);
}

float HyPanel::GetWidth(float fPercent /*= 1.0f*/) const
{
	switch(m_eNodeType)
	{
	case HYTYPE_Entity: // 'BoundingVolume' panel (aka no visual panel)
		return m_PanelData.m_BoundingVolumeSize.x * fPercent;

	case HYTYPE_Sprite: // 'NodeItem' panel
	case HYTYPE_Spine: // 'NodeItem' panel
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
			return static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateWidth(m_PanelData.m_pNodeItem->GetState(), fPercent);
		else
			return m_PanelData.m_pNodeItem->GetWidth(fPercent);

	case HYTYPE_Primitive: // 'Primitive' panel
		if(m_PanelData.m_pPrimParts->m_Body.GetShapeType() == HYFIXTURE_Nothing)
			return 0.0f;
		return (m_PanelData.m_pPrimParts->m_Body.GetWidth() + (m_PanelData.m_pPrimParts->m_uiFrameSize * 2)) * fPercent;

	default:
		HyLogError("HyPanel::GetWidth() - Unhandled m_eNodeType: " << m_eNodeType);
		break;
	}

	return 0.0f;
}

float HyPanel::GetHeight(float fPercent /*= 1.0f*/) const
{
	switch(m_eNodeType)
	{
	case HYTYPE_Entity: // 'BoundingVolume' panel (aka no visual panel)
		return m_PanelData.m_BoundingVolumeSize.y * fPercent;

	case HYTYPE_Sprite: // 'NodeItem' panel
	case HYTYPE_Spine: // 'NodeItem' panel
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		if(m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
			return static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateHeight(m_PanelData.m_pNodeItem->GetState(), fPercent);
		else
			return m_PanelData.m_pNodeItem->GetHeight(fPercent);

	case HYTYPE_Primitive: // 'Primitive' panel
		if(m_PanelData.m_pPrimParts->m_Body.GetShapeType() == HYFIXTURE_Nothing)
			return 0.0f;
		return (m_PanelData.m_pPrimParts->m_Body.GetHeight() + (m_PanelData.m_pPrimParts->m_uiFrameSize * 2)) * fPercent;

	default:
		HyLogError("HyPanel::GetHeight() - Unhandled m_eNodeType: " << m_eNodeType);
		break;
	}
	
	return 0.0f;
}

glm::vec2 HyPanel::GetSize() const
{
	return glm::vec2(GetWidth(), GetHeight());
}

void HyPanel::SetSize(float fWidth, float fHeight)
{
	switch(m_eNodeType)
	{
	case HYTYPE_Entity: // 'BoundingVolume' panel (aka no visual panel)
		m_PanelData.m_BoundingVolumeSize = glm::vec2(fWidth, fHeight);
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
	case HYTYPE_Spine: // 'NodeItem' panel
	case HYTYPE_TexturedQuad: { // 'NodeItem' panel
		m_PanelData.m_pNodeItem->scale.SetAll(1.0f);
		glm::vec2 vDefaultSize = GetSize();

		if(fWidth == 0.0f)
			fWidth = vDefaultSize.x;
		if(fHeight == 0.0f)
			fHeight = vDefaultSize.y;

		if(vDefaultSize.x != 0.0f && vDefaultSize.y != 0.0f)
		{
			if(true)//bKeepProportions)
			{
				float fScale = std::min(fWidth / vDefaultSize.x, fHeight / vDefaultSize.y);
				m_PanelData.m_pNodeItem->scale.SetAll(fScale);
			}
			else
				m_PanelData.m_pNodeItem->scale.Set(fWidth / vDefaultSize.x, fHeight / vDefaultSize.y);
		}
		break; }

	case HYTYPE_Primitive: // 'Primitive' panel
		ConstructPrimitives(fWidth, fHeight);
		break;

	default:
		HyLogError("HyPanel::SetSize() - Unhandled m_eNodeType: " << m_eNodeType);
		break;
	}
}

bool HyPanel::SetState(uint32 uiWidgetState)
{
	if(IsItemForPanel())
		return m_PanelData.m_pNodeItem->SetState(uiWidgetState);
	else if(IsPrimForPanel())
	{
		switch(static_cast<HyPanelState>(uiWidgetState))
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
			HyError("HyPanel::SetState() - Unknown widget state enum: " << uiWidgetState);
			return false;
		}

		return true;
	}

	return false;
}

uint32 HyPanel::GetNumStates()
{
	if(IsItemForPanel())
		return m_PanelData.m_pNodeItem->GetNumStates();
	else if(IsPrimForPanel())
		return HYNUM_PANELSTATES;

	return 0;
}

bool HyPanel::IsVisible() const
{
	IHyBody2d *pPanelNode = GetPanelNode();
	if(pPanelNode)
		return pPanelNode->IsVisible();

	return false;
}

void HyPanel::SetVisible(bool bVisible)
{
	IHyBody2d *pPanelNode = GetPanelNode();
	if(pPanelNode)
		return pPanelNode->SetVisible(bVisible);
}

HyAnimFloat *HyPanel::Alpha()
{
	IHyBody2d *pPanelNode = GetPanelNode();
	if(pPanelNode)
		return &pPanelNode->alpha;

	return nullptr;
}

bool HyPanel::IsNull() const
{
	return m_eNodeType == HYTYPE_Unknown;
}

bool HyPanel::IsBvForPanel() const
{
	return m_eNodeType == HYTYPE_Entity;
}

bool HyPanel::IsItemForPanel() const
{
	return m_eNodeType != HYTYPE_Entity && m_eNodeType != HYTYPE_Primitive && m_eNodeType != HYTYPE_Unknown;
}

bool HyPanel::IsPrimForPanel() const
{
	return m_eNodeType == HYTYPE_Primitive;
}

uint32 HyPanel::GetFrameStrokeSize() const
{
	if(IsPrimForPanel())
		return m_PanelData.m_pPrimParts->m_uiFrameSize;
	else
		return 0;
}

HyColor HyPanel::GetPanelColor() const
{
	if(IsPrimForPanel())
		return HyColor(m_PanelData.m_pPrimParts->m_Body.topColor.X(), m_PanelData.m_pPrimParts->m_Body.topColor.Y(), m_PanelData.m_pPrimParts->m_Body.topColor.Z());
	else
		return HyColor::Black;
}

HyColor HyPanel::GetFrameColor() const
{
	if(IsPrimForPanel())
		return HyColor(m_PanelData.m_pPrimParts->m_Frame1.topColor.X(), m_PanelData.m_pPrimParts->m_Frame1.topColor.Y(), m_PanelData.m_pPrimParts->m_Frame1.topColor.Z());
	else
		return HyColor::Black;
}

HyColor HyPanel::GetTertiaryColor() const
{
	if(IsPrimForPanel())
		return m_PanelData.m_pPrimParts->m_TertiaryColor;
	else
		return HyColor::Black;
}

IHyBody2d *HyPanel::GetPanelNode() const
{
	switch(m_eNodeType)
	{
	case HYTYPE_Entity:			// 'BoundingVolume' panel (aka no visual panel)
		break;

	case HYTYPE_Sprite:			// 'NodeItem' panel
	case HYTYPE_Spine:			// 'NodeItem' panel
	case HYTYPE_TexturedQuad:	// 'NodeItem' panel
		return m_PanelData.m_pNodeItem;

	case HYTYPE_Primitive:		// 'Primitive' panel
		return m_PanelData.m_pPrimParts;

	default:
		HyLogError("HyPanel::GetPanelNode() - Unhandled m_eNodeType: " << m_eNodeType);
		break;
	}

	return nullptr;
}

HyUiPanelInit HyPanel::ClonePanelInit() const
{
	HyUiPanelInit init = {};
	if(m_eNodeType == HYTYPE_Entity)
		init.m_eNodeType = HYTYPE_Entity;
	else if(IsItemForPanel())
	{
		init.m_eNodeType = m_PanelData.m_pNodeItem->GetType();
		init.m_NodePath = m_PanelData.m_pNodeItem->GetPath();
	}
	else if(IsPrimForPanel())
	{
		init.m_eNodeType = HYTYPE_Primitive;
		init.m_uiFrameSize = m_PanelData.m_pPrimParts->m_uiFrameSize;
		init.m_PanelColor = m_PanelData.m_pPrimParts->m_PanelColor;
		init.m_FrameColor = m_PanelData.m_pPrimParts->m_FrameColor;
		init.m_TertiaryColor = m_PanelData.m_pPrimParts->m_TertiaryColor;
	}

	glm::vec2 vSize = GetSize();
	init.m_uiWidth = static_cast<uint32>(vSize.x);
	init.m_uiHeight = static_cast<uint32>(vSize.y);

	return init;
}

glm::vec2 HyPanel::GetBotLeftOffset()
{
	if(IsItemForPanel() && m_PanelData.m_pNodeItem->IsLoadDataValid() && m_PanelData.m_pNodeItem->GetType() == HYTYPE_Sprite)
	{
		glm::ivec2 vStateOffset = static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GetStateOffset(m_PanelData.m_pNodeItem->GetState());
		return -glm::vec2(vStateOffset.x * m_PanelData.m_pNodeItem->scale.X(), vStateOffset.y * m_PanelData.m_pNodeItem->scale.Y());
	}

	return glm::vec2(0.0f, 0.0f);
}

#ifdef HY_PLATFORM_GUI
void HyPanel::GuiOverridePanelNodeData(HyType eNodeType, HyJsonObj itemDataObj, bool bUseGuiOverrideName, HyEntity2d *pParent)
{	
	if(eNodeType == HYTYPE_Sprite)
	{
		DeleteData();
		m_PanelData.m_pNodeItem = HY_NEW HySprite2d("", HY_GUI_DATAOVERRIDE, pParent);
		static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->GuiOverrideData<HySpriteData>(itemDataObj, bUseGuiOverrideName);
		static_cast<HySprite2d *>(m_PanelData.m_pNodeItem)->SetAllBoundsIncludeAlphaCrop(true);
		m_eNodeType = eNodeType;
	}
	else if(eNodeType == HYTYPE_Spine)
	{
		DeleteData();
		m_PanelData.m_pNodeItem = HY_NEW HySpine2d("", HY_GUI_DATAOVERRIDE, pParent);
		static_cast<HySpine2d *>(m_PanelData.m_pNodeItem)->GuiOverrideData<HySpineData>(itemDataObj, bUseGuiOverrideName);
		m_eNodeType = eNodeType;
	}
	else
		HyLogError("HyPanel::GuiOverrideNodeData() - unsupported type: " << eNodeType);
}
#endif

void HyPanel::ConstructPrimitives(float fWidth, float fHeight)
{
	if(fWidth <= 0.0f || fHeight <= 0.0f)
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Frame2.SetAsNothing();
		m_PanelData.m_pPrimParts->m_Body.SetAsNothing();
		return;
	}

	if(m_PanelData.m_pPrimParts->m_uiFrameSize > 0)
	{
		m_PanelData.m_pPrimParts->m_Frame1.SetAsBox(fWidth, fHeight);

		if(m_PanelData.m_pPrimParts->m_uiFrameSize > 1)
		{
			m_PanelData.m_pPrimParts->m_Frame2.SetAsBox(fWidth - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f, fHeight - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f) + 2.0f);
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

	m_PanelData.m_pPrimParts->m_Body.SetAsBox(fWidth - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f), fHeight - (m_PanelData.m_pPrimParts->m_uiFrameSize * 2.0f));
	m_PanelData.m_pPrimParts->m_Body.pos.Set(static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize), static_cast<int32>(m_PanelData.m_pPrimParts->m_uiFrameSize));
}

void HyPanel::DeleteData()
{
	switch(m_eNodeType)
	{
	case HYTYPE_Unknown:
		break;

	case HYTYPE_Entity:
		m_PanelData.m_BoundingVolumeSize.x = m_PanelData.m_BoundingVolumeSize.y = 0.0f;
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
