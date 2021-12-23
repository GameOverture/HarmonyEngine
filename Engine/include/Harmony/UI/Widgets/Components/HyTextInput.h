/**************************************************************************
*	HyTextInput.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyTextInput_h__
#define HyTextInput_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"
#include "Time/Watches/HyTimer.h"
#include "Input/HyInput.h"

class HyTextInput : public HyEntity2d
{
protected:
	HyText2d &										m_TextRef;

	int32											m_iCursorIndex;			// Cursor index in full UTF-8 characters
	int32											m_iSelectionLength;		// Selection length in full UTF-8 characters
	HyPrimitive2d									m_Selection;			// Actual text selection highlight visual
	HyPrimitive2d									m_PrimCursor;			// Shows a standard vertical line draw with a primitive

	HyTimer											m_BlinkTimer;

public:
	HyTextInput(HyText2d &text2dRef, HyEntity2d *pParent = nullptr);
	virtual ~HyTextInput();
	
	void SetCursor(int32 iUtf8CharIndex, int32 iSelectionLen);

	void OnTakeKeyboardFocus();
	void OnRelinquishKeyboardFocus();

	void OnUiTextInput(std::string sNewUtf8Text);
	void OnUiKeyboardInput(HyKeyboardBtn eBtn);

protected:
	static void OnCursorTimer(void *pThisData);
};

#endif /* HyTextInput_h__ */
