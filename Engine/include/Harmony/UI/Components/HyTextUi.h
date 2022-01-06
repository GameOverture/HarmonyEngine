///**************************************************************************
//*	HyTextUi.h
//*
//*	Harmony Engine
//*	Copyright (c) 2021 Jason Knobler
//*
//*	Harmony License:
//*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
//*************************************************************************/
//#ifndef HyTextUi_h__
//#define HyTextUi_h__
//
//#include "Afx/HyStdAfx.h"
//#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntityLeaf2d.h"
//#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

//#include "Input/HyInput.h"
//
//class HyTextUi : public HyEntityLeaf2d<HyText2d>
//{
//protected:
//
//
//public:
//	HyTextUi(HyEntity2d *pParent = nullptr);
//	HyTextUi(std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
//	virtual ~HyTextUi();
//
//	const std::string &GetUtf8String() const;
//	void SetText(const std::string &sUtf8Text);
//	void SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);
//
//	HyAlignment GetTextAlignment() const;
//	void SetTextAlignment(HyAlignment eAlignment);
//
//	float GetTextWidth(bool bIncludeScaling = true);
//	float GetTextHeight(bool bIncludeScaling = true);
//
//	uint32 GetNumCharacters() const;
//
//	uint32 GetCharacterCode(uint32 uiCharIndex) const;
//	void SetGlyphAlpha(uint32 uiCharIndex, float fAlpha);
//
//	glm::vec2 GetTextCursorPos();
//	glm::vec2 GetTextBottomLeft();
//
//	bool IsMonospacedDigits() const;
//	void SetMonospacedDigits(bool bSet);
//
//	const glm::vec2 &GetTextBoxDimensions() const;
//
//	void SetAsLine();
//	void SetAsColumn(float fWidth, bool bSplitWordsToFit = false);
//	void SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically = true);
//	
//	bool IsCursorShown() const;
//	void SetCursor(int32 iUtf8CharIndex, int32 iSelectionLen);
//
//	void OnTakeKeyboardFocus();
//	void OnRelinquishKeyboardFocus();
//
//	void OnUiTextInput(std::string sNewUtf8Text);
//	void OnUiKeyboardInput(HyKeyboardBtn eBtn);
//
//protected:
//	static void OnCursorTimer(void *pThisData);
//};
//
//#endif /* HyTextUi_h__ */
