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
	IHyWidget(pParent),
	m_eStackedAlignment(HYALIGN_Center),
	m_iSideBySidePadding(0)
{
	RegisterAssembleEntity();

	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_uiAttribs |= LABELATTRIB_Vertical;
}

HyLabel::HyLabel(const HyPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_eStackedAlignment(HYALIGN_Center),
	m_iSideBySidePadding(0)
{
	RegisterAssembleEntity();

	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_uiAttribs |= LABELATTRIB_Vertical;

	Setup(panelInit, HyNodePath(), HyMargins<float>());
}

HyLabel::HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_eStackedAlignment(HYALIGN_Center),
	m_iSideBySidePadding(0)
{
	RegisterAssembleEntity();

	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_uiAttribs |= LABELATTRIB_Vertical;

	Setup(panelInit, textNodePath, HyMargins<float>());
}

HyLabel::HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_eStackedAlignment(HYALIGN_Center),
	m_iSideBySidePadding(0)
{
	RegisterAssembleEntity();

	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_uiAttribs |= LABELATTRIB_Vertical;

	Setup(panelInit, textNodePath, textMargins);
}

/*virtual*/ HyLabel::~HyLabel()
{
}

/*virtual*/ float HyLabel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	Assemble();

	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		if((m_uiAttribs & LABELATTRIB_Vertical) == 0)
			return (m_Panel.GetWidth() + GetTextWidth() + m_iSideBySidePadding) * fPercent;
		else
			return HyMath::Max(m_Panel.GetWidth(), GetTextWidth()) * fPercent;
	}
	else // Is stacked
		return m_Panel.GetWidth() * fPercent;
}

/*virtual*/ float HyLabel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	Assemble();

	if(m_uiAttribs & LABELATTRIB_IsSideBySide)
	{
		if(m_uiAttribs & LABELATTRIB_Vertical)
			return (m_Panel.GetHeight() + GetTextHeight() + m_iSideBySidePadding) * fPercent;
		else
			return HyMath::Max(m_Panel.GetHeight(), GetTextHeight()) * fPercent;
	}
	else // Is stacked
		return m_Panel.GetHeight() * fPercent;
}

/*virtual*/ float HyLabel::GetTextWidth(float fPercent /*= 1.0f*/)
{
	Assemble();
	return m_Text.GetWidth(m_Text.scale.X()) * fPercent;
}

/*virtual*/ float HyLabel::GetTextHeight(float fPercent /*= 1.0f*/)
{
	Assemble();
	return m_Text.GetHeight(m_Text.scale.Y()) * fPercent;
}

/*virtual*/ bool HyLabel::IsLoadDataValid() /*override*/
{
	return (m_Panel.IsPrimitive() && m_Panel.IsAutoSize() == false) ||
		   (m_Panel.IsNode() && m_Panel.GetNode()->IsLoadDataValid()) ||
		   m_Text.IsLoadDataValid();
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

	SetAsEnabled(IsEnabled());

	switch(GetTextType())
	{
	case HYTEXT_Line:		SetAsLine(); break;
	case HYTEXT_Column:		SetAsColumn(GetWidth()); break;
	case HYTEXT_Box:		SetAsBox(GetWidth(), GetHeight(), m_uiAttribs & LABELATTRIB_Vertical, m_uiAttribs & LABELATTRIB_BoxUseScissorOrSbsTextFirst); break;
	case HYTEXT_ScaleBox:	SetAsScaleBox(GetWidth(), GetHeight(), m_uiAttribs & LABELATTRIB_Vertical); break;
	default:
		HyLogError("HyRichText::Setup() - Unhandled text type: " << GetTextType());
		break;
	}

	SetAssembleNeeded();
	OnSetup();
}

HyNodePath HyLabel::GetTextNodePath() const
{
	return m_Text.GetPath();
}

