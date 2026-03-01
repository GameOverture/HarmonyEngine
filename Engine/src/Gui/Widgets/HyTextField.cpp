/**************************************************************************
*	HyTextField.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Widgets/HyTextField.h"
#include "HyEngine.h"

#define HYLINEEDIT_BLINKDUR 0.5f

HyTextField::HyTextField(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_uiCursorIndex(0),
	m_uiSelectionIndex(0),
	m_Selection(this),
	m_Cursor(this),
	m_fpOnSubmit(nullptr)
{
	m_bUseWidgetStates = true;

	m_Selection.alpha.Set(0.5f);
	ChildInsert(m_Text, m_Selection);
	ChildAppend(m_Cursor);

	SetAlignment(HYALIGN_Left);
	SetAsLine();

	SetCursor(GetCursorIndex(), GetSelectionIndex());

	m_BlinkTimer.SetExpiredCallback(
		[this]()
		{
			m_Cursor.SetVisible(!m_Cursor.IsVisible());
			m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
		});
	m_BlinkTimer.Init(HYLINEEDIT_BLINKDUR);

	SetKeyboardFocusAllowed(true);
	SetHoverCursor(HYMOUSECURSOR_IBeam);
}

HyTextField::HyTextField(const HyUiPanelInit &initRef, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, textInit, pParent),
	m_uiCursorIndex(0),
	m_uiSelectionIndex(0),
	m_Selection(this),
	m_Cursor(this),
	m_fpOnSubmit(nullptr)
{
	m_bUseWidgetStates = true;

	m_Selection.alpha.Set(0.5f);
	ChildInsert(m_Text, m_Selection);
	ChildAppend(m_Cursor);

	SetAlignment(HYALIGN_Left);
	SetAsLine();

	SetCursor(GetCursorIndex(), GetSelectionIndex());

	m_BlinkTimer.SetExpiredCallback(
		[this]()
		{
			m_Cursor.SetVisible(!m_Cursor.IsVisible());
			m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
		});
	m_BlinkTimer.Init(HYLINEEDIT_BLINKDUR);

	SetKeyboardFocusAllowed(true);
	SetHoverCursor(HYMOUSECURSOR_IBeam);
}

/*virtual*/ HyTextField::~HyTextField()
{
}

/*virtual*/ void HyTextField::SetText(const std::string &sUtf8Text) /*override*/
{
	HyLabel::SetText(sUtf8Text);

	if(m_uiCursorIndex > HyIO::Utf8Length(sUtf8Text))
		SetCursor(static_cast<uint32>(HyIO::Utf8Length(sUtf8Text)));
}

bool HyTextField::IsInputValidated() const
{
	return (m_uiEntityAttribs & TEXTFIELDATTRIB_UseValidator) != 0;
}

std::string HyTextField::GetInputValidator() const
{
	return m_sValidationRegEx;
}

void HyTextField::SetInputValidator(std::string sRegEx, bool bCaseSensitive)
{
	m_sValidationRegEx = sRegEx;

	std::regex_constants::syntax_option_type typeFlags = std::regex_constants::optimize;
	if(bCaseSensitive)
		typeFlags |= std::regex_constants::icase;

	m_InputValidator = std::regex(m_sValidationRegEx, typeFlags);
	m_uiEntityAttribs |= TEXTFIELDATTRIB_UseValidator;
}

void HyTextField::ClearInputValidator()
{
	m_uiEntityAttribs &= ~TEXTFIELDATTRIB_UseValidator;
}

bool HyTextField::IsCursorShown() const
{
	return m_BlinkTimer.IsRunning();
}

uint32 HyTextField::GetCursorIndex() const
{
	return m_uiCursorIndex;
}

uint32 HyTextField::GetSelectionIndex() const
{
	return m_uiSelectionIndex;
}

void HyTextField::GetSelection(uint32 &uiStartIndexOut, uint32 &uiEndIndexOut) const
{
	if(GetCursorIndex() < GetSelectionIndex())
	{
		uiStartIndexOut = GetCursorIndex();
		uiEndIndexOut = GetSelectionIndex();
	}
	else
	{
		uiStartIndexOut = GetSelectionIndex();
		uiEndIndexOut = GetCursorIndex();
	}
}

void HyTextField::SetCursor(uint32 uiCharIndex)
{
	SetCursor(uiCharIndex, uiCharIndex);
}

