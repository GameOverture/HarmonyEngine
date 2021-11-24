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
#include "HyEngine.h"

HyLabel::HyLabel(HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "")
{
	Setup("", "", "", "");
}

HyLabel::HyLabel(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "")
{
	Setup(initRef, sTextPrefix, sTextName, 0, 0, 0, 0);
}

HyLabel::HyLabel(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "")
{
	Setup(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

HyLabel::HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "")
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName);
}

HyLabel::HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiPanelAttribs(0),
	m_pPrimPanel(nullptr),
	m_SpritePanel("", "", this),
	m_Text("", "")
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

/*virtual*/ HyLabel::~HyLabel()
{
	delete m_pPrimPanel;
}

void HyLabel::Setup(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName)
{
	Setup(initRef, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(const HyPrimitivePanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	m_uiPanelAttribs |= PANELATTRIB_IsPrimitive;

	m_SpritePanel.Uninit();
	delete m_pPrimPanel;
	m_pPrimPanel = HY_NEW HyPrimitivePanel(initRef, this);

	m_Text.Init(sTextPrefix, sTextName, this);
	m_TextMargins.Set(static_cast<float>(iTextMarginLeft),
					  static_cast<float>(iTextMarginBottom),
					  static_cast<float>(iTextMarginRight),
					  static_cast<float>(iTextMarginTop));

	SetAsEnabled(IsEnabled());
	SetAsHighlighted(IsHighlighted());

	ResetTextAndPanel();
	OnSetup();
}

void HyLabel::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	m_uiPanelAttribs &= ~PANELATTRIB_IsPrimitive;

	delete m_pPrimPanel;
	m_pPrimPanel = nullptr;
	m_SpritePanel.Init(sPanelPrefix, sPanelName, this);

	m_Text.Init(sTextPrefix, sTextName, this);
	m_TextMargins.Set(static_cast<float>(iTextMarginLeft),
					  static_cast<float>(iTextMarginBottom),
					  static_cast<float>(iTextMarginRight),
					  static_cast<float>(iTextMarginTop));

	SetAsEnabled(IsEnabled());
	SetAsHighlighted(IsHighlighted());

	ResetTextAndPanel();
	OnSetup();
}

/*virtual*/ void HyLabel::SetAsStacked(HyAlignment eTextAlignment /*= HYALIGN_HCenter*/)
{
	m_uiPanelAttribs &= ~PANELATTRIB_IsSideBySide;
	switch(eTextAlignment)
	{
	case HYALIGN_Left:
		m_uiPanelAttribs &= ~(PANELATTRIB_StackedTextRightAlign | PANELATTRIB_StackedTextJustifyAlign);
		m_uiPanelAttribs |= PANELATTRIB_StackedTextLeftAlign;
		break;
	case HYALIGN_HCenter:
		m_uiPanelAttribs &= ~(PANELATTRIB_StackedTextLeftAlign | PANELATTRIB_StackedTextRightAlign | PANELATTRIB_StackedTextJustifyAlign);
		break;
	case HYALIGN_Right:
		m_uiPanelAttribs &= ~(PANELATTRIB_StackedTextLeftAlign | PANELATTRIB_StackedTextJustifyAlign);
		m_uiPanelAttribs |= PANELATTRIB_StackedTextRightAlign;
		break;
	case HYALIGN_Justify:
		m_uiPanelAttribs &= ~(PANELATTRIB_StackedTextLeftAlign | PANELATTRIB_StackedTextRightAlign);
		m_uiPanelAttribs |= PANELATTRIB_StackedTextJustifyAlign;
		break;
	}
	
	ResetTextAndPanel();
}

void HyLabel::SetAsSideBySide(bool bPanelBeforeText /*= true*/, int32 iPadding /*= 5*/, HyOrientation eOrientation /*= HYORIEN_Horizontal*/)
{
	m_uiPanelAttribs |= PANELATTRIB_IsSideBySide;
	if(bPanelBeforeText)
		m_uiPanelAttribs &= ~PANELATTRIB_SideBySideTextFirst;
	else
		m_uiPanelAttribs |= PANELATTRIB_SideBySideTextFirst;

	m_TextMargins.iTag = iPadding;

	if(eOrientation == HYORIEN_Horizontal)
		m_uiPanelAttribs &= ~PANELATTRIB_SideBySideVertical;
	else
		m_uiPanelAttribs |= PANELATTRIB_SideBySideVertical;

	ResetTextAndPanel();
}

float HyLabel::GetPanelWidth()
{
	if(m_pPrimPanel)
		return m_pPrimPanel->GetSceneWidth();
	else if(m_SpritePanel.IsLoadDataValid())
		return m_SpritePanel.GetStateMaxWidth(m_SpritePanel.GetState(), true);

	return 0.0f;
}

float HyLabel::GetPanelHeight()
{
	if(m_pPrimPanel)
		return m_pPrimPanel->GetSceneHeight();
	else if(m_SpritePanel.IsLoadDataValid())
		return m_SpritePanel.GetStateMaxHeight(m_SpritePanel.GetState(), true);

	return 0.0f;
}

glm::vec2 HyLabel::GetPanelDimensions()
{
	return glm::vec2(GetPanelWidth(), GetPanelHeight());
}

uint32 HyLabel::GetSpriteState() const
{
	return m_SpritePanel.GetState();
}

/*virtual*/ void HyLabel::SetSpriteState(uint32 uiStateIndex)
{
	if(m_SpritePanel.IsLoadDataValid() == false)
	{
		HyLogDebug("HyLabel::SetSpriteState was invoked with an invalid panel sprite");
		return;
	}

	// Now set new sprite state, so below ResetTextAndPanel() can offset properly onto it
	m_SpritePanel.SetState(uiStateIndex);
	ResetTextAndPanel();
}

/*virtual*/ std::string HyLabel::GetUtf8String() const
{
	return m_Text.GetUtf8String();
}

void HyLabel::SetText(const std::stringstream &ssUtf8Text)
{
	SetText(ssUtf8Text.str());
}

void HyLabel::SetText(const std::string &sUtf8Text)
{
	m_Text.SetText(sUtf8Text);
	ResetTextAndPanel();
}

/*virtual*/ void HyLabel::SetTextState(uint32 uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
	ResetTextAndPanel();
}

/*virtual*/ void HyLabel::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_Text.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

bool HyLabel::IsPrimitivePanel() const
{
	return (m_uiPanelAttribs & PANELATTRIB_IsPrimitive) != 0;
}

bool HyLabel::IsEnabled() const
{
	return (m_uiPanelAttribs & PANELATTRIB_IsDisabled) == 0;
}

/*virtual*/ void HyLabel::SetAsEnabled(bool bEnabled)
{
	if(bEnabled)
	{
		m_uiPanelAttribs &= ~PANELATTRIB_IsDisabled;
		topColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);
		botColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);

		if(IsMouseInBounds())
			OnMouseEnter();
	}
	else
	{
		m_uiPanelAttribs |= PANELATTRIB_IsDisabled;

		if(IsHideDisabled() == false)
		{
			topColor.Tween(0.3f, 0.3f, 0.3f, 0.25f);
			botColor.Tween(0.3f, 0.3f, 0.3f, 0.25f);
		}

		OnMouseLeave();
	}
}