HyTextType HyLabel::GetTextType() const
{
	return static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsLine() const
{
	return IsSideBySide() == false && HYTEXT_Line == static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsColumn() const
{
	return IsSideBySide() == false && HYTEXT_Column == static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsBox() const
{
	return IsSideBySide() == false && HYTEXT_Box == static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsScaleBox() const
{
	return IsSideBySide() == false && HYTEXT_ScaleBox == static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsVertical() const
{
	return IsSideBySide() == false && HYTEXT_Vertical == static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsSideBySide() const
{
	return (m_uiAttribs & LABELATTRIB_IsSideBySide) != 0;
}

void HyLabel::SetAsLine()
{
	m_uiAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiAttribs |= (HYTEXT_Line << LABELATTRIB_StackedTextTypeOffset);

	SetAssembleNeeded();
}

void HyLabel::SetAsColumn()
{
	SetAsColumn(m_Panel.GetWidth());
}

void HyLabel::SetAsColumn(float fWidth)
{
	m_uiAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiAttribs |= (HYTEXT_Column << LABELATTRIB_StackedTextTypeOffset);

	if(m_Panel.GetWidth() != fWidth)
	{
		HyPanelInit panelInit = m_Panel.CloneInit();
		panelInit.m_uiWidth = fWidth;
		m_Panel.Setup(panelInit);
		//OnPanelUpdated();
	}

	SetAssembleNeeded();
}

void HyLabel::SetAsBox(bool bCenterVertically /*= false*/, bool bUseScissor /*= true*/)
{
	SetAsBox(m_Panel.GetWidth(), m_Panel.GetHeight(), bCenterVertically, bUseScissor);
}

void HyLabel::SetAsBox(float fWidth, float fHeight, bool bCenterVertically /*= false*/, bool bUseScissor /*= true*/)
{
	m_uiAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiAttribs |= (HYTEXT_Box << LABELATTRIB_StackedTextTypeOffset);

	if(bCenterVertically)
		m_uiAttribs |= LABELATTRIB_Vertical;
	else
		m_uiAttribs &= ~LABELATTRIB_Vertical;

	if(bUseScissor)
		m_uiAttribs |= LABELATTRIB_BoxUseScissorOrSbsTextFirst;
	else
		m_uiAttribs &= ~LABELATTRIB_BoxUseScissorOrSbsTextFirst;

	if(m_Panel.GetWidth() != fWidth || m_Panel.GetHeight() != fHeight)
	{
		HyPanelInit panelInit = m_Panel.CloneInit();
		panelInit.m_uiWidth = fWidth;
		panelInit.m_uiHeight = fHeight;
		m_Panel.Setup(panelInit);
	}

	SetAssembleNeeded();
}

void HyLabel::SetAsScaleBox(bool bCenterVertically /*= true*/)
{
	SetAsScaleBox(m_Panel.GetWidth(), m_Panel.GetHeight(), bCenterVertically);
}

void HyLabel::SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically /*= true*/)
{
	m_uiAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);

	if(bCenterVertically)
		m_uiAttribs |= LABELATTRIB_Vertical;
	else
		m_uiAttribs &= ~LABELATTRIB_Vertical;

	if(m_Panel.GetWidth() != fWidth || m_Panel.GetHeight() != fHeight)
	{
		HyPanelInit panelInit = m_Panel.CloneInit();
		panelInit.m_uiWidth = fWidth;
		panelInit.m_uiHeight = fHeight;
		m_Panel.Setup(panelInit);
	}

	SetAssembleNeeded();
}

void HyLabel::SetAsVertical()
{
	m_uiAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiAttribs |= (HYTEXT_Vertical << LABELATTRIB_StackedTextTypeOffset);

	SetAssembleNeeded();
}

void HyLabel::SetAsSideBySide(bool bPanelBeforeText /*= true*/, int32 iPadding /*= 5*/, HyOrientation eOrientation /*= HYORIENT_Horizontal*/)
{
	m_uiAttribs |= LABELATTRIB_IsSideBySide;
	if(bPanelBeforeText)
		m_uiAttribs &= ~LABELATTRIB_BoxUseScissorOrSbsTextFirst;
	else
		m_uiAttribs |= LABELATTRIB_BoxUseScissorOrSbsTextFirst;

	m_iSideBySidePadding = iPadding;

	if(eOrientation == HYORIENT_Horizontal)
		m_uiAttribs &= ~LABELATTRIB_Vertical;
	else
		m_uiAttribs |= LABELATTRIB_Vertical;

	SetAssembleNeeded();
}

HyAlignment HyLabel::GetAlignment() const
{
	return IsSideBySide() ? HYALIGN_Unknown : m_eStackedAlignment;
}

void HyLabel::SetAlignment(HyAlignment eAlignment)
{
	m_eStackedAlignment = eAlignment;
	SetAssembleNeeded();
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
	SetAssembleNeeded();
}

uint32 HyLabel::GetTextState() const
{
	return m_Text.GetState();
}

/*virtual*/ void HyLabel::SetTextState(uint32 uiStateIndex)
{
	m_Text.SetState(uiStateIndex);
	SetAssembleNeeded();
}

float HyLabel::GetLineBreakHeight(float fPercent /*= 1.0f*/)
{
	return m_Text.GetLineBreakHeight(fPercent);
}

float HyLabel::GetLineDescender(float fPercent /*= 1.0f*/)
{
	return m_Text.GetLineDescender(fPercent);
}

glm::vec2 HyLabel::GetGlyphOffset(uint32 uiCharIndex, uint32 uiLayerIndex)
{
	return m_Text.GetGlyphOffset(uiCharIndex, uiLayerIndex);
}

glm::vec2 HyLabel::GetGlyphSize(uint32 uiCharIndex, uint32 uiLayerIndex)
{
	return m_Text.GetGlyphSize(uiCharIndex, uiLayerIndex);
}

bool HyLabel::IsCharacterAvailable(uint32 uiStateIndex, const std::string sUtf8Character)
{
	return m_Text.IsCharacterAvailable(uiStateIndex, sUtf8Character);
}

uint32 HyLabel::GetNumCharacters() const
{
	return m_Text.GetNumCharacters();
}

uint32 HyLabel::GetCharacterCode(uint32 uiCharIndex) const
{
	return m_Text.GetCharacterCode(uiCharIndex);
}

glm::vec2 HyLabel::GetCharacterOffset(uint32 uiCharIndex)
{
	return m_Text.GetCharacterOffset(uiCharIndex);
}

void HyLabel::SetCharacterOffset(uint32 uiCharIndex, glm::vec2 vOffsetAmt)
{
	m_Text.SetCharacterOffset(uiCharIndex, vOffsetAmt);
}

float HyLabel::GetCharacterScale(uint32 uiCharIndex)
{
	return m_Text.GetCharacterScale(uiCharIndex);
}

void HyLabel::SetCharacterScale(uint32 uiCharIndex, float fScale)
{
	m_Text.SetCharacterScale(uiCharIndex, fScale);
}

float HyLabel::GetCharacterAlpha(uint32 uiCharIndex)
{
	return m_Text.GetCharacterAlpha(uiCharIndex);
}

void HyLabel::SetCharacterAlpha(uint32 uiCharIndex, float fAlpha)
{
	m_Text.SetCharacterAlpha(uiCharIndex, fAlpha);
}

uint32 HyLabel::GetNumLayers()
{
	return m_Text.GetNumLayers();
}

uint32 HyLabel::GetNumLayers(uint32 uiStateIndex)
{
	return m_Text.GetNumLayers(uiStateIndex);
}

std::pair<HyColor, HyColor> HyLabel::GetLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex)
{
	return m_Text.GetLayerColor(uiStateIndex, uiLayerIndex);
}

/*virtual*/ void HyLabel::SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor)
{
	m_Text.SetLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);
}

bool HyLabel::IsMonospacedDigits() const
{
	return m_Text.IsMonospacedDigits();
}

/*virtual*/ void HyLabel::SetMonospacedDigits(bool bSet)
{
	m_Text.SetMonospacedDigits(bSet);
	SetAssembleNeeded();
}

/*virtual*/ glm::vec2 HyLabel::GetPosOffset() /*override*/
{
	if((m_uiAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		Assemble();
		return m_Panel.GetBotLeftOffset();
	}

	return glm::vec2(0.0f, 0.0f);
}

/*virtual*/ void HyLabel::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, 0, 0);

	if((m_uiAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsAutoSize())
		{
			HySetVec(m_vSizeHint, static_cast<int32>(m_Text.GetWidth(1.0f) + m_TextMargins.left + m_TextMargins.right),
								  static_cast<int32>(m_Text.GetHeight(1.0f)) + m_TextMargins.top + m_TextMargins.bottom);
		}
		else
			m_vSizeHint = m_Panel.GetPanelSizeHint();
	}
	else // Side-by-side
	{
		glm::vec2 vPanelSizeHint = m_Panel.GetPanelSizeHint();
		if(m_uiAttribs & LABELATTRIB_Vertical)
		{
			m_vSizeHint.x = static_cast<int32>(HyMath::Max(vPanelSizeHint.x, GetTextWidth()));
			m_vSizeHint.y = static_cast<int32>(vPanelSizeHint.y + m_iSideBySidePadding + GetTextHeight());
		}
		else // Horizontal
		{
			m_vSizeHint.x = static_cast<int32>(vPanelSizeHint.x + m_iSideBySidePadding + GetTextWidth());
			m_vSizeHint.y = static_cast<int32>(HyMath::Max(vPanelSizeHint.y, GetTextHeight()));
		}
	}
}

