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
	m_uiLabelAttribs(0),
	m_Panel(this)
{
	Setup(HyPanelInit(), "", "");
}

HyLabel::HyLabel(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiLabelAttribs(0),
	m_Panel(this)
{
	Setup(initRef, sTextPrefix, sTextName, 0, 0, 0, 0);
}

HyLabel::HyLabel(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	IHyEntityUi(pParent),
	m_uiLabelAttribs(0),
	m_Panel(this)
{
	Setup(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

/*virtual*/ HyLabel::~HyLabel()
{
}

void HyLabel::Setup(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName)
{
	Setup(initRef, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	m_Panel.Setup(initRef);

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
	m_uiLabelAttribs &= ~LABELATTRIB_IsSideBySide;
	switch(eTextAlignment)
	{
	case HYALIGN_Left:
		m_uiLabelAttribs &= ~(LABELATTRIB_StackedTextRightAlign | LABELATTRIB_StackedTextJustifyAlign);
		m_uiLabelAttribs |= LABELATTRIB_StackedTextLeftAlign;
		break;
	case HYALIGN_HCenter:
		m_uiLabelAttribs &= ~(LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextRightAlign | LABELATTRIB_StackedTextJustifyAlign);
		break;
	case HYALIGN_Right:
		m_uiLabelAttribs &= ~(LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextJustifyAlign);
		m_uiLabelAttribs |= LABELATTRIB_StackedTextRightAlign;
		break;
	case HYALIGN_Justify:
		m_uiLabelAttribs &= ~(LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextRightAlign);
		m_uiLabelAttribs |= LABELATTRIB_StackedTextJustifyAlign;
		break;
	}
	
	ResetTextAndPanel();
}

void HyLabel::SetAsSideBySide(bool bPanelBeforeText /*= true*/, int32 iPadding /*= 5*/, HyOrientation eOrientation /*= HYORIEN_Horizontal*/)
{
	m_uiLabelAttribs |= LABELATTRIB_IsSideBySide;
	if(bPanelBeforeText)
		m_uiLabelAttribs &= ~LABELATTRIB_SideBySideTextFirst;
	else
		m_uiLabelAttribs |= LABELATTRIB_SideBySideTextFirst;

	m_TextMargins.iTag = iPadding;

	if(eOrientation == HYORIEN_Horizontal)
		m_uiLabelAttribs &= ~LABELATTRIB_SideBySideVertical;
	else
		m_uiLabelAttribs |= LABELATTRIB_SideBySideVertical;

	ResetTextAndPanel();
}

float HyLabel::GetPanelWidth()
{
	return m_Panel.GetWidth();
}

float HyLabel::GetPanelHeight()
{
	return m_Panel.GetHeight();
}

glm::vec2 HyLabel::GetPanelSize()
{
	return glm::vec2(GetPanelWidth(), GetPanelHeight());
}

uint32 HyLabel::GetSpriteState() const
{
	return m_Panel.GetSpriteState();
}

void HyLabel::SetPanelVisible(bool bVisible)
{
	m_Panel.SetVisible(bVisible);
}

/*virtual*/ void HyLabel::SetSpriteState(uint32 uiStateIndex)
{
	m_Panel.SetSpriteState(uiStateIndex);
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
	return m_Panel.IsPrimitive();
}

bool HyLabel::IsEnabled() const
{
	return (m_uiLabelAttribs & LABELATTRIB_IsDisabled) == 0;
}

/*virtual*/ void HyLabel::SetAsEnabled(bool bEnabled)
{
	if(bEnabled)
	{
		m_uiLabelAttribs &= ~LABELATTRIB_IsDisabled;
		topColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);
		botColor.Tween(1.0f, 1.0f, 1.0f, 0.25f);

		if(IsMouseInBounds())
			OnMouseEnter();
	}
	else
	{
		m_uiLabelAttribs |= LABELATTRIB_IsDisabled;

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
	return (m_uiLabelAttribs & LABELATTRIB_IsHighlighted) != 0;
}

/*virtual*/ void HyLabel::SetAsHighlighted(bool bIsHighlighted)
{
	if(bIsHighlighted == IsHighlighted())
		return;

	if(bIsHighlighted)
	{
		m_uiLabelAttribs |= LABELATTRIB_IsHighlighted;
		m_Panel.SetBorderColor(0x0000FF);
	}
	else
	{
		m_uiLabelAttribs &= ~LABELATTRIB_IsHighlighted;
		m_Panel.SetBorderColor(0x3F3F41);
	}
}

bool HyLabel::IsShowHandCursor() const
{
	return (m_uiLabelAttribs & LABELATTRIB_ShowHandCursor) != 0;
}

void HyLabel::SetShowHandCursor(bool bShowHandCursor)
{
	if(bShowHandCursor)
		m_uiLabelAttribs |= LABELATTRIB_ShowHandCursor;
	else
		m_uiLabelAttribs &= ~LABELATTRIB_ShowHandCursor;
}

bool HyLabel::IsHideDisabled() const
{
	return (m_uiLabelAttribs & LABELATTRIB_HideDisabled) != 0;
}

void HyLabel::SetHideDisabled(bool bIsHideDisabled)
{
	if(bIsHideDisabled)
	{
		m_uiLabelAttribs |= LABELATTRIB_HideDisabled;
		SetTint(1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_uiLabelAttribs &= ~LABELATTRIB_HideDisabled;
		SetAsEnabled(IsEnabled());
	}
}

HySprite2d &HyLabel::GetSpriteNode()
{
	return m_Panel.GetSprite();
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
	if((m_uiLabelAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsValid())
			return m_Panel.GetBotLeftOffset();
		else if(m_Text.IsLoadDataValid())
			return -m_Text.GetTextBottomLeft();
	}

	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ void HyLabel::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, 0, 0);

	if((m_uiLabelAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsValid())
			m_vSizeHint = m_Panel.GetSizeHint();
		else if(m_Text.IsLoadDataValid())
			HySetVec(m_vSizeHint, static_cast<int32>(m_Text.GetTextWidth(false)), static_cast<int32>(m_Text.GetTextHeight(false)));
	}
	else // Side-by-side
	{
		glm::vec2 vPanelSizeHint = m_Panel.GetSizeHint();
		if(m_uiLabelAttribs & LABELATTRIB_SideBySideVertical)
		{
			m_vSizeHint.x = HyMax(vPanelSizeHint.x, m_Text.GetTextWidth(false));
			m_vSizeHint.y = vPanelSizeHint.y + m_TextMargins.iTag + m_Text.GetTextHeight(false);
		}
		else // Horizontal
		{
			m_vSizeHint.x = vPanelSizeHint.x + m_TextMargins.iTag + m_Text.GetTextWidth(false);
			m_vSizeHint.y = HyMax(vPanelSizeHint.y, m_Text.GetTextHeight(false));
		}
	}
}

/*virtual*/ glm::ivec2 HyLabel::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	if((m_uiLabelAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsValid())
		{
			m_Panel.SetSize(uiNewWidth, uiNewHeight);
		}
		else if(m_Text.IsLoadDataValid())
		{
			glm::vec2 vTextSize(m_Text.GetTextWidth(false), m_Text.GetTextHeight(false));

			float fScaleX = uiNewWidth / vTextSize.x;
			float fScaleY = uiNewHeight / vTextSize.y;
			m_Text.scale.Set(HyMin(fScaleX, fScaleY));

			uiNewWidth = m_Text.GetTextWidth(true);
			uiNewHeight = m_Text.GetTextHeight(true);
		}
	}
	else // Side-by-side
	{
		glm::ivec2 vSizeHint = GetSizeHint();
		glm::ivec2 vPanelSizeHint = m_Panel.GetSizeHint();
		glm::ivec2 vTextSizeHint(m_Text.GetTextWidth(false), m_Text.GetTextHeight(false));

		glm::ivec2 vNewPanelSize, vNewTextSize;
		if(m_uiLabelAttribs & LABELATTRIB_SideBySideVertical)
		{
			vSizeHint.y -= m_TextMargins.iTag;

		}
		else // Horizontal
		{
			vSizeHint.x -= m_TextMargins.iTag;

			// Determine what % of size goes to panel/text
			float fPanelPerc = static_cast<float>(vPanelSizeHint.x) / static_cast<float>(vSizeHint.x);
			float fTextPerc = static_cast<float>(vTextSizeHint.x) / static_cast<float>(vSizeHint.x);

			vNewPanelSize = HyMath::LockAspectRatio(vPanelSizeHint.x, vPanelSizeHint.y, uiNewWidth * fPanelPerc, uiNewHeight);
			vNewTextSize = HyMath::LockAspectRatio(vTextSizeHint.x, vTextSizeHint.y, uiNewWidth * fTextPerc, uiNewHeight);
			//HySetVec(vNewTextSize, uiNewWidth * fTextPerc, vTextSizeHint.y);
		}

		if(m_Panel.IsValid())
			m_Panel.SetSize(vNewPanelSize.x, vNewPanelSize.y);

		float fScaleX = static_cast<float>(vNewTextSize.x) / static_cast<float>(vTextSizeHint.x);
		float fScaleY = static_cast<float>(vNewTextSize.y) / static_cast<float>(vTextSizeHint.y);
		m_Text.scale.Set(HyMin(fScaleX, fScaleY));
	}

	ResetTextAndPanel();
	
	return glm::ivec2(uiNewWidth, uiNewHeight);
}

/*virtual*/ void HyLabel::ResetTextAndPanel()
{
	if(m_uiLabelAttribs & LABELATTRIB_IsSideBySide)
	{
		m_Text.SetAsLine();

		// Determine "first" and "second" size and offset
		IHyBody2d *pFirst = nullptr;
		glm::vec2 vFirstSize;
		glm::vec2 vFirstOffset;

		IHyBody2d *pSecond = nullptr;
		glm::vec2 vSecondSize;
		glm::vec2 vSecondOffset;

		if(m_uiLabelAttribs & LABELATTRIB_SideBySideTextFirst)
		{
			if(m_uiLabelAttribs & LABELATTRIB_SideBySideVertical)
				m_Text.SetTextAlignment(HYALIGN_HCenter);
			else
				m_Text.SetTextAlignment(HYALIGN_Right);

			pFirst = &m_Text;
			HySetVec(vFirstSize, m_Text.GetTextWidth(true), m_Text.GetTextHeight(true));
			vFirstOffset = m_Text.GetTextBottomLeft();

			pSecond = &m_Panel;
			HySetVec(vSecondSize, GetPanelWidth(), GetPanelHeight());
			vSecondOffset = -m_Panel.GetBotLeftOffset();
		}
		else
		{
			if(m_uiLabelAttribs & LABELATTRIB_SideBySideVertical)
				m_Text.SetTextAlignment(HYALIGN_HCenter);
			else
				m_Text.SetTextAlignment(HYALIGN_Left);

			pFirst = &m_Panel;
			HySetVec(vFirstSize, GetPanelWidth(), GetPanelHeight());
			vFirstOffset = -m_Panel.GetBotLeftOffset();

			pSecond = &m_Text;
			HySetVec(vSecondSize, m_Text.GetTextWidth(true), m_Text.GetTextHeight(true));
			vSecondOffset = m_Text.GetTextBottomLeft();
		}

		// Then position "first" and "second" appropriately
		pFirst->pos.Set(-vFirstOffset);
		pSecond->pos.Set(-vSecondOffset);

		// NOTE: 'm_TextMargins.iTag' is the padding between the panel/text (when set as side-by-side)
		if(m_uiLabelAttribs & LABELATTRIB_SideBySideVertical)
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
		if(0 == (m_uiLabelAttribs & (LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextRightAlign | LABELATTRIB_StackedTextJustifyAlign)))
			eAlignment = HYALIGN_HCenter;
		else if(m_uiLabelAttribs & LABELATTRIB_StackedTextLeftAlign)
			eAlignment = HYALIGN_Left;
		else if(m_uiLabelAttribs & LABELATTRIB_StackedTextRightAlign)
			eAlignment = HYALIGN_Right;
		else
			eAlignment = HYALIGN_Justify;
		m_Text.SetTextAlignment(eAlignment);

		glm::vec2 vPanelDimensions = m_Panel.GetSize();
		glm::ivec2 vPanelOffset = m_Panel.GetBotLeftOffset();

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
