/**************************************************************************
*	HyLabel.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "Diagnostics/Console/IHyConsole.h"

HyLabel::HyLabel(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent)
{
	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_Text.SetAlignment(HYALIGN_Center);
}

HyLabel::HyLabel(const HyPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent)
{
	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_Text.SetAlignment(HYALIGN_Center);
	Setup(panelInit, HyNodePath(), HyMargins<float>());
}

HyLabel::HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent)
{
	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_Text.SetAlignment(HYALIGN_Center);
	Setup(panelInit, textNodePath, HyMargins<float>());
}

HyLabel::HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent)
{
	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_Text.SetAlignment(HYALIGN_Center);
	Setup(panelInit, textNodePath, textMargins);
}

/*virtual*/ HyLabel::~HyLabel()
{
}

/*virtual*/ float HyLabel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		if((m_uiAttribs & LABELATTRIB_SideBySideVertical) == 0)
			return (m_Panel.GetWidth(m_Panel.scale.X()) + m_Text.GetWidth(m_Text.scale.X()) + m_iSideBySidePadding) * fPercent;
		else
			return HyMath::Max(m_Panel.GetWidth(m_Panel.scale.X()), m_Text.GetWidth(m_Text.scale.X())) * fPercent;
	}
	else // Is stacked
	{
		if(m_Panel.IsValid())
			return m_Panel.GetWidth(m_Panel.scale.X()) * fPercent;
		else
			return m_Text.GetWidth(m_Text.scale.X()) * fPercent;
	}
}

/*virtual*/ float HyLabel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
			return (m_Panel.GetHeight(m_Panel.scale.Y()) + m_Text.GetHeight(m_Text.scale.Y()) + m_iSideBySidePadding) * fPercent;
		else
			return HyMath::Max(m_Panel.GetHeight(m_Panel.scale.Y()), m_Text.GetHeight(m_Text.scale.Y())) * fPercent;
	}
	else // Is stacked
	{
		if(m_Panel.IsValid())
			return m_Panel.GetHeight(m_Panel.scale.Y()) * fPercent;
		else
			return m_Text.GetHeight(m_Text.scale.Y()) * fPercent;
	}
}

/*virtual*/ bool HyLabel::IsLoadDataValid() /*override*/
{
	return m_Panel.IsValid() || m_Text.IsLoadDataValid();
}

void HyLabel::Setup(const HyPanelInit &panelInit)
{
	Setup(panelInit, m_Text.GetPath(), m_TextMargins);
}

void HyLabel::Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath)
{
	Setup(panelInit, textNodePath, m_TextMargins);
}

void HyLabel::Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins)
{
	m_Panel.Setup(panelInit);
	m_Text.Init(textNodePath, this);
	m_TextMargins = textMargins;

	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
			SetAsSideBySide((m_uiAttribs & LABELATTRIB_SideBySideTextFirst) == 0, m_iSideBySidePadding, HYORIENT_Vertical);
		else
			SetAsSideBySide((m_uiAttribs & LABELATTRIB_SideBySideTextFirst) == 0, m_iSideBySidePadding, HYORIENT_Horizontal);
	}
	else
		SetAsStacked(m_Text.GetAlignment(), static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset));

	SetAsEnabled(IsEnabled());
	ResetTextAndPanel();
	OnSetup();
}

bool HyLabel::IsSideBySide() const
{
	return (m_uiAttribs & LABELATTRIB_IsSideBySide) != 0;
}

/*virtual*/ void HyLabel::SetAsStacked(HyAlignment eTextAlignment /*= HYALIGN_Center*/, HyTextType eTextType /*= HYTEXT_ScaleBox*/)
{
	m_uiAttribs &= ~LABELATTRIB_IsSideBySide;
	m_iSideBySidePadding = 0;

	m_Text.SetAlignment(eTextAlignment);
	m_uiAttribs &= ~LABELATTRIB_StackedTextTypeMask;					// Clear the existing stacked text type bits
	m_uiAttribs |= (eTextType << LABELATTRIB_StackedTextTypeOffset);	// Set the new stacked text type bits in the proper location
	
	ResetTextAndPanel();
}