/*virtual*/ void HyLabel::OnAssemble() /*override*/
{
	//SetSizeAndLayoutDirty();

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

		if(m_uiAttribs & LABELATTRIB_BoxUseScissorOrSbsTextFirst)
		{
			if(m_uiAttribs & LABELATTRIB_Vertical)
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
			if(m_uiAttribs & LABELATTRIB_Vertical)
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

		if(m_uiAttribs & LABELATTRIB_Vertical)
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
		m_Text.SetAlignment(m_eStackedAlignment);

		glm::vec2 vPanelDimensions(m_Panel.GetWidth(), m_Panel.GetHeight());
		glm::ivec2 vPanelOffset = m_Panel.GetBotLeftOffset();

		// Position text to bottom left of 'm_TextMargins'
		m_Text.pos.Set(m_Panel.GetFrameStrokeSize() + (m_TextMargins.left - vPanelOffset.x),
					   m_Panel.GetFrameStrokeSize() + (m_TextMargins.bottom - vPanelOffset.y));

		float fLineOffsetX = 0.0f;	// If *this is 'LABELATTRIB_StackedTextUseLine' determine how much to offset m_Text's position (not needed for scale boxes)
		if(m_eStackedAlignment == HYALIGN_Center)
			fLineOffsetX = (vPanelDimensions.x * 0.5f) - ((m_TextMargins.left + m_TextMargins.right));
		else if(m_eStackedAlignment == HYALIGN_Right)
			fLineOffsetX = vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right));

		// Set text type/size
		HyTextType eStackedTextType = static_cast<HyTextType>((m_uiAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);

		if(vPanelDimensions.x != 0.0f && vPanelDimensions.y != 0.0f)
		{
			switch(eStackedTextType)
			{
			case HYTEXT_Line: {
				m_Text.SetAsLine();
				float fLineOffsetY = 0.0f;
				float fVerticalSpace = vPanelDimensions.y - ((m_TextMargins.top + m_TextMargins.bottom)) - (m_Panel.GetFrameStrokeSize() * 2);
				float fTextHeight = m_Text.GetLineBreakHeight(m_Text.scale.Y());
				if(fVerticalSpace > fTextHeight)
					fLineOffsetY = (fVerticalSpace - fTextHeight) * 0.5f;
				m_Text.pos.Offset(fLineOffsetX, fLineOffsetY - m_Text.GetLineDescender(m_Text.scale.Y()));
				break; }

			case HYTEXT_ScaleBox:
				m_Text.SetAsScaleBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right)) - (m_Panel.GetFrameStrokeSize() * 2),
					vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top)) - (m_Panel.GetFrameStrokeSize() * 2), true);
				break;

			case HYTEXT_Box:
				m_Text.SetAsBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right)) - (m_Panel.GetFrameStrokeSize() * 2),
					vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top)) - (m_Panel.GetFrameStrokeSize() * 2), true);
				break;

			case HYTEXT_Column:
				m_Text.SetAsColumn(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right)) - (m_Panel.GetFrameStrokeSize() * 2));
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

	// Inform everwhere that *this has been updated
	SetDirty(IHyNode::DIRTY_SceneAABB);
	//SetSizeAndLayoutDirty();
}

