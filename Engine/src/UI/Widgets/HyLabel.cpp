/**************************************************************************
*	HyLabel.cpp
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "Assets/Nodes/HySprite2dData.h"
#include "Diagnostics/Console/IHyConsole.h"

HyLabel::PrimPanel::PrimPanel(int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_Fill(this),
	m_Stroke(this)
{
	m_Fill.SetAsBox(iWidth, iHeight);
	m_Fill.SetTint(0.5f, 0.5f, 0.5f);
	
	m_Stroke.SetAsBox(iWidth, iHeight);
	m_Stroke.SetTint(0.3f, 0.3f, 0.3f);
	m_Stroke.SetWireframe(true);
	m_Stroke.SetLineThickness(iStroke);

	Load();
}

HyLabel::HyLabel(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
}

HyLabel::HyLabel(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(iWidth, iHeight, iStroke, sTextPrefix, sTextName);
}

HyLabel::HyLabel(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(iWidth, iHeight, iStroke, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

HyLabel::HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName);
}

HyLabel::HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_uiInfoPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "", this)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

/*virtual*/ HyLabel::~HyLabel()
{
	delete m_pPrimPanel;
}

void HyLabel::Setup(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName)
{
	Setup(iWidth, iHeight, iStroke, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	m_uiInfoPanelAttribs |= INFOPANELATTRIB_IsPrimitive;

	m_SpritePanel.Uninit();
	delete m_pPrimPanel;
	m_pPrimPanel = HY_NEW PrimPanel(iWidth, iHeight, iStroke, this);

	m_Text.Init(sTextPrefix, sTextName, this);
	m_TextMargins.Set(iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);

	CommonSetup();
	OnSetup();
}

void HyLabel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_IsPrimitive;

	delete m_pPrimPanel;
	m_pPrimPanel = nullptr;
	m_SpritePanel.Init(sPanelPrefix, sPanelName, this);

	m_Text.Init(sTextPrefix, sTextName, this);
	m_TextMargins.Set(iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);

	CommonSetup();
	OnSetup();
}

float HyLabel::GetPanelWidth()
{
	if(m_pPrimPanel)
		return m_pPrimPanel->GetSceneAABB().GetExtents().x * 2.0f;
	else if(m_SpritePanel.IsLoadDataValid())
		return m_SpritePanel.GetCurFrameWidth(true);

	return 0.0f;
}

float HyLabel::GetPanelHeight()
{
	if(m_pPrimPanel)
		return m_pPrimPanel->GetSceneAABB().GetExtents().y * 2.0f;
	else if(m_SpritePanel.IsLoadDataValid())
		return m_SpritePanel.GetCurFrameHeight(true);

	return 0.0f;
}

glm::vec2 HyLabel::GetPanelDimensions()
{
	if(m_pPrimPanel)
		return glm::vec2(m_pPrimPanel->GetSceneAABB().GetExtents().x * 2.0f, m_pPrimPanel->GetSceneAABB().GetExtents().y * 2.0f);
	else if(m_SpritePanel.IsLoadDataValid())
		return glm::vec2(m_SpritePanel.GetCurFrameWidth(true), m_SpritePanel.GetCurFrameHeight(true));

	return glm::vec2(0.0f, 0.0f);
}

uint32 HyLabel::GetSpriteState() const
{
	return m_SpritePanel.GetState();
}

/*virtual*/ void HyLabel::SetSpriteState(uint32 uiStateIndex)
{
	if(m_SpritePanel.IsLoadDataValid() == false)
	{
		HyLogWarning("HyLabel::SetSpriteState was invoked with an invalid panel sprite");
		return;
	}

	// Now set new sprite state, so below ResetTextOnPanel() can offset properly onto it
	m_SpritePanel.SetState(uiStateIndex);
	ResetTextOnPanel();
}

/*virtual*/ std::string HyLabel::GetText() const
{
	return m_Text.GetText();
}

/*virtual*/ void HyLabel::SetText(std::string sText)
{
	m_Text.SetText(sText);
}

/*virtual*/ void HyLabel::SetTextState(uint32 uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
}

/*virtual*/ void HyLabel::SetTextAlignment(HyAlignment eAlignment)
{
	m_Text.SetTextAlignment(eAlignment);
}

/*virtual*/ void HyLabel::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_Text.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

bool HyLabel::IsPrimitivePanel() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsPrimitive) != 0;
}

bool HyLabel::IsDisabled() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsDisabled) != 0;
}

/*virtual*/ void HyLabel::SetAsDisabled(bool bIsDisabled)
{
	if(bIsDisabled)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_IsDisabled;

		if(IsHideDisabled() == false)
			SetTint(0.3f, 0.3f, 0.3f);
	}
	else
	{
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_IsDisabled;
		SetTint(1.0f, 1.0f, 1.0f);
	}
}

bool HyLabel::IsHighlighted() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_IsHighlighted) != 0;
}

