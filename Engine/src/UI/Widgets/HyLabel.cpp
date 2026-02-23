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

	m_uiEntityAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_uiEntityAttribs |= LABELATTRIB_Vertical;
	m_bUseWidgetStates = false;
}

HyLabel::HyLabel(const HyUiPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_eStackedAlignment(HYALIGN_Center),
	m_iSideBySidePadding(0)
{
	RegisterAssembleEntity();

	m_uiEntityAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_uiEntityAttribs |= LABELATTRIB_Vertical;
	m_bUseWidgetStates = false;

	Setup(panelInit, HyUiTextInit());
}

HyLabel::HyLabel(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	IHyWidget(pParent),
	m_eStackedAlignment(HYALIGN_Center),
	m_iSideBySidePadding(0)
{
	RegisterAssembleEntity();

	m_uiEntityAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);
	m_uiEntityAttribs |= LABELATTRIB_Vertical;
	m_bUseWidgetStates = false;

	Setup(panelInit, textInit);
}

/*virtual*/ HyLabel::~HyLabel()
{
}

/*virtual*/ float HyLabel::GetWidth(float fPercent /*= 1.0f*/) /*override*/
{
	Assemble();

	if(m_uiEntityAttribs & LABELATTRIB_IsSideBySide)
	{
		if((m_uiEntityAttribs & LABELATTRIB_Vertical) == 0)
			return (panel.GetWidth() + GetTextWidth() + m_iSideBySidePadding) * fPercent;
		else
			return HyMath::Max(panel.GetWidth(), GetTextWidth()) * fPercent;
	}
	else // Is stacked
		return panel.GetWidth(fPercent);
}

/*virtual*/ float HyLabel::GetHeight(float fPercent /*= 1.0f*/) /*override*/
{
	Assemble();

	if(m_uiEntityAttribs & LABELATTRIB_IsSideBySide)
	{
		if(m_uiEntityAttribs & LABELATTRIB_Vertical)
			return (panel.GetHeight() + GetTextHeight() + m_iSideBySidePadding) * fPercent;
		else
			return HyMath::Max(panel.GetHeight(), GetTextHeight()) * fPercent;
	}
	else // Is stacked
		return panel.GetHeight(fPercent);
}

/*virtual*/ const b2AABB &HyLabel::GetSceneAABB() /*override*/
{
	Assemble();

	if(panel.IsBvForPanel()) // Ensure to account for a 'bounding volume' panel
		return b2AABB_Union(IHyWidget::GetSceneAABB(), { {0, 0}, {panel.GetWidth(), panel.GetHeight()} });
	
	return IHyWidget::GetSceneAABB();
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
	return panel.IsPrimForPanel() ||
		   (panel.IsItemForPanel() && panel.GetPanelNode()->IsLoadDataValid()) ||
		   m_Text.IsLoadDataValid();
}

void HyLabel::Setup(const HyUiPanelInit &panelInit)
{
	panel.Setup(panelInit, this);

	SetEnabled(IsEnabled());
	SetAssembleNeeded();
	SetSizeDirty();
	OnSetup();
}

void HyLabel::Setup(const HyUiTextInit &textInit)
{
	m_Text.Init(textInit.m_NodePath, this);
	SetTextMargins(textInit.m_Margins);

	SetEnabled(IsEnabled());
	SetAssembleNeeded();
	SetSizeDirty();
	OnSetup();
}

void HyLabel::Setup(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit)
{
	Setup(panelInit);
	Setup(textInit);
}

HyMargins<float> HyLabel::GetTextMargins() const
{
	return m_TextMargins;
}

void HyLabel::SetTextMargins(HyMargins<float> margins)
{
	m_TextMargins = margins;

	switch(GetTextType())
	{
	case HYTEXT_Line:		SetAsLine(); break;
	case HYTEXT_Column:		SetAsColumn(GetWidth()); break;
	case HYTEXT_Box:		SetAsBox(GetWidth(), GetHeight(), m_uiEntityAttribs & LABELATTRIB_Vertical, m_uiEntityAttribs & LABELATTRIB_BoxUseScissorOrSbsTextFirst); break;
	case HYTEXT_ScaleBox:	SetAsScaleBox(GetWidth(), GetHeight(), m_uiEntityAttribs & LABELATTRIB_Vertical); break;
	default:
		HyLogError("HyLabel::Setup() - Unhandled text type: " << GetTextType());
		break;
	}

	SetAssembleNeeded();
}

HyNodePath HyLabel::GetTextNodePath() const
{
	return m_Text.GetPath();
}

HyTextType HyLabel::GetTextType() const
{
	return static_cast<HyTextType>((m_uiEntityAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsLine() const
{
	return IsSideBySide() == false && HYTEXT_Line == static_cast<HyTextType>((m_uiEntityAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsColumn() const
{
	return IsSideBySide() == false && HYTEXT_Column == static_cast<HyTextType>((m_uiEntityAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsBox() const
{
	return IsSideBySide() == false && HYTEXT_Box == static_cast<HyTextType>((m_uiEntityAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsScaleBox() const
{
	return IsSideBySide() == false && HYTEXT_ScaleBox == static_cast<HyTextType>((m_uiEntityAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsVertical() const
{
	return IsSideBySide() == false && HYTEXT_Vertical == static_cast<HyTextType>((m_uiEntityAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);
}

bool HyLabel::IsSideBySide() const
{
	return (m_uiEntityAttribs & LABELATTRIB_IsSideBySide) != 0;
}

void HyLabel::SetAsLine()
{
	m_uiEntityAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiEntityAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiEntityAttribs |= (HYTEXT_Line << LABELATTRIB_StackedTextTypeOffset);

	SetAssembleNeeded();
}

void HyLabel::SetAsColumn()
{
	SetAsColumn(panel.GetWidth());
}

void HyLabel::SetAsColumn(float fWidth)
{
	m_uiEntityAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiEntityAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiEntityAttribs |= (HYTEXT_Column << LABELATTRIB_StackedTextTypeOffset);

	if(panel.GetWidth() != fWidth)
		panel.SetSize(fWidth, panel.GetHeight());

	SetAssembleNeeded();
}

void HyLabel::SetAsBox(bool bCenterVertically /*= false*/, bool bUseScissor /*= true*/)
{
	SetAsBox(panel.GetWidth(), panel.GetHeight(), bCenterVertically, bUseScissor);
}

void HyLabel::SetAsBox(float fWidth, float fHeight, bool bCenterVertically /*= false*/, bool bUseScissor /*= true*/)
{
	m_uiEntityAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiEntityAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiEntityAttribs |= (HYTEXT_Box << LABELATTRIB_StackedTextTypeOffset);

	if(bCenterVertically)
		m_uiEntityAttribs |= LABELATTRIB_Vertical;
	else
		m_uiEntityAttribs &= ~LABELATTRIB_Vertical;

	if(bUseScissor)
		m_uiEntityAttribs |= LABELATTRIB_BoxUseScissorOrSbsTextFirst;
	else
		m_uiEntityAttribs &= ~LABELATTRIB_BoxUseScissorOrSbsTextFirst;

	if(panel.GetWidth() != fWidth || panel.GetHeight() != fHeight)
		panel.SetSize(fWidth, fHeight);

	SetAssembleNeeded();
}

void HyLabel::SetAsScaleBox(bool bCenterVertically /*= true*/)
{
	SetAsScaleBox(panel.GetWidth(), panel.GetHeight(), bCenterVertically);
}

void HyLabel::SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically /*= true*/)
{
	m_uiEntityAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiEntityAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiEntityAttribs |= (HYTEXT_ScaleBox << LABELATTRIB_StackedTextTypeOffset);

	if(bCenterVertically)
		m_uiEntityAttribs |= LABELATTRIB_Vertical;
	else
		m_uiEntityAttribs &= ~LABELATTRIB_Vertical;

	if(panel.GetWidth() != fWidth || panel.GetHeight() != fHeight)
		panel.SetSize(fWidth, fHeight);

	SetAssembleNeeded();
}

void HyLabel::SetAsVertical()
{
	m_uiEntityAttribs &= ~LABELATTRIB_IsSideBySide;

	m_uiEntityAttribs &= ~LABELATTRIB_StackedTextTypeMask;
	m_uiEntityAttribs |= (HYTEXT_Vertical << LABELATTRIB_StackedTextTypeOffset);

	SetAssembleNeeded();
}

void HyLabel::SetAsSideBySide(bool bPanelBeforeText /*= true*/, int32 iPadding /*= 5*/, HyOrientation eOrientation /*= HYORIENT_Horizontal*/)
{
	m_uiEntityAttribs |= LABELATTRIB_IsSideBySide;
	if(bPanelBeforeText)
		m_uiEntityAttribs &= ~LABELATTRIB_BoxUseScissorOrSbsTextFirst;
	else
		m_uiEntityAttribs |= LABELATTRIB_BoxUseScissorOrSbsTextFirst;

	m_iSideBySidePadding = iPadding;

	if(eOrientation == HYORIENT_Horizontal)
		m_uiEntityAttribs &= ~LABELATTRIB_Vertical;
	else
		m_uiEntityAttribs |= LABELATTRIB_Vertical;

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
	if(sUtf8Text == m_Text.GetUtf8String())
		return;

	m_Text.SetText(sUtf8Text);
	SetAssembleNeeded();
}

uint32 HyLabel::GetNumTextStates()
{
	return m_Text.GetNumStates();
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

#ifdef HY_PLATFORM_GUI
void HyLabel::GuiOverrideTextNodeData(HyJsonObj itemDataObj, bool bUseGuiOverrideName /*= true*/)
{
	m_Text.GuiOverrideData<HyTextData>(itemDataObj, bUseGuiOverrideName);
}
#endif

/*virtual*/ void HyLabel::OnAssemble() /*override*/
{
	if(m_uiEntityAttribs & LABELATTRIB_IsSideBySide)
	{
		m_Text.SetAsLine();

		// Determine "first" and "second" size and offset
		IHyBody2d *pFirst = nullptr;
		glm::vec2 vFirstSize;

		IHyBody2d *pSecond = nullptr;
		glm::vec2 vSecondSize;

		if(m_uiEntityAttribs & LABELATTRIB_BoxUseScissorOrSbsTextFirst)
		{
			if(m_uiEntityAttribs & LABELATTRIB_Vertical)
				m_Text.SetAlignment(HYALIGN_Center);
			else
				m_Text.SetAlignment(HYALIGN_Right);

			pFirst = &m_Text;
			HySetVec(vFirstSize, m_Text.GetWidth(m_Text.scale.X()), m_Text.GetLineBreakHeight(m_Text.scale.Y()));
			pFirst->pos.Set(-m_Text.GetBottomLeft());

			if(panel.GetPanelNode())
			{
				pSecond = panel.GetPanelNode();
				HySetVec(vSecondSize, panel.GetWidth(), panel.GetHeight());
				pSecond->pos.Set(0, 0);
			}
		}
		else
		{
			if(m_uiEntityAttribs & LABELATTRIB_Vertical)
				m_Text.SetAlignment(HYALIGN_Center);
			else
				m_Text.SetAlignment(HYALIGN_Left);

			if(panel.GetPanelNode())
			{
				pFirst = panel.GetPanelNode();
				HySetVec(vFirstSize, panel.GetWidth(), panel.GetHeight());
				pFirst->pos.Set(0, 0);
			}

			pSecond = &m_Text;
			HySetVec(vSecondSize, m_Text.GetWidth(m_Text.scale.X()), m_Text.GetLineBreakHeight(m_Text.scale.Y()));
			pSecond->pos.Set(-m_Text.GetBottomLeft());
		}

		// Then offset "first" and "second" appropriately
		if(m_uiEntityAttribs & LABELATTRIB_Vertical)
		{
			if(vFirstSize.x >= vSecondSize.x)
			{
				if(pFirst)
					pFirst->pos.Offset(0.0f, vSecondSize.y + m_iSideBySidePadding);
				if(pSecond)
					pSecond->pos.Offset((vFirstSize.x - vSecondSize.x) * 0.5f, 0.0f);
			}
			else if(pFirst)
				pFirst->pos.Offset((vFirstSize.x - vSecondSize.x) * 0.5f, vSecondSize.y + m_iSideBySidePadding);
		}
		else // Horizontal side-by-side
		{
			if(vFirstSize.y >= vSecondSize.y)
			{
				if(pSecond)
					pSecond->pos.Offset(vFirstSize.x + m_iSideBySidePadding, (vFirstSize.y - vSecondSize.y) * 0.5f);
			}
			else
			{
				if(pFirst)
					pFirst->pos.Offset(0.0f, (vSecondSize.y - vFirstSize.y) * 0.5f);
				if(pSecond)
					pSecond->pos.Offset(vFirstSize.x + m_iSideBySidePadding, 0.0f);
			}
		}

		m_Text.pos.Offset(0.0f, -m_Text.GetLineDescender(m_Text.scale.GetY()));
	}
	else // Stacked Panel/Text
	{
		m_Text.SetAlignment(m_eStackedAlignment);

		glm::vec2 vPanelDimensions(panel.GetWidth(), panel.GetHeight());

		// Position text to bottom left of 'm_TextMargins'
		m_Text.pos.Set(panel.GetFrameStrokeSize() + m_TextMargins.left,
					   panel.GetFrameStrokeSize() + m_TextMargins.bottom);

		float fLineOffsetX = 0.0f;	// If *this is 'LABELATTRIB_StackedTextUseLine' determine how much to offset m_Text's position (not needed for scale boxes)
		if(m_eStackedAlignment == HYALIGN_Center)
			fLineOffsetX = (vPanelDimensions.x * 0.5f) - ((m_TextMargins.left + m_TextMargins.right));
		else if(m_eStackedAlignment == HYALIGN_Right)
			fLineOffsetX = vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right));

		// Set text type/size
		HyTextType eStackedTextType = static_cast<HyTextType>((m_uiEntityAttribs & LABELATTRIB_StackedTextTypeMask) >> LABELATTRIB_StackedTextTypeOffset);

		if(vPanelDimensions.x != 0.0f && vPanelDimensions.y != 0.0f)
		{
			switch(eStackedTextType)
			{
			case HYTEXT_Line: {
				m_Text.SetAsLine();
				float fLineOffsetY = 0.0f;
				float fVerticalSpace = vPanelDimensions.y - ((m_TextMargins.top + m_TextMargins.bottom)) - (panel.GetFrameStrokeSize() * 2);
				float fTextHeight = m_Text.GetLineBreakHeight(m_Text.scale.Y());
				if(fVerticalSpace > fTextHeight)
					fLineOffsetY = (fVerticalSpace - fTextHeight) * 0.5f;
				m_Text.pos.Offset(fLineOffsetX, fLineOffsetY - m_Text.GetLineDescender(m_Text.scale.Y()));
				break; }

			case HYTEXT_ScaleBox:
				m_Text.SetAsScaleBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right)) - (panel.GetFrameStrokeSize() * 2),
					vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top)) - (panel.GetFrameStrokeSize() * 2), true);
				break;

			case HYTEXT_Box:
				m_Text.SetAsBox(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right)) - (panel.GetFrameStrokeSize() * 2),
					vPanelDimensions.y - ((m_TextMargins.bottom + m_TextMargins.top)) - (panel.GetFrameStrokeSize() * 2), true);
				break;

			case HYTEXT_Column:
				m_Text.SetAsColumn(vPanelDimensions.x - ((m_TextMargins.left + m_TextMargins.right)) - (panel.GetFrameStrokeSize() * 2));
				m_Text.pos.Offset(0.0f, panel.GetHeight() - m_Text.GetLineAscender(1.0f));
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
	SetSizeDirty();
}

/*virtual*/ glm::ivec2 HyLabel::OnCalcPreferredSize() /*override*/
{
	if((m_uiEntityAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		glm::vec2 vPanelSize = panel.GetSize();

		if(vPanelSize.x == 0 || vPanelSize.y == 0)
			return glm::ivec2(static_cast<int32>(m_Text.GetWidth(1.0f) + m_TextMargins.left + m_TextMargins.right), static_cast<int32>(m_Text.GetHeight(1.0f)) + m_TextMargins.top + m_TextMargins.bottom);
		else
			return vPanelSize;
	}
	else // Side-by-side
	{
		glm::vec2 vPanelSize = panel.GetSize();
		if(m_uiEntityAttribs & LABELATTRIB_Vertical)
			return glm::ivec2(static_cast<int32>(HyMath::Max(vPanelSize.x, GetTextWidth())), static_cast<int32>(vPanelSize.y + m_iSideBySidePadding + GetTextHeight()));
		else // Horizontal
			return glm::ivec2(static_cast<int32>(vPanelSize.x + m_iSideBySidePadding + GetTextWidth()), static_cast<int32>(HyMath::Max(vPanelSize.y, GetTextHeight())));
	}
}

/*virtual*/ glm::ivec2 HyLabel::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	if((m_uiEntityAttribs & LABELATTRIB_IsSideBySide) == 0) // Is Stacked
	{
		if(m_Text.GetUtf8String().empty() == false)
		{
			glm::ivec2 vTextSizeHint = GetPreferredSize();
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
		
		panel.SetSize(uiNewWidth, uiNewHeight);
	}
	else // Side-by-side
	{
		glm::ivec2 vSizeHint = GetPreferredSize();
		glm::ivec2 vPanelSizeHint = panel.GetSize();
		glm::ivec2 vTextSizeHint(GetTextWidth(), GetTextHeight());

		glm::ivec2 vNewPanelSize, vNewTextSize;
		if(m_uiEntityAttribs & LABELATTRIB_Vertical)
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

		panel.SetSize(vNewPanelSize.x, vNewPanelSize.y);

		float fScaleX = static_cast<float>(vNewTextSize.x) / static_cast<float>(vTextSizeHint.x);
		float fScaleY = static_cast<float>(vNewTextSize.y) / static_cast<float>(vTextSizeHint.y);
		m_Text.scale.SetAll(HyMath::Min(fScaleX, fScaleY));
	}
	
	return glm::ivec2(uiNewWidth, uiNewHeight);
}

/*virtual*/ void HyLabel::OnApplyWidgetState(HyPanelState eWidgetState) /*override*/
{
	if(m_bUseWidgetStates)
		panel.SetState(eWidgetState);

	//if(IsUsingPanelStates())
	//{
	//	HyPanelState eCurState = CalcPanelState();
	//	if(m_ePanelState != eCurState)
	//	{
	//		m_ePanelState = eCurState;
	//		SetState(m_ePanelState);
	//	}
	//}

	//SetAssembleNeeded();
}
