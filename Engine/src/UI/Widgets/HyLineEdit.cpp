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

/*virtual*/ void HyLineEdit::OnUiTextInput(std::string sNewText) /*override*/
{
	m_Text.OnUiTextInput(sNewText);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
}

/*virtual*/ void HyLineEdit::OnUiKeyboardInput(HyKeyboardBtn eBtn) /*override*/
{
	m_Text.OnUiKeyboardInput(eBtn);
	SetText(m_Text.GetUtf8String()); // Ensure HyLabel is informed of m_Text changing
}

/*virtual*/ void HyLineEdit::OnUiMouseClicked() /*override*/
{
	RequestKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnTakeKeyboardFocus() /*override*/
{
	m_Text.OnTakeKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnRelinquishKeyboardFocus() /*override*/
{
	m_Text.OnRelinquishKeyboardFocus();
}

/*virtual*/ void HyLineEdit::OnSetup() /*override*/
{
	SetKeyboardFocusAllowed(true);
	SetHoverCursor(HYMOUSECURSOR_IBeam);
	m_uiAttribs |= LABELATTRIB_StackedTextUseLine | LABELATTRIB_StackedTextLeftAlign;
}