/*virtual*/ void HyLabel::SetAsHighlighted(bool bIsHighlighted)
{
	if(bIsHighlighted == IsHighlighted())
		return;

	if(bIsHighlighted)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_IsHighlighted;
		if(m_pPrimPanel)
		{
			m_pPrimPanel->m_Stroke.SetTint(0.0f, 0.0f, 1.0f);
			m_pPrimPanel->m_Stroke.SetLineThickness(m_pPrimPanel->m_Stroke.GetLineThickness() * 2.0f);
		}
	}
	else
	{
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_IsHighlighted;
		if(m_pPrimPanel)
		{
			m_pPrimPanel->m_Stroke.SetTint(0.3f, 0.3f, 0.3f);
			m_pPrimPanel->m_Stroke.SetLineThickness(m_pPrimPanel->m_Stroke.GetLineThickness() / 2.0f);
		}
	}
}

bool HyLabel::IsHideDisabled() const
{
	return (m_uiInfoPanelAttribs & INFOPANELATTRIB_HideDisabled) != 0;
}

void HyLabel::SetHideDisabled(bool bIsHideDisabled)
{
	if(bIsHideDisabled)
	{
		m_uiInfoPanelAttribs |= INFOPANELATTRIB_HideDisabled;
		SetTint(1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_uiInfoPanelAttribs &= ~INFOPANELATTRIB_HideDisabled;
		SetAsDisabled(IsDisabled());
	}
}

HyEntity2d *HyLabel::GetPrimitiveNode()
{
	return	m_pPrimPanel;
}

HySprite2d &HyLabel::GetSpriteNode()
{
	return m_SpritePanel;
}

HyText2d &HyLabel::GetTextNode()
{
	return m_Text;
}

void HyLabel::CommonSetup()
{
	m_Text.SetTextAlignment(HYALIGN_HCenter);

	ResetTextOnPanel();

	SetAsDisabled(IsDisabled());
	SetAsHighlighted(IsHighlighted());

	// Determine the m_vUiSizeHint
	if(m_pPrimPanel)
	{
		glm::vec2 vCachedScale = m_pPrimPanel->scale.Get();
		m_pPrimPanel->scale.Set(1.0f, 1.0f);

		auto &aabb = m_pPrimPanel->GetSceneAABB();
		if(aabb.IsValid())
			HySetVec(m_vUiSizeHint, aabb.GetExtents().x * 2.0f, aabb.GetExtents().y * 2.0f);
		
		m_pPrimPanel->scale = vCachedScale;
	}
	else if(m_SpritePanel.IsLoadDataValid())
		HySetVec(m_vUiSizeHint, m_SpritePanel.GetCurFrameWidth(false), m_SpritePanel.GetCurFrameHeight(false));
	
	// m_vUiSizeHint must be established
	if(m_vUiSizeHint.x == 0 || m_vUiSizeHint.y == 0)
		HySetVec(m_vUiSizeHint, 300, 75);
}

/*virtual*/ glm::vec2 HyLabel::GetPosOffset() /*override*/
{
	b2Vec2 ptLowerBound(0.0f, 0.0f);
	if(m_pPrimPanel)
		ptLowerBound = m_pPrimPanel->GetSceneAABB().lowerBound;
	else if(m_SpritePanel.IsLoadDataValid())
		ptLowerBound = m_SpritePanel.GetSceneAABB().lowerBound;
	else
	{
		// Have a zero vector be returned
		ptLowerBound.x = pos.Get().x;
		ptLowerBound.y = pos.Get().y;
	}

	return glm::vec2(pos.Get() - glm::vec2(ptLowerBound.x, ptLowerBound.y));
}

/*virtual*/ void HyLabel::OnResize(int32 iNewWidth, int32 iNewHeight) /*override*/
{
	if(m_pPrimPanel)
	{
		m_pPrimPanel->scale.X(static_cast<float>(iNewWidth) / m_vUiSizeHint.x);
		m_pPrimPanel->scale.Y(static_cast<float>(iNewHeight) / m_vUiSizeHint.y);
	}
	else if(m_SpritePanel.IsLoadDataValid())
	{
		m_SpritePanel.scale.X(static_cast<float>(iNewWidth) / m_vUiSizeHint.x);
		m_SpritePanel.scale.Y(static_cast<float>(iNewHeight) / m_vUiSizeHint.y);
	}
	else
		m_Text.SetAsScaleBox(iNewWidth - m_TextMargins.left - m_TextMargins.right, iNewHeight - m_TextMargins.bottom - m_TextMargins.top);

	ResetTextOnPanel();
}

/*virtual*/ void HyLabel::ResetTextOnPanel()
{
	glm::vec2 vPanelDimensions = GetPanelDimensions();
	glm::ivec2 vPanelOffset = GetPosOffset();

	// Position text
	m_Text.pos.Set((m_TextMargins.left * (vPanelDimensions.x / m_vUiSizeHint.x)) - vPanelOffset.x,
				   (m_TextMargins.bottom * (vPanelDimensions.y / m_vUiSizeHint.y)) - vPanelOffset.y);

	// Size text
	if(vPanelDimensions.x != 0.0f && vPanelDimensions.y != 0.0f)
		m_Text.SetAsScaleBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / m_vUiSizeHint.x)),
							 vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top) * (vPanelDimensions.y / m_vUiSizeHint.y)), true);
}
