/**************************************************************************
*	HyTextField.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTextField_h__
#define HyTextField_h__

#include "Afx/HyStdAfx.h"
#include "Gui/Widgets/HyLabel.h"
#include "Time/Watches/HyTimer.h"

#include <regex>

class HyTextField : public HyLabel
{
protected:
	enum TextFieldAttributes
	{
		TEXTFIELDATTRIB_UseValidator = 1 << 22,
		
		TEXTFIELDATTRIB_NEXTFLAG = 1 << 23
	};
	static_assert((int)TEXTFIELDATTRIB_UseValidator == (int)LABELATTRIB_NEXTFLAG, "HyTextField is not matching with base classes attrib flags");

	std::string							m_sValidationRegEx;
	std::regex							m_InputValidator;

	uint32								m_uiCursorIndex;		// Cursor index in full UTF-8 characters
	uint32								m_uiSelectionIndex;		// Selection index in full UTF-8 characters. Anything between m_iCursorIndex and m_iSelectionIndex will be selected. Will equal when no selection.
	HyPrimitive2d						m_Selection;			// Actual text selection highlight visual

	HyPrimitive2d						m_Cursor;				// Shows a standard vertical line draw with a primitive
	HyTimer								m_BlinkTimer;

	std::function<void(HyTextField *)>	m_fpOnSubmit;			// Called when the user presses enter, or invokes Submit()

public:
	HyTextField(HyEntity2d *pParent = nullptr);
	HyTextField(const HyUiPanelInit &initRef, const HyUiTextInit &textInit, HyEntity2d *pParent = nullptr);
	virtual ~HyTextField();

	virtual void SetText(const std::string &sUtf8Text) override;

	bool IsInputValidated() const;
	std::string GetInputValidator() const;
	void SetInputValidator(std::string sRegEx, bool bCaseSensitive);
	void ClearInputValidator();

	bool IsCursorShown() const;
	uint32 GetCursorIndex() const;
	uint32 GetSelectionIndex() const; // Anything between GetCursorIndex() and GetSelectionIndex() will be selected. Will equal when no selection.
	void GetSelection(uint32 &uiStartIndexOut, uint32 &uiEndIndexOut) const;
	
	void SetCursor(uint32 uiCharIndex);
	void SetCursor(uint32 uiCharIndex, uint32 uiSelectionIndex);

	void SetOnSubmit(std::function<void(HyTextField *)> fpOnSubmit);
	void Submit();

protected:
	virtual void OnUiTextInput(std::string sNewUtf8Text) override;
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) override;

	virtual void OnTakeKeyboardFocus() override;
	virtual void OnRelinquishKeyboardFocus() override;

	void MoveCursor(int32 iOffset, bool bSelection);

	virtual void OnApplyWidgetState(HyPanelState eWidgetState) override;

//private:
//	using HyLabel::SetAsStacked;
//	using HyLabel::SetAsSideBySide;

};

#endif /* HyTextField_h__ */