bool HyLabel::IsHighlighted() const
{
	return (m_uiPanelAttribs & PANELATTRIB_IsHighlighted) != 0;
}

/*virtual*/ void HyLabel::SetAsHighlighted(bool bIsHighlighted)
{
	if(bIsHighlighted == IsHighlighted())
		return;

	if(bIsHighlighted)
	{
		m_uiPanelAttribs |= PANELATTRIB_IsHighlighted;
		if(m_pPrimPanel)
		{
			m_pPrimPanel->SetBorderColor(0x0000FF);
			//m_pPrimPanel->m_Stroke.SetLineThickness(m_pPrimPanel->m_Stroke.GetLineThickness() * 2.0f);
		}
	}
	else
	{
		m_uiPanelAttribs &= ~PANELATTRIB_IsHighlighted;
		if(m_pPrimPanel)
		{
			m_pPrimPanel->SetBorderColor(0x3F3F41);
			//m_pPrimPanel->m_Stroke.SetLineThickness(m_pPrimPanel->m_Stroke.GetLineThickness() / 2.0f);
		}
	}
}

bool HyLabel::IsShowHandCursor() const
{
	return (m_uiPanelAttribs & PANELATTRIB_ShowHandCursor) != 0;
}

void HyLabel::SetShowHandCursor(bool bShowHandCursor)
{
	if(bShowHandCursor)
		m_uiPanelAttribs |= PANELATTRIB_ShowHandCursor;
	else
		m_uiPanelAttribs &= ~PANELATTRIB_ShowHandCursor;
}

