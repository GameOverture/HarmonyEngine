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
	int32								m_iCursorIndex;			// Cursor index in full UTF-8 characters
	int32								m_iSelectionLength;		// Selection length in full UTF-8 characters
	HyPrimitive2d						m_Selection;			// Actual text selection highlight visual
	HyPrimitive2d						m_PrimCursor;			// Shows a standard vertical line draw with a primitive

	HyTimer								m_BlinkTimer;

public:
	HyLineEdit(HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyLineEdit();

	virtual void SetText(const std::string &sUtf8Text) override;

	bool IsCursorShown() const;
	void SetCursor(int32 iUtf8CharIndex, int32 iSelectionLen);

protected:
	virtual void OnUiTextInput(std::string sNewUtf8Text) override;
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn) override;
	virtual void OnUiMouseClicked() override;

	virtual void OnTakeKeyboardFocus() override;
	virtual void OnRelinquishKeyboardFocus() override;

	virtual void OnSetup() override;

	static void OnCursorTimer(void *pThisData);
};

#endif /* HyLineEdit_h__ */