/*virtual*/ glm::ivec2 HyLabel::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	if((m_uiAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Panel.IsAutoSize() && m_Text.GetUtf8String().empty() == false)
		{
			glm::ivec2 vTextSizeHint = GetSizeHint();
			if((vTextSizeHint.x != uiNewWidth || vTextSizeHint.y != uiNewHeight) &&
			   (vTextSizeHint.x != 0.0f && vTextSizeHint.y != 0.0f))
			{
				float fScaleX = uiNewWidth / vTextSizeHint.x;
				float fScaleY = uiNewHeight / vTextSizeHint.y;
				m_Text.scale.SetAll(HyMath::Min(fScaleX, fScaleY));

				uiNewWidth = static_cast<uint32>(GetTextWidth());
				uiNewHeight = static_cast<uint32>(GetTextHeight());
			}
		}
		
		m_Panel.SetSize(uiNewWidth, uiNewHeight);
	}
	else // Side-by-side
	{
		glm::ivec2 vSizeHint = GetSizeHint();
		glm::ivec2 vPanelSizeHint = m_Panel.GetPanelSizeHint();
		glm::ivec2 vTextSizeHint(GetTextWidth(), GetTextHeight());

		glm::ivec2 vNewPanelSize, vNewTextSize;
		if(m_uiAttribs & LABELATTRIB_Vertical)
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
		m_Text.scale.SetAll(HyMath::Min(fScaleX, fScaleY));
	}

	SetAssembleNeeded();
	
	return glm::ivec2(uiNewWidth, uiNewHeight);
}

/*virtual*/ void HyLabel::OnPanelUpdated() /*override*/
{
	SetAssembleNeeded();
}
