///**************************************************************************
//*	HyTextUi.cpp
//*
//*	Harmony Engine
//*	Copyright (c) 2021 Jason Knobler
//*
//*	Harmony License:
//*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
//*************************************************************************/
//#include "Afx/HyStdAfx.h"
//#include "UI/Components/HyTextUi.h"
//#include "HyEngine.h"
//
//HyTextUi::HyTextUi(HyEntity2d *pParent /*= nullptr*/) :
//	HyEntityLeaf2d<HyText2d>(pParent),
//	
//{
//}
//
//HyTextUi::HyTextUi(std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
//	HyEntityLeaf2d<HyText2d>(sTextPrefix, sTextName, pParent),
//	m_iCursorIndex(0),
//	m_iSelectionLength(0),
//	m_Selection(this),
//	m_PrimCursor(this)
//{
//	
//
//	
//}
//
///*virtual*/ HyTextUi::~HyTextUi()
//{
//}
//
//const std::string &HyTextUi::GetUtf8String() const
//{
//	return m_Leaf.GetUtf8String();
//}
//
//void HyTextUi::SetText(const std::string &sUtf8Text)
//{
//	m_Leaf.SetText(sUtf8Text);
//	SetCursor(sUtf8Text.length(), 0);
//}
//
//void HyTextUi::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
//{
//	m_Leaf.SetLayerColor(uiLayerIndex, fR, fG, fB);
//}
//
//HyAlignment HyTextUi::GetTextAlignment() const
//{
//	return m_Leaf.GetTextAlignment();
//}
//
//void HyTextUi::SetTextAlignment(HyAlignment eAlignment)
//{
//	m_Leaf.SetTextAlignment(eAlignment);
//}
//
//float HyTextUi::GetTextWidth(bool bIncludeScaling /*= true*/)
//{
//	return m_Leaf.GetTextWidth(bIncludeScaling);
//}
//
//float HyTextUi::GetTextHeight(bool bIncludeScaling /*= true*/)
//{
//	return m_Leaf.GetTextHeight(bIncludeScaling);
//}
//
//uint32 HyTextUi::GetNumCharacters() const
//{
//	return m_Leaf.GetNumCharacters();
//}
//
//uint32 HyTextUi::GetCharacterCode(uint32 uiCharIndex) const
//{
//	return m_Leaf.GetCharacterCode(uiCharIndex);
//}
//
//void HyTextUi::SetGlyphAlpha(uint32 uiCharIndex, float fAlpha)
//{
//	m_Leaf.SetGlyphAlpha(uiCharIndex, fAlpha);
//}
//
//glm::vec2 HyTextUi::GetTextCursorPos()
//{
//	return m_Leaf.GetTextCursorPos();
//}
//
//glm::vec2 HyTextUi::GetTextBottomLeft()
//{
//	return m_Leaf.GetTextBottomLeft();
//}
//
//bool HyTextUi::IsMonospacedDigits() const
//{
//	return m_Leaf.IsMonospacedDigits();
//}
//
//void HyTextUi::SetMonospacedDigits(bool bSet)
//{
//	m_Leaf.SetMonospacedDigits(bSet);
//}
//
//const glm::vec2 &HyTextUi::GetTextBoxDimensions() const
//{
//	return m_Leaf.GetTextBoxDimensions();
//}
//
//void HyTextUi::SetAsLine()
//{
//	m_Leaf.SetAsLine();
//}
//
//void HyTextUi::SetAsColumn(float fWidth, bool bSplitWordsToFit /*= false*/)
//{
//	m_Leaf.SetAsColumn(fWidth, bSplitWordsToFit);
//}
//
//void HyTextUi::SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically /*= true*/)
//{
//	m_Leaf.SetAsScaleBox(fWidth, fHeight, bCenterVertically);
//}
//
//void HyTextUi::OnTakeKeyboardFocus()
//{
//	
//}
//
//void HyTextUi::OnRelinquishKeyboardFocus()
//{
//	
//}
//
//void HyTextUi::OnUiTextInput(std::string sNewUtf8Text)
//{
//
//}
//
//void HyTextUi::OnUiKeyboardInput(HyKeyboardBtn eBtn)
//{
//
//}
