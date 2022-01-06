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
	m_bUseValidator(false),
	m_iCursorIndex(0),
	m_iSelectionLength(0),
	m_Selection(this),
	m_Cursor(this)
{
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, pParent),
	m_bUseValidator(false),
	m_iCursorIndex(0),
	m_iSelectionLength(0),
	m_Selection(this),
	m_Cursor(this)
{
	OnSetup();
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_bUseValidator(false),
	m_iCursorIndex(0),
	m_iSelectionLength(0),
	m_Selection(this),
	m_Cursor(this)
{
	OnSetup();
}

/*virtual*/ HyLineEdit::~HyLineEdit()
{
}

/*virtual*/ void HyLineEdit::SetText(const std::string &sUtf8Text) /*override*/
{
	HyLabel::SetText(sUtf8Text);
	SetCursor(sUtf8Text.length(), 0);
}

void HyLineEdit::SetInputValidator(const std::regex &regEx)
{
	m_bUseValidator = true;
	m_InputValidator = regEx;
}

void HyLineEdit::ClearInputValidator()
{
	m_bUseValidator = false;
}

bool HyLineEdit::IsCursorShown() const
{
	return m_BlinkTimer.IsRunning();
}

void HyLineEdit::SetCursor(int32 iUtf8CharIndex, int32 iSelectionLen)
{
	m_iCursorIndex = iUtf8CharIndex;
	m_iSelectionLength = iSelectionLen;

	if(m_Text.IsLoadDataValid() == false || IsCursorShown() == false)
	{
		m_Selection.SetAsNothing();
		m_Cursor.SetAsNothing();
		return;
	}

	const HyText2dData *pTextData = static_cast<const HyText2dData *>(m_Text.AcquireData());
	float fCursorHeight = pTextData->GetLineHeight(m_Text.GetState());

	if(m_iSelectionLength > 0)
	{
		float fWidth = m_Text.GetGlyphOffset(m_iCursorIndex + m_iSelectionLength, 0).x - m_Text.GetGlyphOffset(m_iCursorIndex, 0).x;
		m_Selection.SetAsBox(fWidth, fCursorHeight);
	}
	else
		m_Selection.SetAsNothing();
	m_Cursor.SetAsBox(2.0f, fCursorHeight);

	m_Selection.SetDisplayOrder(m_Text.GetDisplayOrder() - 1);
	m_Cursor.SetDisplayOrder(m_Text.GetDisplayOrder() + 1);

	m_Cursor.pos.Set(m_Text.pos);
	m_Cursor.pos.Offset(m_Text.GetTextCursorPos().x, 0.0f);
	
	m_Selection.alpha.Set(m_BlinkTimer.IsRunning() * 1.0f);
	m_Cursor.alpha.Set(m_BlinkTimer.IsRunning() * 1.0f);
}

/*virtual*/ void HyLineEdit::OnUiTextInput(std::string sNewUtf8Text) /*override*/
{
	if(m_bUseValidator)
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

	if(m_iCursorIndex == HyIO::Utf8Length(sText))
		sText += sNewUtf8Text;
	else
	{
		if(m_iSelectionLength > 0)
		{
			HyIO::Utf8Erase(sText, m_iCursorIndex, m_iSelectionLength);
			m_iSelectionLength = 0;
		}

		HyIO::Utf8Insert(sText, m_iCursorIndex, sNewUtf8Text);
	}
	m_iCursorIndex += HyIO::Utf8Length(sNewUtf8Text);

	m_Text.SetText(sText);

	m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
}

/*virtual*/ void HyLineEdit::OnUiKeyboardInput(HyKeyboardBtn eBtn) /*override*/
{
	switch(eBtn)
	{
	case HYKEY_Enter:
		break;

	case HYKEY_BackSpace: {
		std::string sText = m_Text.GetUtf8String();
		do
		{
			if(sText.empty())
				break;

			if((sText.back() & 0x80) == 0x00) // One byte
			{
				sText.pop_back();
				break;
			}

			if((sText.back() & 0xC0) == 0x80) // Byte from the multi-byte sequence
				sText.pop_back();
			if((sText.back() & 0xC0) == 0xC0) // First byte of multi-byte sequence (now UTF-8 character fully deleted)
			{
				sText.pop_back();
				break;
			}

		} while(true);
		m_Text.SetText(sText);
		break; }

	default:
		break;
	}

	m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
}

/*virtual*/ void HyLineEdit::OnUiMouseClicked() /*override*/
{
	RequestKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnTakeKeyboardFocus() /*override*/
{
	HyEngine::Input().StartTextInput();

	m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
	SetCursor(m_Text.GetUtf8String().length(), 0);
}

/*virtual*/ void HyLineEdit::OnRelinquishKeyboardFocus() /*override*/
{
	HyEngine::Input().StopTextInput();
	m_BlinkTimer.Reset();
	m_Cursor.alpha.Set(0.0f);
}

/*virtual*/ void HyLineEdit::OnSetup() /*override*/
{
	SetCursor(m_iCursorIndex, m_iSelectionLength);

	m_BlinkTimer.SetExpiredCallback(OnCursorTimer, this);
	m_BlinkTimer.Init(HYLINEEDIT_BLINKDUR);

	SetKeyboardFocusAllowed(true);
	SetHoverCursor(HYMOUSECURSOR_IBeam);
	m_uiAttribs |= LABELATTRIB_StackedTextUseLine | LABELATTRIB_StackedTextLeftAlign;
}

void HyLineEdit::ToggleCursorVisible(bool bForceShown)
{
	if(bForceShown || m_Cursor.alpha.Get() == 0.0f)
		m_Cursor.alpha.Set(1.0f);
	else
		m_Cursor.alpha.Set(0.0f);
}

/*static*/ void HyLineEdit::OnCursorTimer(void *pThisData)
{
	HyLineEdit *pThis = static_cast<HyLineEdit *>(pThisData);

	pThis->ToggleCursorVisible(false);

	pThis->m_BlinkTimer.InitStart(HYLINEEDIT_BLINKDUR);
}
