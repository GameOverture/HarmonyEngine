/**************************************************************************
*	HyLineEdit.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLineEdit_h__
#define HyLineEdit_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "Time/Watches/HyTimer.h"

class HyLineEdit : public HyLabel
{
protected:
	HyText2d			m_TextCursor;	// Stores and shows the cursor character in UTF-8
	HyTimer				m_TextCursorBlinkTimer;

	// Text selction highlight
	HyPrimitive2d		m_Selection;
	int32				m_iSelectionStartIndex;

public:
	HyLineEdit(HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyLineEdit();

	void SetCursorChar(std::string sUtf8Char);

protected:
	virtual void OnSetup() override;

	static void OnCursorTimer(void *pThisData);
};

#endif /* HyLineEdit_h__ */