void HyLabel::SetAsSideBySide(bool bPanelBeforeText /*= true*/, int32 iPadding /*= 5*/, HyOrientation eOrientation /*= HYORIENT_Horizontal*/)
{
	m_uiAttribs |= LABELATTRIB_IsSideBySide;
	if(bPanelBeforeText)
		m_uiAttribs &= ~LABELATTRIB_SideBySideTextFirst;
	else
		m_uiAttribs |= LABELATTRIB_SideBySideTextFirst;

	m_iSideBySidePadding = iPadding;

	if(eOrientation == HYORIENT_Horizontal)
		m_uiAttribs &= ~LABELATTRIB_SideBySideVertical;
	else
		m_uiAttribs |= LABELATTRIB_SideBySideVertical;

	ResetTextAndPanel();
}

bool HyLabel::IsTextVisible() const
{
	return m_Text.IsVisible();
}

void HyLabel::SetTextVisible(bool bVisible)
{
	m_Text.SetVisible(bVisible);
}

/*virtual*/ std::string HyLabel::GetUtf8String() const
{
	return m_Text.GetUtf8String();
}

void HyLabel::SetText(const std::stringstream &ssUtf8Text)
{
	SetText(ssUtf8Text.str());
}

/*virtual*/ void HyLabel::SetText(const std::string &sUtf8Text)
{
	m_Text.SetText(sUtf8Text);
	ResetTextAndPanel();
}

uint32 HyLabel::GetTextState() const
{
	return m_Text.GetState();
}

/*virtual*/ void HyLabel::SetTextState(uint32 uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
	ResetTextAndPanel();
}

std::string HyLabel::GetTextPrefix() const
{
	return m_Text.GetPrefix();
}

std::string HyLabel::GetTextName() const
{
	return m_Text.GetName();
}

/*virtual*/ void HyLabel::SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor)
{
	m_Text.SetLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);
}

bool HyLabel::IsTextMonospacedDigits() const
{
	return m_Text.IsMonospacedDigits();
}

/*virtual*/ void HyLabel::SetTextMonospacedDigits(bool bSet)
{
	m_Text.SetMonospacedDigits(bSet);
	ResetTextAndPanel();
}

bool HyLabel::IsGlyphAvailable(std::string sUtf8Character)
{
	return m_Text.IsGlyphAvailable(sUtf8Character);
}

float HyLabel::GetTextWidth(float fPercent /*= 1.0f*/)
{
	return m_Text.GetWidth(fPercent);
}

float HyLabel::GetTextHeight(float fPercent /*= 1.0f*/)
{
	return m_Text.GetHeight(fPercent);
}

/*virtual*/ glm::vec2 HyLabel::GetPosOffset() /*override*/
{
	if((m_uiAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsValid() == false)
		{
			glm::vec2 vOffset = m_Text.GetBottomLeft();
			vOffset *= m_Text.scale.Get();
			return -vOffset;
		}
		else
			return m_Panel.GetBotLeftOffset();
	}

	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ void HyLabel::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, 0, 0);

	if((m_uiAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsValid() == false)
			HySetVec(m_vSizeHint, static_cast<int32>(m_Text.GetWidth()), static_cast<int32>(m_Text.GetHeight()));
		else
			m_vSizeHint = m_Panel.GetSizeHint();
	}
	else // Side-by-side
	{
		glm::vec2 vPanelSizeHint = m_Panel.GetSizeHint();
		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
		{
			m_vSizeHint.x = static_cast<int32>(HyMath::Max(vPanelSizeHint.x, m_Text.GetWidth()));
			m_vSizeHint.y = static_cast<int32>(vPanelSizeHint.y + m_iSideBySidePadding + m_Text.GetHeight());
		}
		else // Horizontal
		{
			m_vSizeHint.x = static_cast<int32>(vPanelSizeHint.x + m_iSideBySidePadding + m_Text.GetWidth());
			m_vSizeHint.y = static_cast<int32>(HyMath::Max(vPanelSizeHint.y, m_Text.GetHeight()));
		}
	}
}