bool HyLabel::IsHideDisabled() const
{
	return (m_uiPanelAttribs & PANELATTRIB_HideDisabled) != 0;
}

void HyLabel::SetHideDisabled(bool bIsHideDisabled)
{
	if(bIsHideDisabled)
	{
		m_uiPanelAttribs |= PANELATTRIB_HideDisabled;
		SetTint(1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_uiPanelAttribs &= ~PANELATTRIB_HideDisabled;
		SetAsEnabled(IsEnabled());
	}
}

HyPrimitivePanel *HyLabel::GetPrimitivePanel()
{
	return m_pPrimPanel;
}

HySprite2d &HyLabel::GetSpriteNode()
{
	return m_SpritePanel;
}

HyText2d &HyLabel::GetTextNode()
{
	return m_Text;
}

/*virtual*/ void HyLabel::OnMouseEnter() /*override*/
{
	if(IsShowHandCursor() && IsEnabled())
		HyEngine::Input().SetMouseCursor(HYMOUSECURSOR_Hand);
}

/*virtual*/ void HyLabel::OnMouseLeave() /*override*/
{
	if(IsShowHandCursor())
		HyEngine::Input().ResetMouseCursor();
}

/*virtual*/ glm::vec2 HyLabel::GetPosOffset() /*override*/
{
	if((m_uiPanelAttribs & PANELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_pPrimPanel || m_SpritePanel.IsLoadDataValid())
			return GetPanelOffset();
		else if(m_Text.IsLoadDataValid())
			return -m_Text.GetTextBottomLeft();
	}

	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ void HyLabel::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, 0, 0);

	if((m_uiPanelAttribs & PANELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_pPrimPanel)
		{
			glm::vec2 vPreserveScale = m_pPrimPanel->scale.Get();
			m_pPrimPanel->scale.Set(1.0f, 1.0f);

			auto &aabb = m_pPrimPanel->GetSceneAABB();
			if(aabb.IsValid())
				HySetVec(m_vSizeHint, static_cast<int32>(aabb.GetExtents().x * 2.0f), static_cast<int32>(aabb.GetExtents().y * 2.0f));

			m_pPrimPanel->scale = vPreserveScale;
		}
		else if(m_SpritePanel.IsLoadDataValid())
		{
			HySetVec(m_vSizeHint,
				static_cast<int32>(m_SpritePanel.GetStateMaxWidth(m_SpritePanel.GetState(), false)),
				static_cast<int32>(m_SpritePanel.GetStateMaxHeight(m_SpritePanel.GetState(), false)));
		}
		else if(m_Text.IsLoadDataValid())
			HySetVec(m_vSizeHint, static_cast<int32>(m_Text.GetTextWidth(false)), static_cast<int32>(m_Text.GetTextHeight(false)));
	}
	else // Side-by-side
	{
		if(m_uiPanelAttribs & PANELATTRIB_SideBySideVertical)
		{
			m_vSizeHint.x = HyMax(GetPanelWidth(), m_Text.GetTextWidth(true));
			m_vSizeHint.y = GetPanelHeight() + m_TextMargins.iTag + m_Text.GetTextHeight(true);
		}
		else
		{
			m_vSizeHint.x = GetPanelWidth() + m_TextMargins.iTag + m_Text.GetTextWidth(true);
			m_vSizeHint.y = HyMax(GetPanelHeight(), m_Text.GetTextHeight(true));
		}
	}
}

