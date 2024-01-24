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
#include "Assets/Nodes/HySpriteData.h"
#include "Diagnostics/Console/IHyConsole.h"

HyLabel::HyLabel(HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Panel(this)
{
	Setup(HyPanelInit(), "", "");
}

HyLabel::HyLabel(const HyPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Panel(this)
{
	Setup(initRef, "", "", 0, 0, 0, 0);
}

HyLabel::HyLabel(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Panel(this)
{
	Setup(initRef, sTextPrefix, sTextName, 0, 0, 0, 0);
}

HyLabel::HyLabel(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_Panel(this)
{
	Setup(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

/*virtual*/ HyLabel::~HyLabel()
{
}

/*virtual*/ float HyLabel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		if((m_uiAttribs & LABELATTRIB_SideBySideVertical) == 0)
			return (m_Panel.size.X() + (m_Text.GetWidth() * m_Text.scale.X()) + m_TextMargins.iTag) * fPercent;
		else
			return HyMath::Max(m_Panel.size.X(), m_Text.GetWidth() * m_Text.scale.X()) * fPercent;
	}
	else // Is stacked
	{
		if(m_Panel.IsValid())
			return m_Panel.size.X() * fPercent;
		else
			return m_Text.GetWidth(fPercent);
	}
}

/*virtual*/ float HyLabel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
			return (m_Panel.size.Y() + (m_Text.GetHeight() * m_Text.scale.Y()) + m_TextMargins.iTag) * fPercent;
		else
			return HyMath::Max(m_Panel.size.Y(), m_Text.GetHeight() * m_Text.scale.Y()) * fPercent;
	}
	else // Is stacked
	{
		if(m_Panel.IsValid())
			return m_Panel.size.Y() * fPercent;
		else
			return m_Text.GetHeight(fPercent);
	}
}

/*virtual*/ bool HyLabel::IsLoadDataValid() /*override*/
{
	return m_Panel.IsValid() || m_Text.IsLoadDataValid();
}

void HyLabel::Setup(const HyPanelInit &initRef)
{
	Setup(initRef, m_Text.GetPrefix(), m_Text.GetName(), 0, 0, 0, 0);
}

void HyLabel::Setup(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName)
{
	Setup(initRef, sTextPrefix, sTextName, 0, 0, 0, 0);
}

void HyLabel::Setup(const HyPanelInit &initRef, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	Setup(initRef, m_Text.GetPrefix(), m_Text.GetName(), iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop);
}

void HyLabel::Setup(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop)
{
	m_Panel.Setup(initRef, false);

	m_Text.Init(sTextPrefix, sTextName, this);
	m_TextMargins.Set(static_cast<float>(iTextMarginLeft),
					  static_cast<float>(iTextMarginBottom),
					  static_cast<float>(iTextMarginRight),
					  static_cast<float>(iTextMarginTop));

	SetAsEnabled(IsEnabled());

	ResetTextAndPanel();
	OnSetup();
}

/*virtual*/ void HyLabel::SetAsStacked(HyAlignment eTextAlignment /*= HYALIGN_Center*/, bool bUseScaleBox /*= true*/)
{
	m_uiAttribs &= ~LABELATTRIB_IsSideBySide;
	switch(eTextAlignment)
	{
	case HYALIGN_Left:
		m_uiAttribs &= ~(LABELATTRIB_StackedTextRightAlign | LABELATTRIB_StackedTextJustifyAlign);
		m_uiAttribs |= LABELATTRIB_StackedTextLeftAlign;
		break;
	case HYALIGN_Center:
		m_uiAttribs &= ~(LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextRightAlign | LABELATTRIB_StackedTextJustifyAlign);
		break;
	case HYALIGN_Right:
		m_uiAttribs &= ~(LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextJustifyAlign);
		m_uiAttribs |= LABELATTRIB_StackedTextRightAlign;
		break;
	case HYALIGN_Justify:
		m_uiAttribs &= ~(LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextRightAlign);
		m_uiAttribs |= LABELATTRIB_StackedTextJustifyAlign;
		break;
	}

	if(bUseScaleBox)
		m_uiAttribs &= ~LABELATTRIB_StackedTextUseLine;
	else
		m_uiAttribs |= LABELATTRIB_StackedTextUseLine;
	
	ResetTextAndPanel();
}

void HyLabel::SetAsSideBySide(bool bPanelBeforeText /*= true*/, int32 iPadding /*= 5*/, HyOrientation eOrientation /*= HYORIEN_Horizontal*/)
{
	m_uiAttribs |= LABELATTRIB_IsSideBySide;
	if(bPanelBeforeText)
		m_uiAttribs &= ~LABELATTRIB_SideBySideTextFirst;
	else
		m_uiAttribs |= LABELATTRIB_SideBySideTextFirst;

	m_TextMargins.iTag = iPadding;

	if(eOrientation == HYORIEN_Horizontal)
		m_uiAttribs &= ~LABELATTRIB_SideBySideVertical;
	else
		m_uiAttribs |= LABELATTRIB_SideBySideVertical;

	ResetTextAndPanel();
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

void HyLabel::SetSpriteAnimCtrl(HyAnimCtrl eAnimCtrl)
{
	m_Panel.GetSprite().SetAnimCtrl(eAnimCtrl);
	ResetTextAndPanel();
}

bool HyLabel::IsGlyphAvailable(std::string sUtf8Character)
{
	return m_Text.IsGlyphAvailable(sUtf8Character);
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
			glm::vec2 vOffset = m_Text.GetTextBottomLeft();
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
			m_vSizeHint.y = static_cast<int32>(vPanelSizeHint.y + m_TextMargins.iTag + m_Text.GetHeight());
		}
		else // Horizontal
		{
			m_vSizeHint.x = static_cast<int32>(vPanelSizeHint.x + m_TextMargins.iTag + m_Text.GetWidth());
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
			m_Panel.size.Set(static_cast<int32>(uiNewWidth), static_cast<int32>(uiNewHeight));
	}
	else // Side-by-side
	{
		glm::ivec2 vSizeHint = GetSizeHint();
		glm::ivec2 vPanelSizeHint = m_Panel.GetSizeHint();
		glm::ivec2 vTextSizeHint(m_Text.GetWidth(), m_Text.GetHeight());

		glm::ivec2 vNewPanelSize, vNewTextSize;
		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
		{
			vSizeHint.y -= m_TextMargins.iTag;

		}
		else // Horizontal
		{
			vSizeHint.x -= m_TextMargins.iTag;

			// Determine what % of size goes to panel/text
			float fPanelPerc = static_cast<float>(vPanelSizeHint.x) / static_cast<float>(vSizeHint.x);
			float fTextPerc = static_cast<float>(vTextSizeHint.x) / static_cast<float>(vSizeHint.x);

			vNewPanelSize = HyMath::LockAspectRatio(vPanelSizeHint.x, vPanelSizeHint.y, static_cast<int32>(uiNewWidth * fPanelPerc), uiNewHeight);
			vNewTextSize = HyMath::LockAspectRatio(vTextSizeHint.x, vTextSizeHint.y, static_cast<int32>(uiNewWidth * fTextPerc), uiNewHeight);
			//HySetVec(vNewTextSize, uiNewWidth * fTextPerc, vTextSizeHint.y);
		}

		m_Panel.size.Set(vNewPanelSize.x, vNewPanelSize.y);

		float fScaleX = static_cast<float>(vNewTextSize.x) / static_cast<float>(vTextSizeHint.x);
		float fScaleY = static_cast<float>(vNewTextSize.y) / static_cast<float>(vTextSizeHint.y);
		m_Text.scale.Set(HyMath::Min(fScaleX, fScaleY));
	}

	ResetTextAndPanel();
	
	return glm::ivec2(uiNewWidth, uiNewHeight);
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
				m_Text.SetTextAlignment(HYALIGN_Center);
			else
				m_Text.SetTextAlignment(HYALIGN_Right);

			pFirst = &m_Text;
			HySetVec(vFirstSize, m_Text.GetWidth() * m_Text.scale.X(), m_Text.GetLineBreakHeight(m_Text.scale.Y()));
			vFirstOffset = m_Text.GetTextBottomLeft();

			pSecond = &m_Panel;
			vSecondSize = m_Panel.size.Get();
			vSecondOffset = -m_Panel.GetBotLeftOffset();
		}
		else
		{
			if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
				m_Text.SetTextAlignment(HYALIGN_Center);
			else
				m_Text.SetTextAlignment(HYALIGN_Left);

			pFirst = &m_Panel;
			vFirstSize = m_Panel.size.Get();
			vFirstOffset = -m_Panel.GetBotLeftOffset();

			pSecond = &m_Text;
			HySetVec(vSecondSize, m_Text.GetWidth() * m_Text.scale.X(), m_Text.GetLineBreakHeight(m_Text.scale.Y()));
			vSecondOffset = m_Text.GetTextBottomLeft();
		}

		// Then position "first" and "second" appropriately
		pFirst->pos.Set(-vFirstOffset);
		pSecond->pos.Set(-vSecondOffset);

		// NOTE: 'm_TextMargins.iTag' is the padding between the panel/text (when set as side-by-side)
		if(m_uiAttribs & LABELATTRIB_SideBySideVertical)
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

		m_Text.pos.Offset(0.0f, -m_Text.GetLineDescender(m_Text.scale.GetY()));
	}
	else // Stacked Panel/Text
	{
		glm::ivec2 vUiSizeHint = GetSizeHint();
		glm::vec2 vPanelDimensions = m_Panel.size.Get();
		glm::ivec2 vPanelOffset = m_Panel.GetBotLeftOffset();

		// Position text to bottom left of 'm_TextMargins'
		m_Text.pos.Set(m_Panel.GetFrameStrokeSize() + ((m_TextMargins.left * (vPanelDimensions.x / vUiSizeHint.x)) - vPanelOffset.x),
					   m_Panel.GetFrameStrokeSize() + ((m_TextMargins.bottom * (vPanelDimensions.y / vUiSizeHint.y)) - vPanelOffset.y));

		HyAlignment eAlignment;
		float fLineOffsetX = 0.0f;	// If *this is 'LABELATTRIB_StackedTextUseLine' determine how much to offset m_Text's position (not needed for scale boxes)
		if(0 == (m_uiAttribs & (LABELATTRIB_StackedTextLeftAlign | LABELATTRIB_StackedTextRightAlign | LABELATTRIB_StackedTextJustifyAlign)))
		{
			eAlignment = HYALIGN_Center;
			fLineOffsetX = (vPanelDimensions.x * 0.5f) - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x));
		}
		else if(m_uiAttribs & LABELATTRIB_StackedTextLeftAlign)
			eAlignment = HYALIGN_Left;
		else if(m_uiAttribs & LABELATTRIB_StackedTextRightAlign)
		{
			eAlignment = HYALIGN_Right;
			fLineOffsetX = vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x));
		}
		else
			eAlignment = HYALIGN_Justify;
		m_Text.SetTextAlignment(eAlignment);

		// Size text
		if(vPanelDimensions.x != 0.0f && vPanelDimensions.y != 0.0f)
		{
			if(m_uiAttribs & LABELATTRIB_StackedTextUseLine)
			{
				m_Text.SetAsLine();

				float fLineOffsetY = 0.0f;
				float fVerticalSpace = vPanelDimensions.y - ((m_TextMargins.top + m_TextMargins.bottom) * (vPanelDimensions.y / vUiSizeHint.y)) - (m_Panel.GetFrameStrokeSize() * 2);
				float fTextHeight = m_Text.GetLineBreakHeight(m_Text.scale.Y());
				if(fVerticalSpace > fTextHeight)
					fLineOffsetY = (fVerticalSpace - fTextHeight) * 0.5f;
				m_Text.pos.Offset(fLineOffsetX, fLineOffsetY - m_Text.GetLineDescender(m_Text.scale.Y()));
			}
			else
				m_Text.SetAsScaleBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right) * (vPanelDimensions.x / vUiSizeHint.x)),
									 vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top) * (vPanelDimensions.y / vUiSizeHint.y)), true);
		}
		else
			m_Text.SetAsLine();
	}

	SetSizeAndLayoutDirty();
}
