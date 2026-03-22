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
#include "Gui/Components/HyPanel.h"
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

// Constructs an 'Primitive' panel. Default HyColor values of 0xDEADBE will be set to a default color determined by the widget
HyUiPanelInit::HyUiPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor /*= HyColor(0xDE,0xAD,0xBE)*/, HyColor frameColor /*= HyColor(0xDE,0xAD,0xBE)*/, HyColor tertiaryColor /*= HyColor(0xDE,0xAD,0xBE)*/) :
	m_eNodeType(HYTYPE_Primitive),
	m_uiWidth(uiWidth),
	m_uiHeight(uiHeight),
	m_NodePath(),
	m_uiFrameSize(uiFrameSize),
	m_PanelColor(panelColor),
	m_FrameColor(frameColor),
	m_TertiaryColor(tertiaryColor)
{ }

// Constructs an 'Primitive' panel as the specified default widget. Default HyColor values of 0xDEADBE will be set to a default color determined by the widget
HyUiPanelInit::HyUiPanelInit(HyWidgetType eWidgetType, HyColor panelColor /*= HyColor(0xDE,0xAD,0xBE)*/, HyColor frameColor /*= HyColor(0xDE,0xAD,0xBE)*/, HyColor tertiaryColor /*= HyColor(0xDE,0xAD,0xBE)*/) :
	m_eNodeType(HYTYPE_Primitive),
	m_NodePath(),
	m_PanelColor(panelColor),
	m_FrameColor(frameColor),
	m_TertiaryColor(tertiaryColor)
{
	switch(eWidgetType)
	{
	case HYWIDGET_Label:
	case HYWIDGET_RackMeter:
	case HYWIDGET_BarMeter:
		m_uiWidth = 150;
		m_uiHeight = 25;
		m_uiFrameSize = 1;
		break;
	case HYWIDGET_Button:
		m_uiWidth = 100;
		m_uiHeight = 25;
		m_uiFrameSize = 1;
		break;
	case HYWIDGET_CheckBox:
	case HYWIDGET_RadioButton:
		m_uiWidth = 15;
		m_uiHeight = 15;
		m_uiFrameSize = 1;
		break;
	case HYWIDGET_Slider:
	case HYWIDGET_ComboBox:
		m_uiWidth = 100;
		m_uiHeight = 25;
		m_uiFrameSize = 1;
		break;
	case HYWIDGET_TextField:
		m_uiWidth = 150;
		m_uiHeight = 25;
		m_uiFrameSize = 1;
		break;
	case HYWIDGET_RichText:
		m_uiWidth = 300;
		m_uiHeight = 200;
		m_uiFrameSize = 1;
		break;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyPanel::HyPanel(HyEntity2d *pParent) :
	m_pParent(pParent),
	m_eNodeType(HYTYPE_Unknown),
	m_Size(0.0f, 0.0f),
	m_pNode(nullptr)
{
	HyAssert(m_pParent != nullptr, "HyPanel::HyPanel 'm_pParent' cannot be null");
}

HyPanel::HyPanel(const HyUiPanelInit &initRef, HyEntity2d *pParent) :
	m_pParent(pParent),
	m_eNodeType(HYTYPE_Unknown),
	m_Size(0.0f, 0.0f),
	m_pNode(nullptr)
{
	HyAssert(m_pParent != nullptr, "HyPanel::HyPanel 'm_pParent' cannot be null");
	Setup(initRef);
}

HyPanel::~HyPanel()
{
	DeleteData();
}

void HyPanel::Setup(const HyUiPanelInit &initRef)
{
	DeleteData(); // Delete any previous data

	m_eNodeType = initRef.m_eNodeType;
	switch(m_eNodeType)
	{
	case HYTYPE_Entity: // 'BoundingVolume' panel (aka no visual panel)
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
		m_pNode = HY_NEW HySprite2d(initRef.m_NodePath, nullptr);
		m_pParent->ChildPrepend(*m_pNode);
		InitalizeSprite();
		m_pNode->pos.Set(m_ptPosition);
		break;
	case HYTYPE_Spine: // 'NodeItem' panel
		m_pNode = HY_NEW HySpine2d(initRef.m_NodePath, nullptr);
		m_pParent->ChildPrepend(*m_pNode);
		m_pNode->pos.Set(m_ptPosition);
		break;
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
		m_pNode = HY_NEW HyTexturedQuad2d(initRef.m_NodePath, nullptr);
		m_pParent->ChildPrepend(*m_pNode);
		m_pNode->pos.Set(m_ptPosition);
		break;

	case HYTYPE_Primitive: { // 'Primitive' panel
		m_pNode = HY_NEW PrimMade(initRef, nullptr);
		m_pParent->ChildPrepend(*m_pNode);

		PrimMade *pPrimMade = static_cast<PrimMade *>(m_pNode);
		// If a PrimPart's color's RGBA is 0xDEADBEEF, then reassign it to a default color
		if(pPrimMade->m_PanelColor == HyColor::_InternalUse)
			pPrimMade->m_PanelColor = HyColor::PanelWidget;
		if(pPrimMade->m_FrameColor == HyColor::_InternalUse)
			pPrimMade->m_FrameColor = HyColor::FrameWidget;
		if(pPrimMade->m_TertiaryColor == HyColor::_InternalUse)
			pPrimMade->m_TertiaryColor = HyColor::Orange;

		pPrimMade->SetLayerColor(PRIMLAYER_Body, pPrimMade->m_PanelColor);
		pPrimMade->SetLayerColor(PRIMLAYER_Frame, pPrimMade->m_FrameColor);
		pPrimMade->SetLayerColor(PRIMLAYER_Highlight, pPrimMade->m_TertiaryColor);
		pPrimMade->pos.Set(m_ptPosition);
		SetState(HYPANELSTATE_Idle);
		break; }

	default:
		HyLogError("HyPanel::Setup() - Unhandled m_eNodeType: " << m_eNodeType);
		break;
	}

	SetSize(initRef.m_uiWidth, initRef.m_uiHeight);
}

float HyPanel::GetWidth(float fPercent /*= 1.0f*/) const
{
	return m_Size.x * fPercent;
	//switch(m_eNodeType)
	//{
	//case HYTYPE_Entity: // 'BoundingVolume' panel (aka no visual panel)
	//	return m_Size.x * fPercent;

	//case HYTYPE_Sprite: // 'NodeItem' panel
	//case HYTYPE_Spine: // 'NodeItem' panel
	//case HYTYPE_TexturedQuad: // 'NodeItem' panel
	//	if(m_pNode->GetType() == HYTYPE_Sprite)
	//		return static_cast<HySprite2d *>(m_pNode)->GetStateWidth(m_pNode->GetState(), fPercent);
	//	else
	//		return m_pNode->GetWidth(fPercent);

	//case HYTYPE_Primitive: // 'Primitive' panel
	//	if(m_PanelData.m_pPrimMade == nullptr || m_PanelData.m_pPrimMade->GetLayerType(PRIMLAYER_Body) == HYFIXTURE_Nothing)
	//		return 0.0f;
	//	return m_PanelData.m_pPrimMade->GetWidth(fPercent);

	//default:
	//	HyLogError("HyPanel::GetWidth() - Unhandled m_eNodeType: " << m_eNodeType);
	//	break;
	//}

	//return 0.0f;
}

float HyPanel::GetHeight(float fPercent /*= 1.0f*/) const
{
	return m_Size.y * fPercent;
	//switch(m_eNodeType)
	//{
	//case HYTYPE_Entity: // 'BoundingVolume' panel (aka no visual panel)
	//	return m_Size.y * fPercent;

	//case HYTYPE_Sprite: // 'NodeItem' panel
	//case HYTYPE_Spine: // 'NodeItem' panel
	//case HYTYPE_TexturedQuad: // 'NodeItem' panel
	//	if(m_pNode->GetType() == HYTYPE_Sprite)
	//		return static_cast<HySprite2d *>(m_pNode)->GetStateHeight(m_pNode->GetState(), fPercent);
	//	else
	//		return m_pNode->GetHeight(fPercent);

	//case HYTYPE_Primitive: // 'Primitive' panel
	//	if(m_PanelData.m_pPrimMade == nullptr || m_PanelData.m_pPrimMade->GetLayerType(PRIMLAYER_Body) == HYFIXTURE_Nothing)
	//		return 0.0f;
	//	return m_PanelData.m_pPrimMade->GetHeight(fPercent);

	//default:
	//	HyLogError("HyPanel::GetHeight() - Unhandled m_eNodeType: " << m_eNodeType);
	//	break;
	//}
	//
	//return 0.0f;
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
		HySetVec(m_Size, fWidth, fHeight);
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
	case HYTYPE_Spine: // 'NodeItem' panel
	case HYTYPE_TexturedQuad: { // 'NodeItem' panel
		m_pNode->scale.SetAll(1.0f);


		glm::vec2 vDefaultSize(0.0f, 0.0f);
		if(m_pNode->GetType() == HYTYPE_Sprite)
		{
			vDefaultSize.x = static_cast<HySprite2d *>(m_pNode)->GetStateWidth(m_pNode->GetState(), 1.0f);
			vDefaultSize.y = static_cast<HySprite2d *>(m_pNode)->GetStateHeight(m_pNode->GetState(), 1.0f);
		}
		else
		{
			vDefaultSize.x = m_pNode->GetWidth(1.0f);
			vDefaultSize.y = m_pNode->GetHeight(1.0f);
		}
	
		if(fWidth == 0.0f)
			fWidth = vDefaultSize.x;
		if(fHeight == 0.0f)
			fHeight = vDefaultSize.y;
		HySetVec(m_Size, fWidth, fHeight);

		if(vDefaultSize.x != 0.0f && vDefaultSize.y != 0.0f)
		{
			if(true)//bKeepProportions)
			{
				float fScale = std::min(fWidth / vDefaultSize.x, fHeight / vDefaultSize.y);
				m_pNode->scale.SetAll(fScale);
			}
			else
				m_pNode->scale.Set(fWidth / vDefaultSize.x, fHeight / vDefaultSize.y);
		}

		if(m_eNodeType == HYTYPE_Sprite)
			InitalizeSprite();
		break; }

	case HYTYPE_Primitive: // 'Primitive' panel
		HySetVec(m_Size, fWidth, fHeight);
		ConstructPrimitives();
		break;

	default:
		HyLogError("HyPanel::SetSize() - Unhandled m_eNodeType: " << m_eNodeType);
		break;
	}
}

bool HyPanel::SetState(uint32 uiWidgetState)
{
	if(IsItemForPanel())
		return m_pNode->SetState(uiWidgetState);
	else if(IsPrimForPanel())
	{
		PrimMade *pPrimMade = static_cast<PrimMade *>(m_pNode);
		switch(static_cast<HyPanelState>(uiWidgetState))
		{
		case HYPANELSTATE_Idle:
			pPrimMade->SetLayerColor(PRIMLAYER_Body, pPrimMade->m_PanelColor);
			pPrimMade->SetLayerColor(PRIMLAYER_Frame, pPrimMade->m_FrameColor);
			pPrimMade->SetLayerVisible(PRIMLAYER_Highlight, false);
			break;
		case HYPANELSTATE_Down:
			pPrimMade->SetLayerColor(PRIMLAYER_Body, pPrimMade->m_PanelColor.Darken());
			pPrimMade->SetLayerColor(PRIMLAYER_Frame, pPrimMade->m_FrameColor);
			pPrimMade->SetLayerVisible(PRIMLAYER_Highlight, false);
			break;
		case HYPANELSTATE_Hover:
			pPrimMade->SetLayerColor(PRIMLAYER_Body, pPrimMade->m_PanelColor.Lighten());
			pPrimMade->SetLayerColor(PRIMLAYER_Frame, pPrimMade->m_FrameColor);
			pPrimMade->SetLayerVisible(PRIMLAYER_Highlight, false);
			break;
		case HYPANELSTATE_Highlighted:
			pPrimMade->SetLayerColor(PRIMLAYER_Body, pPrimMade->m_PanelColor);
			pPrimMade->SetLayerColor(PRIMLAYER_Frame, pPrimMade->m_FrameColor.Lighten());
			pPrimMade->SetLayerVisible(PRIMLAYER_Highlight, true);
			break;
		case HYPANELSTATE_HighlightedDown:
			pPrimMade->SetLayerColor(PRIMLAYER_Body, pPrimMade->m_PanelColor.Darken());
			pPrimMade->SetLayerColor(PRIMLAYER_Frame, pPrimMade->m_FrameColor.Lighten());
			pPrimMade->SetLayerVisible(PRIMLAYER_Highlight, true);
			break;
		case HYPANELSTATE_HighlightedHover:
			pPrimMade->SetLayerColor(PRIMLAYER_Body, pPrimMade->m_PanelColor.Lighten());
			pPrimMade->SetLayerColor(PRIMLAYER_Frame, pPrimMade->m_FrameColor.Lighten());
			pPrimMade->SetLayerVisible(PRIMLAYER_Highlight, true);
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
		return m_pNode->GetNumStates();
	else if(IsPrimForPanel())
		return HYNUM_PANELSTATES;

	return 0;
}

glm::vec2 HyPanel::GetPosition() const
{
	IHyBody2d *pPanelNode = GetPanelNode();
	if(pPanelNode)
		return pPanelNode->pos.Get();

	return m_ptPosition;
}

void HyPanel::SetPosition(float fX, float fY)
{
	IHyBody2d *pPanelNode = GetPanelNode();
	if(pPanelNode)
		pPanelNode->pos.Set(fX, fY);
	
	HySetVec(m_ptPosition, fX, fY);
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
		return static_cast<PrimMade *>(m_pNode)->m_uiFrameSize;
	else
		return 0;
}

HyColor HyPanel::GetPanelColor() const
{
	if(IsPrimForPanel())
		return static_cast<PrimMade *>(m_pNode)->m_PanelColor;
	else
		return HyColor::Black;
}

HyColor HyPanel::GetFrameColor() const
{
	if(IsPrimForPanel())
		return static_cast<PrimMade *>(m_pNode)->m_FrameColor;
	else
		return HyColor::Black;
}

HyColor HyPanel::GetTertiaryColor() const
{
	if(IsPrimForPanel())
		return static_cast<PrimMade *>(m_pNode)->m_TertiaryColor;
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
	case HYTYPE_Primitive:		// 'Primitive' panel
		return m_pNode;

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
		init.m_eNodeType = m_pNode->GetType();
		init.m_NodePath = m_pNode->GetPath();
	}
	else if(IsPrimForPanel())
	{
		PrimMade *pPrimMade = static_cast<PrimMade *>(m_pNode);

		init.m_eNodeType = HYTYPE_Primitive;
		init.m_uiFrameSize = pPrimMade->m_uiFrameSize;
		init.m_PanelColor = pPrimMade->m_PanelColor;
		init.m_FrameColor = pPrimMade->m_FrameColor;
		init.m_TertiaryColor = pPrimMade->m_TertiaryColor;
	}

	glm::vec2 vSize = GetSize();
	init.m_uiWidth = static_cast<uint32>(vSize.x);
	init.m_uiHeight = static_cast<uint32>(vSize.y);

	return init;
}

#ifdef HY_PLATFORM_GUI
void HyPanel::GuiOverridePanelNodeData(HyType eNodeType, HyJsonObj itemDataObj, bool bUseGuiOverrideName, HyEntity2d *pParent)
{	
	if(eNodeType == HYTYPE_Sprite)
	{
		DeleteData();
		m_pNode = HY_NEW HySprite2d("", HY_GUI_DATAOVERRIDE, pParent);
		static_cast<HySprite2d *>(m_pNode)->GuiOverrideData<HySpriteData>(itemDataObj, bUseGuiOverrideName);
		m_pNode->Load();
		m_eNodeType = eNodeType;
		InitalizeSprite();
	}
	else if(eNodeType == HYTYPE_Spine)
	{
		DeleteData();
		m_pNode = HY_NEW HySpine2d("", HY_GUI_DATAOVERRIDE, pParent);
		static_cast<HySpine2d *>(m_pNode)->GuiOverrideData<HySpineData>(itemDataObj, bUseGuiOverrideName);
		m_pNode->Load();
		m_eNodeType = eNodeType;
	}
	else
		HyLogError("HyPanel::GuiOverrideNodeData() - unsupported type: " << eNodeType);
}
#endif

void HyPanel::InitalizeSprite()
{
	if(IsItemForPanel() == false || m_pNode == nullptr || m_pNode->GetType() != HYTYPE_Sprite)
	{
		HyLogError("HyPanel::InitalizeSprite() - Panel is not a 'NodeItem' panel with a Sprite node");
		return;
	}
	
	static_cast<HySprite2d *>(m_pNode)->SetAllBoundsIncludeAlphaCrop(true);

	if(m_pNode->IsLoadDataValid() == false)
		return;

	glm::ivec2 vStateOffset = static_cast<HySprite2d *>(m_pNode)->GetStateOffset(m_pNode->GetState());
	m_pNode->pos.Set((GetWidth(0.5f) + vStateOffset.x) * m_pNode->scale.X(), (GetHeight(0.5f) + vStateOffset.y) * m_pNode->scale.Y());
}

void HyPanel::ConstructPrimitives()
{
	PrimMade *pPrimMade = static_cast<PrimMade *>(m_pNode);
	float fWidth = m_Size.x;
	float fHeight = m_Size.y;

	if(fWidth <= 0.0f || fHeight <= 0.0f)
	{
		pPrimMade->SetAsNothing(PRIMLAYER_Frame);
		pPrimMade->SetAsNothing(PRIMLAYER_Body);
		pPrimMade->SetAsNothing(PRIMLAYER_Highlight);
		return;
	}

	if(pPrimMade->m_uiFrameSize > 0)
		pPrimMade->SetAsBox(PRIMLAYER_Frame, fWidth, fHeight, 0.0f);
	else
		pPrimMade->SetAsNothing(PRIMLAYER_Frame);
	
	float fHighlightThickness = std::max(1.0f, static_cast<float>(pPrimMade->m_uiFrameSize) * 0.25f);
	HyRect frameRect(fWidth - (pPrimMade->m_uiFrameSize * 2.0f),
					 fHeight - (pPrimMade->m_uiFrameSize * 2.0f));
	pPrimMade->SetAsBox(PRIMLAYER_Highlight, frameRect, fHighlightThickness);
		

	HyRect bodyRect(fWidth - (pPrimMade->m_uiFrameSize * 2.0f),
					fHeight - (pPrimMade->m_uiFrameSize * 2.0f));
	pPrimMade->SetAsBox(PRIMLAYER_Body, bodyRect, 0.0f);
}

void HyPanel::DeleteData()
{
	switch(m_eNodeType)
	{
	case HYTYPE_Unknown:
		break;

	case HYTYPE_Entity:
		break;

	case HYTYPE_Sprite: // 'NodeItem' panel
	case HYTYPE_Spine: // 'NodeItem' panel
	case HYTYPE_TexturedQuad: // 'NodeItem' panel
	case HYTYPE_Primitive: // 'Primitive' panel
		delete m_pNode;
		m_pNode = nullptr;
		break;

	default:
		HyLogError("HyPanel::Setup() - Invalid HyType for HyBody: " << m_eNodeType);
		break;
	}

	m_eNodeType = HYTYPE_Unknown;
	m_Size.x = m_Size.y = 0.0f;
}