/*virtual*/ glm::ivec2 HyLabel::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	if((m_uiAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsValid() == false)
		{
			glm::vec2 vTextSize(m_Text.GetWidth(), m_Text.GetHeight());

			float fScaleX = uiNewWidth / vTextSize.x;
			float fScaleY = uiNewHeight / vTextSize.y;
			m_Text.scale.Set(HyMath::Min(fScaleX, fScaleY));

			uiNewWidth = static_cast<uint32>(m_Text.GetWidth(m_Text.scale.X()));
			uiNewHeight = static_cast<uint32>(m_Text.GetHeight(m_Text.scale.Y()));
		}
		else
			m_Panel.SetSize(uiNewWidth, uiNewHeight);
	}
	else // Side-by-side
	{
		glm::ivec2 vSizeHint = GetSizeHint();
		glm::ivec2 vPanelSizeHint = m_Panel.GetSizeHint();
		glm::ivec2 vTextSizeHint(m_Text.GetWidth(), m_Text.GetHeight());

		glm::ivec2 vNewPanelSize, vNewTextSize;
		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
		{
			vSizeHint.y -= m_iSideBySidePadding;

		}
		else // Horizontal
		{
			vSizeHint.x -= m_iSideBySidePadding;

			// Determine what % of size goes to panel/text
			float fPanelPerc = static_cast<float>(vPanelSizeHint.x) / static_cast<float>(vSizeHint.x);
			float fTextPerc = static_cast<float>(vTextSizeHint.x) / static_cast<float>(vSizeHint.x);

			vNewPanelSize = HyMath::LockAspectRatio(vPanelSizeHint.x, vPanelSizeHint.y, static_cast<int32>(uiNewWidth * fPanelPerc), uiNewHeight);
			vNewTextSize = HyMath::LockAspectRatio(vTextSizeHint.x, vTextSizeHint.y, static_cast<int32>(uiNewWidth * fTextPerc), uiNewHeight);
			//HySetVec(vNewTextSize, uiNewWidth * fTextPerc, vTextSizeHint.y);
		}

		m_Panel.SetSize(vNewPanelSize.x, vNewPanelSize.y);

		float fScaleX = static_cast<float>(vNewTextSize.x) / static_cast<float>(vTextSizeHint.x);
		float fScaleY = static_cast<float>(vNewTextSize.y) / static_cast<float>(vTextSizeHint.y);
		m_Text.scale.Set(HyMath::Min(fScaleX, fScaleY));
	}

	ResetTextAndPanel();
	
	return glm::ivec2(uiNewWidth, uiNewHeight);
}

/*virtual*/ void HyLabel::OnPanelUpdated() /*override*/
{
	ResetTextAndPanel();
}

