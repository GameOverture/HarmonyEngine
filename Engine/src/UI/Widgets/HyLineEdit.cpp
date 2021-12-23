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

HyLineEdit::HyLineEdit(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_TextInput(m_Text, this)
{
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, pParent),
	m_TextInput(m_Text, this)
{
	OnSetup();
}

HyLineEdit::HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_TextInput(m_Text, this)
{
	OnSetup();
}

/*virtual*/ HyLineEdit::~HyLineEdit()
{
}

/*virtual*/ void HyLineEdit::SetText(const std::string &sUtf8Text) /*override*/
{
	HyLabel::SetText(sUtf8Text);
	m_TextInput.SetCursor(sUtf8Text.length(), 0);
}

/*virtual*/ void HyLineEdit::OnUiTextInput(std::string sNewText) /*override*/
{
	m_TextInput.OnUiTextInput(sNewText);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
}

/*virtual*/ void HyLineEdit::OnUiKeyboardInput(HyKeyboardBtn eBtn) /*override*/
{
	m_TextInput.OnUiKeyboardInput(eBtn);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
}

/*virtual*/ void HyLineEdit::OnUiMouseClicked() /*override*/
{
	RequestKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnTakeKeyboardFocus() /*override*/
{
	m_TextInput.OnTakeKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnRelinquishKeyboardFocus() /*override*/
{
	m_TextInput.OnRelinquishKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnSetup() /*override*/
{
	m_TextInput.Load();

	SetKeyboardFocusAllowed(true);
	SetHoverCursor(HYMOUSECURSOR_IBeam);
	m_uiAttribs |= LABELATTRIB_StackedTextUseLine | LABELATTRIB_StackedTextLeftAlign;
}