/*virtual*/ glm::ivec2 HyLabel::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	auto vUiSizeHint = GetSizeHint();
	if(m_pPrimPanel)
	{
		m_pPrimPanel->scale.X(static_cast<float>(uiNewWidth) / vUiSizeHint.x);
		m_pPrimPanel->scale.Y(static_cast<float>(uiNewHeight) / vUiSizeHint.y);
	}
	else if(m_SpritePanel.IsLoadDataValid())
	{
		m_SpritePanel.scale.X(static_cast<float>(uiNewWidth) / vUiSizeHint.x);
		m_SpritePanel.scale.Y(static_cast<float>(uiNewHeight) / vUiSizeHint.y);
	}
	else if(m_Text.IsLoadDataValid())
	{
		glm::vec2 vTextSize(m_Text.GetTextWidth(false), m_Text.GetTextHeight(false));

		float fScaleX = uiNewWidth / vTextSize.x;
		float fScaleY = uiNewHeight / vTextSize.y;
		scale.Set(HyMin(fScaleX, fScaleY));

		//m_Text.SetAsScaleBox(iNewWidth - m_TextMargins.left - m_TextMargins.right, iNewHeight - m_TextMargins.bottom - m_TextMargins.top);
	}

	ResetTextAndPanel();
	
	return glm::ivec2(uiNewWidth, uiNewHeight);
}

