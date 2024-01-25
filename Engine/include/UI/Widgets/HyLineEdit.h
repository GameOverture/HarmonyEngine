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

#include <regex>

class HyLineEdit : public HyLabel
{
protected:
	bool								m_bUseValidator;
	std::regex							m_InputValidator;

	uint32								m_uiCursorIndex;		// Cursor index in full UTF-8 characters
	uint32								m_uiSelectionIndex;		// Selection index in full UTF-8 characters. Anything between m_iCursorIndex and m_iSelectionIndex will be selected. Will equal when no selection.
	HyPrimitive2d						m_Selection;			// Actual text selection highlight visual
	HyPrimitive2d						m_Cursor;				// Shows a standard vertical line draw with a primitive

	HyTimer								m_BlinkTimer;

public:
	HyLineEdit(HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyLineEdit(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyLineEdit();

	virtual void SetText(const std::string &sUtf8Text) override;

	void SetInputValidator(const std::regex &regEx);
	void ClearInputValidator();

	bool IsCursorShown() const;
	uint32 GetCursorIndex() const;
	uint32 GetSelectionIndex() const; // Anything between GetCursorIndex() and GetSelectionIndex() will be selected. Will equal when no selection.
	void GetSelection(uint32 &uiStartIndexOut, uint32 &uiEndIndexOut) const;
	
	void SetCursor(uint32 uiCharIndex);
	void SetCursor(uint32 uiCharIndex, uint32 uiSelectionIndex);

protected:
	virtual void OnUiTextInput(std::string sNewUtf8Text) override;
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) override;
	virtual void OnUiMouseClicked() override;

	virtual void OnTakeKeyboardFocus() override;
	virtual void OnRelinquishKeyboardFocus() override;

	virtual void OnSetup() override;

	void MoveCursor(int32 iOffset, bool bSelection);
	void ToggleCursorVisible(bool bForceShown);
	static void OnCursorTimer(void *pThisData);
};

#endif /* HyLineEdit_h__ */