void HyTextField::SetCursor(uint32 uiCharIndex, uint32 uiSelectionIndex)
{
	m_uiCursorIndex = uiCharIndex;
	m_uiSelectionIndex = uiSelectionIndex;

	if(m_Text.IsLoadDataValid() == false || IsCursorShown() == false)
	{
		m_Cursor.SetAsNothing(0);
		m_Selection.SetAsNothing(0);
		return;
	}

	float fCursorHeight = m_Text.GetLineBreakHeight(m_Text.scale.Y());
	m_Cursor.SetAsBox(0, HyRect(2.0f, fCursorHeight));

	m_Cursor.pos.Set(m_Text.pos);
	m_Cursor.pos.Offset(m_Text.GetGlyphOffset(m_uiCursorIndex, 0).x * m_Text.scale.GetX(), m_Text.GetLineDescender(m_Text.scale.Y()));
	
	if(m_uiCursorIndex == m_uiSelectionIndex)
		m_Selection.SetAsNothing(0);
	else
	{
		uint32 uiStartIndex, uiEndIndex;
		GetSelection(uiStartIndex, uiEndIndex);

		float fWidth = m_Text.GetGlyphOffset(uiEndIndex, 0).x - m_Text.GetGlyphOffset(uiStartIndex, 0).x;
		m_Selection.SetAsBox(0, HyRect(fWidth, fCursorHeight));

		m_Selection.pos.Set(m_Text.pos);
		m_Selection.pos.Offset(m_Text.GetGlyphOffset(uiStartIndex, 0).x * m_Text.scale.GetX(), m_Text.GetLineDescender(m_Text.scale.Y()));
	}

	if(IsKeyboardFocus())
	{
		m_Selection.SetVisible(true);
		m_Cursor.SetVisible(true);
		m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	}
	else
	{
		m_Selection.SetVisible(false);
		m_Cursor.SetVisible(false);
	}
}

void HyTextField::SetOnSubmit(std::function<void(HyTextField *)> fpOnSubmit)
{
	m_fpOnSubmit = fpOnSubmit;
}

void HyTextField::Submit()
{
	if(m_fpOnSubmit != nullptr)
		m_fpOnSubmit(this);
}

/*virtual*/ void HyTextField::OnUiTextInput(std::string sNewUtf8Text) /*override*/
{
	if(IsInputValidated())
	{
		std::smatch base_match;
		if(std::regex_match(sNewUtf8Text, base_match, m_InputValidator))
		{
			// The first sub_match is the whole string; the next
			// sub_match is the first parenthesized expression.
			if(base_match.size() == 2)
			{
				std::ssub_match base_sub_match = base_match[1];
				sNewUtf8Text = base_sub_match.str();
			}
		}
		else
			return;
	}

	std::string sText = m_Text.GetUtf8String();

	if(GetCursorIndex() == HyIO::Utf8Length(sText))
		sText += sNewUtf8Text;
	else
	{
		if(GetCursorIndex() != GetSelectionIndex())
		{
			uint32 uiStartIndex, uiEndIndex;
			GetSelection(uiStartIndex, uiEndIndex);
			HyIO::Utf8Erase(sText, uiStartIndex, uiEndIndex - uiStartIndex);
		}

		HyIO::Utf8Insert(sText, GetCursorIndex(), sNewUtf8Text);
	}
	
	m_Text.SetText(sText);

	m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
	
	int iNewCursorIndex = GetCursorIndex() + static_cast<int32>(HyIO::Utf8Length(sNewUtf8Text));
	SetCursor(iNewCursorIndex);
}

