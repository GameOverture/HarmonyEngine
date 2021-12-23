/**************************************************************************
*	HyTextInput.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/Components/HyTextInput.h"
#include "HyEngine.h"

#define HYTEXTINPUT_BLINKDUR 0.5f

HyTextInput::HyTextInput(HyText2d &text2dRef, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_TextRef(text2dRef),
	m_iCursorIndex(0),
	m_iSelectionLength(0),
	m_Selection(this),
	m_PrimCursor(this)
{
	SetCursor(m_iCursorIndex, m_iSelectionLength);

	m_BlinkTimer.SetExpiredCallback(OnCursorTimer, this);
	m_BlinkTimer.Init(HYTEXTINPUT_BLINKDUR);
	alpha.Set(0.0f);
}

/*virtual*/ HyTextInput::~HyTextInput()
{
}

void HyTextInput::SetCursor(int32 iUtf8CharIndex, int32 iSelectionLen)
{
	m_iCursorIndex = iUtf8CharIndex;
	m_iSelectionLength = iSelectionLen;

	if(m_TextRef.IsLoadDataValid() == false)
	{
		m_Selection.SetAsNothing();
		m_PrimCursor.SetAsNothing();
		return;
	}

	const HyText2dData *pTextData = static_cast<const HyText2dData *>(m_TextRef.AcquireData());
	float fCursorHeight = pTextData->GetLineHeight(m_TextRef.GetState());
	
	if(m_iSelectionLength > 0)
	{
		float fWidth = m_TextRef.GetGlyphOffset(m_iCursorIndex + m_iSelectionLength, 0).x - m_TextRef.GetGlyphOffset(m_iCursorIndex, 0).x;
		m_Selection.SetAsBox(fWidth, fCursorHeight);
	}
	else
		m_Selection.SetAsNothing();
	m_PrimCursor.SetAsBox(2.0f, fCursorHeight);

	m_Selection.SetDisplayOrder(m_TextRef.GetDisplayOrder() - 1);
	m_PrimCursor.SetDisplayOrder(m_TextRef.GetDisplayOrder() + 1);

	if(m_iCursorIndex == m_TextRef.GetNumShownCharacters())
		m_PrimCursor.pos.Set(m_TextRef.GetTextCursorPos());
	else
		m_PrimCursor.pos.Set(m_TextRef.GetGlyphOffset(m_iCursorIndex, 0));
}

void HyTextInput::OnTakeKeyboardFocus()
{
	HyEngine::Input().StartTextInput();
	m_BlinkTimer.InitStart(HYTEXTINPUT_BLINKDUR);
	alpha.Set(1.0f);
}

void HyTextInput::OnRelinquishKeyboardFocus()
{
	HyEngine::Input().StopTextInput();
	m_BlinkTimer.Reset();
	alpha.Set(0.0f);
}

void HyTextInput::OnUiTextInput(std::string sNewUtf8Text)
{
	std::string sText = m_TextRef.GetUtf8String();

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

	m_TextRef.SetText(sText);

	m_BlinkTimer.InitStart(HYTEXTINPUT_BLINKDUR);
	alpha.Set(1.0f);
}

void HyTextInput::OnUiKeyboardInput(HyKeyboardBtn eBtn)
{
	switch(eBtn)
	{
	case HYKEY_Enter:
		break;

	case HYKEY_BackSpace: {
		std::string sText = m_TextRef.GetUtf8String();
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
		m_TextRef.SetText(sText);
		break; }
	}

	m_BlinkTimer.InitStart(HYTEXTINPUT_BLINKDUR);
	alpha.Set(1.0f);
}

/*static*/ void HyTextInput::OnCursorTimer(void *pThisData)
{
	HyTextInput *pThis = static_cast<HyTextInput *>(pThisData);

	if(pThis->alpha.Get() == 0.0f)
		pThis->alpha.Set(1.0f);
	else
		pThis->alpha.Set(0.0f);

	pThis->m_BlinkTimer.InitStart(HYTEXTINPUT_BLINKDUR);
}