/*virtual*/ void HyLabel::ResetTextAndPanel()
{
	if(m_uiPanelAttribs & PANELATTRIB_IsSideBySide)
	{
		m_Text.SetAsLine();

		// Determine "first" and "second" size and offset
		IHyBody2d *pFirst = nullptr;
		glm::vec2 vFirstSize;
		glm::vec2 vFirstOffset;

		IHyBody2d *pSecond = nullptr;
		glm::vec2 vSecondSize;
		glm::vec2 vSecondOffset;

		if(m_uiPanelAttribs & PANELATTRIB_SideBySideTextFirst)
		{
			if(m_uiPanelAttribs & PANELATTRIB_SideBySideVertical)
				m_Text.SetTextAlignment(HYALIGN_HCenter);
			else
				m_Text.SetTextAlignment(HYALIGN_Right);

			pFirst = &m_Text;
			HySetVec(vFirstSize, m_Text.GetTextWidth(true), m_Text.GetTextHeight(true));
			vFirstOffset = m_Text.GetTextBottomLeft();

			pSecond = m_pPrimPanel ? static_cast<IHyBody2d *>(m_pPrimPanel) : &m_SpritePanel;
			HySetVec(vSecondSize, GetPanelWidth(), GetPanelHeight());
			vSecondOffset = -GetPanelOffset();
		}
		else
		{
			if(m_uiPanelAttribs & PANELATTRIB_SideBySideVertical)
				m_Text.SetTextAlignment(HYALIGN_HCenter);
			else
				m_Text.SetTextAlignment(HYALIGN_Left);

			pFirst = m_pPrimPanel ? static_cast<IHyBody2d *>(m_pPrimPanel) : &m_SpritePanel;
			HySetVec(vFirstSize, GetPanelWidth(), GetPanelHeight());
			vFirstOffset = -GetPanelOffset();

			pSecond = &m_Text;
			HySetVec(vSecondSize, m_Text.GetTextWidth(true), m_Text.GetTextHeight(true));
			vSecondOffset = m_Text.GetTextBottomLeft();
		}

		// Then position "first" and "second" appropriately
		pFirst->pos.Set(-vFirstOffset);
		pSecond->pos.Set(-vSecondOffset);

		// NOTE: 'm_TextMargins.iTag' is the padding between the panel/text (when set as side-by-side)
		if(m_uiPanelAttribs & PANELATTRIB_SideBySideVertical)
		{
			if(vFirstSize.x >= vSecondSize.x)
			{
				pFirst->pos.Offset(0.0f, vSecondSize.y + m_TextMargins.iTag);
				pSecond->pos.Offset((vFirstSize.x - vSecondSize.x) * 0.5f, 0.0f);
			}
			else
				pFirst->pos.Offset((vFirstSize.x - vSecondSize.x) * 0.5f, vSecondSize.y + m_TextMargins.iTag);
		}
		else // Horizontal side-by-side
		{
			if(vFirstSize.y >= vSecondSize.y)
				pSecond->pos.Offset(vFirstSize.x + m_TextMargins.iTag, (vFirstSize.y - vSecondSize.y) * 0.5f);
			else
			{
				pFirst->pos.Offset(0.0f, (vSecondSize.y - vFirstSize.y) * 0.5f);
				pSecond->pos.Offset(vFirstSize.x + m_TextMargins.iTag, 0.0f);
			}
		}
	}
	else // Stacked Panel/Text
	{
		HyAlignment eAlignment;
		if(0 == (m_uiPanelAttribs & (PANELATTRIB_StackedTextLeftAlign | PANELATTRIB_StackedTextRightAlign | PANELATTRIB_StackedTextJustifyAlign)))
			eAlignment = HYALIGN_HCenter;
		else if(m_uiPanelAttribs & PANELATTRIB_StackedTextLeftAlign)
			eAlignment = HYALIGN_Left;
		else if(m_uiPanelAttribs & PANELATTRIB_StackedTextRightAlign)
			eAlignment = HYALIGN_Right;
		else
			eAlignment = HYALIGN_Justify;
		m_Text.SetTextAlignment(eAlignment);

		glm::vec2 vPanelDimensions = GetPanelDimensions();
		glm::ivec2 vPanelOffset = GetPanelOffset();

		if(m_pPrimPanel)
			HySetVec(vPanelOffset, m_pPrimPanel->GetBorderSize(), m_pPrimPanel->GetBorderSize());

		// Position text
		auto vUiSizeHint = GetSizeHint();
		m_Text.pos.Set((m_TextMargins.left * (vPanelDimensions.x / vUiSizeHint.x)) - vPanelOffset.x,
					   (m_TextMargins.bottom * (vPanelDimensions.y / vUiSizeHint.y)) - vPanelOffset.y);

		// Size text
		if(vPanelDimensions.x != 0.0f && vPanelDimensions.y != 0.0f)
		{
			m_Text.SetAsScaleBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x)),
								 vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top) * (vPanelDimensions.y / vUiSizeHint.y)), true);
		}
	}

	m_bSizeHintDirty = true;
}

glm::vec2 HyLabel::GetPanelOffset()
{
	if(m_pPrimPanel)
		return glm::vec2(0.0f, 0.0f);
	else if(m_SpritePanel.IsLoadDataValid())
	{
		glm::vec2 vPanelDimensions = GetPanelDimensions();

		const HySprite2dData *pPanelData = static_cast<const HySprite2dData *>(m_SpritePanel.AcquireData());
		const HySprite2dFrame &frameRef = pPanelData->GetFrame(m_SpritePanel.GetState(), m_SpritePanel.GetFrame());

		auto vUiSizeHint = GetSizeHint();
		return -glm::vec2(frameRef.vOFFSET.x * (vPanelDimensions.x / vUiSizeHint.x), frameRef.vOFFSET.y * (vPanelDimensions.y / vUiSizeHint.y));
	}

	return glm::vec2(0.0f, 0.0f);
}
