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

/*virtual*/ void HyLineEdit::OnSetup() /*override*/
{
	m_uiLabelAttribs |= LABELATTRIB_StackedTextUseLine | LABELATTRIB_StackedTextLeftAlign;
	m_TextCursorBlinkTimer.SetExpiredCallback(OnCursorTimer, this);
}

/*static*/ void HyLineEdit::OnCursorTimer(void *pThisData)
{
	HyLineEdit *pThis = static_cast<HyLineEdit *>(pThisData);

	//pThis->m_TextCursor.Set
}
