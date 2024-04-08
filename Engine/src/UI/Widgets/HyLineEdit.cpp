/**************************************************************************
*	HyLineEdit.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLineEdit.h"
#include "HyEngine.h"

#define HYLINEEDIT_BLINKDUR 0.5f

HyLineEdit::HyLineEdit(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_uiCursorIndex(0),
	m_uiSelectionIndex(0),
	m_Selection(this),
	m_Cursor(this),
	m_fpOnSubmit(nullptr)
{
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, textNodePath, pParent),
	m_uiCursorIndex(0),
	m_uiSelectionIndex(0),
	m_Selection(this),
	m_Cursor(this),
	m_fpOnSubmit(nullptr)
{
	OnSetup();
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, textNodePath, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_uiCursorIndex(0),
	m_uiSelectionIndex(0),
	m_Selection(this),
	m_Cursor(this),
	m_fpOnSubmit(nullptr)
{
	OnSetup();
}

/*virtual*/ HyLineEdit::~HyLineEdit()
{
}

/*virtual*/ void HyLineEdit::SetText(const std::string &sUtf8Text) /*override*/
{
	HyLabel::SetText(sUtf8Text);

	if(m_uiCursorIndex > HyIO::Utf8Length(sUtf8Text))
		SetCursor(HyIO::Utf8Length(sUtf8Text));
}

bool HyLineEdit::IsInputValidated() const
{
	return (m_uiAttribs & LINEEDITATTRIB_UseValidator) != 0;
}

void HyLineEdit::SetInputValidator(const std::regex &regEx)
{
	m_uiAttribs |= LINEEDITATTRIB_UseValidator;
	m_InputValidator = regEx;
}

void HyLineEdit::ClearInputValidator()
{
	m_uiAttribs &= ~LINEEDITATTRIB_UseValidator;
}

bool HyLineEdit::IsCursorShown() const
{
	return m_BlinkTimer.IsRunning();
}

uint32 HyLineEdit::GetCursorIndex() const
{
	return m_uiCursorIndex;
}

uint32 HyLineEdit::GetSelectionIndex() const
{
	return m_uiSelectionIndex;
}

void HyLineEdit::GetSelection(uint32 &uiStartIndexOut, uint32 &uiEndIndexOut) const
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

void HyLineEdit::SetCursor(uint32 uiCharIndex)
{
	SetCursor(uiCharIndex, uiCharIndex);
}

void HyLineEdit::SetCursor(uint32 uiCharIndex, uint32 uiSelectionIndex)
{
	m_uiCursorIndex = uiCharIndex;
	m_uiSelectionIndex = uiSelectionIndex;

	if(m_Text.IsLoadDataValid() == false || IsCursorShown() == false)
	{
		m_Cursor.SetAsNothing();
		m_Selection.SetAsNothing();
		return;
	}

	float fCursorHeight = m_Text.GetLineBreakHeight(m_Text.scale.Y());
	m_Cursor.SetAsBox(HyRect(2.0f, fCursorHeight));

	m_Cursor.pos.Set(m_Text.pos);
	m_Cursor.pos.Offset(m_Text.GetGlyphOffset(m_uiCursorIndex, 0).x * m_Text.scale.GetX(), m_Text.GetLineDescender(m_Text.scale.Y()));
	
	if(m_uiCursorIndex == m_uiSelectionIndex)
		m_Selection.SetAsNothing();
	else
	{
		uint32 uiStartIndex, uiEndIndex;
		GetSelection(uiStartIndex, uiEndIndex);

		float fWidth = m_Text.GetGlyphOffset(uiEndIndex, 0).x - m_Text.GetGlyphOffset(uiStartIndex, 0).x;
		m_Selection.SetAsBox(HyRect(fWidth, fCursorHeight));

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

void HyLineEdit::SetOnSubmit(std::function<void(HyLineEdit *)> fpOnSubmit)
{
	m_fpOnSubmit = fpOnSubmit;
}

void HyLineEdit::Submit()
{
	if(m_fpOnSubmit != nullptr)
		m_fpOnSubmit(this);
}

/*virtual*/ void HyLineEdit::OnUiTextInput(std::string sNewUtf8Text) /*override*/
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

/*virtual*/ void HyLineEdit::OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) /*override*/
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

/*virtual*/ void HyLineEdit::OnTakeKeyboardFocus() /*override*/
{
	HyEngine::Input().StartTextInput();

	m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	SetCursor(static_cast<int32>(m_Text.GetUtf8String().length()));
}

/*virtual*/ void HyLineEdit::OnRelinquishKeyboardFocus() /*override*/
{
	HyEngine::Input().StopTextInput();
	m_BlinkTimer.Reset();
	
	m_Selection.SetVisible(false);
	m_Cursor.SetVisible(false);
}

/*virtual*/ void HyLineEdit::OnPanelUpdated() /*override*/
{
	HyLabel::OnPanelUpdated();

	m_Cursor.SetTint(m_Panel.GetPanelColor().IsDark() ? HyColor::White : HyColor::Black);
	m_Selection.SetTint(m_Panel.GetPanelColor().IsDark() ? HyColor::White : HyColor::Black);
}

/*virtual*/ void HyLineEdit::OnSetup() /*override*/
{
	m_Selection.alpha.Set(0.5f);
	ChildInsert(m_Text, m_Selection);
	ChildAppend(m_Cursor);

	SetAsStacked(HYALIGN_Left, HYTEXT_Line);

	SetCursor(GetCursorIndex(), GetSelectionIndex());

	m_BlinkTimer.SetExpiredCallback(
		[this]()
		{
			m_Cursor.SetVisible(!m_Cursor.IsVisible());
			m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
		});
	m_BlinkTimer.Init(HYLINEEDIT_BLINKDUR);

	SetKeyboardFocusAllowed(true);
	SetMouseHoverCursor(HYMOUSECURSOR_IBeam);
}

void HyLineEdit::MoveCursor(int32 iOffset, bool bSelection)
{
	int32 iNewCursorIndex = GetCursorIndex() + iOffset;
	if(iNewCursorIndex < 0)
		iNewCursorIndex = 0;
	else if(iNewCursorIndex > static_cast<int32>(m_Text.GetNumCharacters()))
		iNewCursorIndex = m_Text.GetNumCharacters();

	SetCursor(iNewCursorIndex, bSelection ? GetSelectionIndex() : iNewCursorIndex);
}