/*virtual*/ void HyLabel::ResetTextAndPanel()
{
	SetSizeAndLayoutDirty();

	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		m_Text.SetAsLine();

		// Determine "first" and "second" size and offset
		IHyBody2d *pFirst = nullptr;
		glm::vec2 vFirstSize;
		glm::vec2 vFirstOffset;

		IHyBody2d *pSecond = nullptr;
		glm::vec2 vSecondSize;
		glm::vec2 vSecondOffset;

		if(m_uiAttribs & LABELATTRIB_SideBySideTextFirst)
		{
			if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
				m_Text.SetAlignment(HYALIGN_Center);
			else
				m_Text.SetAlignment(HYALIGN_Right);

			pFirst = &m_Text;
			HySetVec(vFirstSize, m_Text.GetWidth(m_Text.scale.X()), m_Text.GetLineBreakHeight(m_Text.scale.Y()));
			vFirstOffset = m_Text.GetBottomLeft();

			pSecond = &m_Panel;
			HySetVec(vSecondSize, m_Panel.GetWidth(m_Panel.scale.GetX()), m_Panel.GetHeight(m_Panel.scale.GetY()));
			vSecondOffset = -m_Panel.GetBotLeftOffset();
		}
		else
		{
			if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
				m_Text.SetAlignment(HYALIGN_Center);
			else
				m_Text.SetAlignment(HYALIGN_Left);

			pFirst = &m_Panel;
			HySetVec(vFirstSize, m_Panel.GetWidth(m_Panel.scale.GetX()), m_Panel.GetHeight(m_Panel.scale.GetY()));
			vFirstOffset = -m_Panel.GetBotLeftOffset();

			pSecond = &m_Text;
			HySetVec(vSecondSize, m_Text.GetWidth(m_Text.scale.X()), m_Text.GetLineBreakHeight(m_Text.scale.Y()));
			vSecondOffset = m_Text.GetBottomLeft();
		}

		// Then position "first" and "second" appropriately
		pFirst->pos.Set(-vFirstOffset);
		pSecond->pos.Set(-vSecondOffset);

		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
		{
			if(vFirstSize.x >= vSecondSize.x)
			{
				pFirst->pos.Offset(0.0f, vSecondSize.y + m_iSideBySidePadding);
				pSecond->pos.Offset((vFirstSize.x - vSecondSize.x) * 0.5f, 0.0f);
			}
			else
				pFirst->pos.Offset((vFirstSize.x - vSecondSize.x) * 0.5f, vSecondSize.y + m_iSideBySidePadding);
		}
		else // Horizontal side-by-side
		{
			if(vFirstSize.y >= vSecondSize.y)
				pSecond->pos.Offset(vFirstSize.x + m_iSideBySidePadding, (vFirstSize.y - vSecondSize.y) * 0.5f);
			else
			{
				pFirst->pos.Offset(0.0f, (vSecondSize.y - vFirstSize.y) * 0.5f);
				pSecond->pos.Offset(vFirstSize.x + m_iSideBySidePadding, 0.0f);
			}
		}

		m_Text.pos.Offset(0.0f, -m_Text.GetLineDescender(m_Text.scale.GetY()));
	}
	else // Stacked Panel/Text
	{
		glm::ivec2 vUiSizeHint = GetSizeHint();
		glm::vec2 vPanelDimensions(m_Panel.GetWidth(m_Panel.scale.GetX()), m_Panel.GetHeight(m_Panel.scale.GetY()));
		glm::ivec2 vPanelOffset = m_Panel.GetBotLeftOffset();

		// Position text to bottom left of 'm_TextMargins'
		m_Text.pos.Set(m_Panel.GetFrameStrokeSize() + ((m_TextMargins.left * (vPanelDimensions.x / vUiSizeHint.x)) - vPanelOffset.x),
					   m_Panel.GetFrameStrokeSize() + ((m_TextMargins.bottom * (vPanelDimensions.y / vUiSizeHint.y)) - vPanelOffset.y));

		
		float fLineOffsetX = 0.0f;	// If *this is 'LABELATTRIB_StackedTextUseLine' determine how much to offset m_Text's position (not needed for scale boxes)
		if(m_Text.GetAlignment() == HYALIGN_Center)
			fLineOffsetX = (vPanelDimensions.x * 0.5f) - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x));
		else if(m_Text.GetAlignment() == HYALIGN_Right)
			fLineOffsetX = vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x));

		// Set text type/size
		HyTextType eStackedTextType = static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);

		if(vPanelDimensions.x != 0.0f && vPanelDimensions.y != 0.0f)
		{
			switch(eStackedTextType)
			{
			case HYTEXT_Line: {
				m_Text.SetAsLine();
				float fLineOffsetY = 0.0f;
				float fVerticalSpace = vPanelDimensions.y - ((m_TextMargins.top + m_TextMargins.bottom) * (vPanelDimensions.y / vUiSizeHint.y)) - (m_Panel.GetFrameStrokeSize() * 2);
				float fTextHeight = m_Text.GetLineBreakHeight(m_Text.scale.Y());
				if(fVerticalSpace > fTextHeight)
					fLineOffsetY = (fVerticalSpace - fTextHeight) * 0.5f;
				m_Text.pos.Offset(fLineOffsetX, fLineOffsetY - m_Text.GetLineDescender(m_Text.scale.Y()));
				break; }

			case HYTEXT_ScaleBox:
				m_Text.SetAsScaleBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x)) - (m_Panel.GetFrameStrokeSize() * 2),
									 vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top) * (vPanelDimensions.y / vUiSizeHint.y)) - (m_Panel.GetFrameStrokeSize() * 2), true);
				break;

			case HYTEXT_Box:
				m_Text.SetAsBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x)) - (m_Panel.GetFrameStrokeSize() * 2),
								vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top) * (vPanelDimensions.y / vUiSizeHint.y)) - (m_Panel.GetFrameStrokeSize() * 2), true);
				break;

			case HYTEXT_Column:
				m_Text.SetAsColumn(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x)) - (m_Panel.GetFrameStrokeSize() * 2));
				break;

			case HYTEXT_Vertical:
				m_Text.SetAsVertical();
				break;

			default:
				HyLogError("HyLabel::ResetTextAndPanel - Unknown text type");
				break;
			}
		}
		else if(eStackedTextType == HYTEXT_Vertical)
			m_Text.SetAsVertical();
		else
			m_Text.SetAsLine();
	}

	SetSizeAndLayoutDirty();
}
