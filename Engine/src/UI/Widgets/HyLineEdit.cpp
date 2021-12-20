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
#include "UI/Widgets/HyLineEdit.h"
#include "HyEngine.h"

HyLineEdit::HyLineEdit(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent)
{
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, pParent)
{
	OnSetup();
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent)
{
	OnSetup();
}

/*virtual*/ HyLineEdit::~HyLineEdit()
{
}

void HyLineEdit::SetCursorChar(std::string sUtf8Char)
{
	m_TextCursor.SetText(sUtf8Char);
}

/*virtual*/ void HyLineEdit::OnUpdate() /*override*/
{
}

/*virtual*/ void HyLineEdit::OnMouseClicked() /*override*/
{
	RequestKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnTakeKeyboardFocus() /*override*/
{
	HyEngine::Input().StartTextInput();
}

/*virtual*/ void HyLineEdit::OnRelinquishKeyboardFocus() /*override*/
{
	int32 iCursorIndexOut, iSelectionLengthOut;
	SetText(HyEngine::Input().GetTextInput(iCursorIndexOut, iSelectionLengthOut));

	HyEngine::Input().StopTextInput();
}

/*virtual*/ void HyLineEdit::OnSetup() /*override*/
{
	m_TextCursor.Init(m_Text.GetPrefix(), m_Text.GetName(), this);
	m_TextCursor.alpha.Set(0.0f);
	m_TextCursor.SetText("|");
	SetKeyboardFocusAllowed(true);
	SetHoverCursor(HYMOUSECURSOR_IBeam);
	m_uiAttribs |= LABELATTRIB_StackedTextUseLine | LABELATTRIB_StackedTextLeftAlign;
	m_TextCursorBlinkTimer.SetExpiredCallback(OnCursorTimer, this);
	m_TextCursorBlinkTimer.InitStart(0.5f);
}

/*static*/ void HyLineEdit::OnCursorTimer(void *pThisData)
{
	HyLineEdit *pThis = static_cast<HyLineEdit *>(pThisData);

	if(pThis->IsKeyboardFocus())
	{
		pThis->m_TextCursor.pos.Set(pThis->m_Text.pos);
		pThis->m_TextCursor.pos.Offset(pThis->m_Text.GetTextCursorPos());
		pThis->m_TextCursor.SetState(pThis->m_Text.GetState());

		if(pThis->m_TextCursor.alpha.Get() == 0.0f)
			pThis->m_TextCursor.alpha.Set(1.0f);
		else
			pThis->m_TextCursor.alpha.Set(0.0f);

		int32 iCursorIndexOut, iSelectionLengthOut;
		pThis->SetText(HyEngine::Input().GetTextInput(iCursorIndexOut, iSelectionLengthOut));
	}
	
	pThis->m_TextCursorBlinkTimer.InitStart(0.5f);
}