/*virtual*/ void HyTextField::OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) /*override*/
{
	if(eBtnState == HYBTN_Release)
		return;

	switch(eBtn)
	{
	case HYKEY_Enter:
		Submit();
		break;

	case HYKEY_Backspace:
	case HYKEY_Delete:
		if(GetCursorIndex() != GetSelectionIndex())
		{
			uint32 uiStartIndex, uiEndIndex;
			GetSelection(uiStartIndex, uiEndIndex);

			std::string sText = m_Text.GetUtf8String();
			HyIO::Utf8Erase(sText, uiStartIndex, uiEndIndex - uiStartIndex);
			m_Text.SetText(sText);
			SetCursor(uiStartIndex);
		}
		else if(eBtn == HYKEY_Backspace)
		{
			if(GetCursorIndex() > 0)
			{
				std::string sText = m_Text.GetUtf8String();
				HyIO::Utf8Erase(sText, GetCursorIndex() - 1, 1);
				m_Text.SetText(sText);
				SetCursor(GetCursorIndex() - 1);
			}
		}
		else // HYKEY_Delete
		{
			if(GetCursorIndex() < HyIO::Utf8Length(m_Text.GetUtf8String()))
			{
				std::string sText = m_Text.GetUtf8String();
				HyIO::Utf8Erase(sText, GetCursorIndex(), 1);
				m_Text.SetText(sText);
				SetCursor(GetCursorIndex());
			}
		}
		break;

	case HYKEY_Left:
		if(iMods & HYKBMOD_Control)
		{
			// When holding ctrl, offset the cursor left, to the beginning of the next word or symbol
			int32 iOffsetAmt = -1;

			// Jump past any whitespace first
			while(static_cast<int32>(GetCursorIndex()) + iOffsetAmt > 0 && HyIO::Utf8IsWhitespace(m_Text.GetUtf8Character(GetCursorIndex() + iOffsetAmt)))
				iOffsetAmt--;

			// Get to the next alphanumeric word
			while(static_cast<int32>(GetCursorIndex()) + iOffsetAmt > 0 && HyIO::Utf8IsAlnum(m_Text.GetUtf8Character(GetCursorIndex() + iOffsetAmt - 1)))
				iOffsetAmt--;

			MoveCursor(iOffsetAmt, iMods & HYKBMOD_Shift);
		}
		else
			MoveCursor(-1, iMods & HYKBMOD_Shift);
		
		break;

	case HYKEY_Right:
		if(iMods & HYKBMOD_Control)
		{
			// When holding ctrl, offset the cursor right, to the beginning of the next word or symbol
			int32 iOffsetAmt = 1;

			// Get to the next alphanumeric word
			while(GetCursorIndex() + iOffsetAmt < m_Text.GetNumCharacters() && HyIO::Utf8IsAlnum(m_Text.GetUtf8Character(GetCursorIndex() + iOffsetAmt)))
				iOffsetAmt++;

			// plus whitespace to get to next word/symbol
			while(GetCursorIndex() + iOffsetAmt < m_Text.GetNumCharacters() && HyIO::Utf8IsWhitespace(m_Text.GetUtf8Character(GetCursorIndex() + iOffsetAmt)))
				iOffsetAmt++;

			MoveCursor(iOffsetAmt, iMods & HYKBMOD_Shift);
		}
		else
			MoveCursor(1, iMods & HYKBMOD_Shift);
		
		break;


	case HYKEY_Home:
		MoveCursor(-static_cast<int32>(GetCursorIndex()), iMods & HYKBMOD_Shift);
		break;

	case HYKEY_End:
		MoveCursor(m_Text.GetNumCharacters() - GetCursorIndex(), iMods & HYKBMOD_Shift);
		break;

	default:
		break;
	}

	m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
}

/*virtual*/ void HyTextField::OnTakeKeyboardFocus() /*override*/
{
	HyEngine::Input().StartTextInput();

	m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	SetCursor(static_cast<int32>(m_Text.GetUtf8String().length()));
}

/*virtual*/ void HyTextField::OnRelinquishKeyboardFocus() /*override*/
{
	HyEngine::Input().StopTextInput();
	m_BlinkTimer.Reset();
	
	m_Selection.SetVisible(false);
	m_Cursor.SetVisible(false);
}

void HyTextField::MoveCursor(int32 iOffset, bool bSelection)
{
	int32 iNewCursorIndex = GetCursorIndex() + iOffset;
	if(iNewCursorIndex < 0)
		iNewCursorIndex = 0;
	else if(iNewCursorIndex > static_cast<int32>(m_Text.GetNumCharacters()))
		iNewCursorIndex = m_Text.GetNumCharacters();

	SetCursor(iNewCursorIndex, bSelection ? GetSelectionIndex() : iNewCursorIndex);
}

/*virtual*/ void HyTextField::OnApplyWidgetState(HyPanelState eWidgetState) /*override*/
{
	HyLabel::OnApplyWidgetState(eWidgetState);

	m_Cursor.SetTint(panel.GetPanelColor().IsDark() ? HyColor::White : HyColor::Black);
	m_Selection.SetTint(panel.GetPanelColor().IsDark() ? HyColor::White : HyColor::Black);
}
