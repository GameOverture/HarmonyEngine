/**************************************************************************
*	HyTextUi.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Components/HyTextUi.h"
#include "HyEngine.h"

#define HyTextUi_BLINKDUR 0.5f

HyTextUi::HyTextUi(HyEntity2d *pParent /*= nullptr*/) :
	HyEntityLeaf2d<HyText2d>(pParent),
	m_iCursorIndex(0),
	m_iSelectionLength(0),
	m_Selection(this),
	m_PrimCursor(this)
{
}

HyTextUi::HyTextUi(std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyEntityLeaf2d<HyText2d>(sTextPrefix, sTextName, pParent),
	m_iCursorIndex(0),
	m_iSelectionLength(0),
	m_Selection(this),
	m_PrimCursor(this)
{
	SetCursor(m_iCursorIndex, m_iSelectionLength);

	m_BlinkTimer.SetExpiredCallback(OnCursorTimer, this);
	m_BlinkTimer.Init(HyTextUi_BLINKDUR);
}

/*virtual*/ HyTextUi::~HyTextUi()
{
}

const std::string &HyTextUi::GetUtf8String() const
{
	return m_Leaf.GetUtf8String();
}

void HyTextUi::SetText(const std::string &sUtf8Text)
{
	m_Leaf.SetText(sUtf8Text);
	SetCursor(sUtf8Text.length(), 0);
}

void HyTextUi::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	m_Leaf.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

HyAlignment HyTextUi::GetTextAlignment() const
{
	return m_Leaf.GetTextAlignment();
}

void HyTextUi::SetTextAlignment(HyAlignment eAlignment)
{
	m_Leaf.SetTextAlignment(eAlignment);
}

float HyTextUi::GetTextWidth(bool bIncludeScaling /*= true*/)
{
	return m_Leaf.GetTextWidth(bIncludeScaling);
}

float HyTextUi::GetTextHeight(bool bIncludeScaling /*= true*/)
{
	return m_Leaf.GetTextHeight(bIncludeScaling);
}

uint32 HyTextUi::GetNumCharacters() const
{
	return m_Leaf.GetNumCharacters();
}

uint32 HyTextUi::GetCharacterCode(uint32 uiCharIndex) const
{
	return m_Leaf.GetCharacterCode(uiCharIndex);
}

void HyTextUi::SetGlyphAlpha(uint32 uiCharIndex, float fAlpha)
{
	m_Leaf.SetGlyphAlpha(uiCharIndex, fAlpha);
}

glm::vec2 HyTextUi::GetTextCursorPos()
{
	return m_Leaf.GetTextCursorPos();
}

glm::vec2 HyTextUi::GetTextBottomLeft()
{
	return m_Leaf.GetTextBottomLeft();
}

bool HyTextUi::IsMonospacedDigits() const
{
	return m_Leaf.IsMonospacedDigits();
}

void HyTextUi::SetMonospacedDigits(bool bSet)
{
	m_Leaf.SetMonospacedDigits(bSet);
}

const glm::vec2 &HyTextUi::GetTextBoxDimensions() const
{
	return m_Leaf.GetTextBoxDimensions();
}

void HyTextUi::SetAsLine()
{
	m_Leaf.SetAsLine();
}

void HyTextUi::SetAsColumn(float fWidth, bool bSplitWordsToFit /*= false*/)
{
	m_Leaf.SetAsColumn(fWidth, bSplitWordsToFit);
}

void HyTextUi::SetAsScaleBox(float fWidth, float fHeight, bool bCenterVertically /*= true*/)
{
	m_Leaf.SetAsScaleBox(fWidth, fHeight, bCenterVertically);
}

bool HyTextUi::IsCursorShown() const
{
	return m_BlinkTimer.IsRunning();
}


void HyTextUi::SetCursor(int32 iUtf8CharIndex, int32 iSelectionLen)
{
	m_iCursorIndex = iUtf8CharIndex;
	m_iSelectionLength = iSelectionLen;

	if(m_Leaf.IsLoadDataValid() == false || IsCursorShown() == false)
	{
		m_Selection.SetAsNothing();
		m_PrimCursor.SetAsNothing();
		return;
	}

	const HyText2dData *pTextData = static_cast<const HyText2dData *>(m_Leaf.AcquireData());
	float fCursorHeight = pTextData->GetLineHeight(m_Leaf.GetState());
	
	if(m_iSelectionLength > 0)
	{
		float fWidth = m_Leaf.GetGlyphOffset(m_iCursorIndex + m_iSelectionLength, 0).x - m_Leaf.GetGlyphOffset(m_iCursorIndex, 0).x;
		m_Selection.SetAsBox(fWidth, fCursorHeight);
	}
	else
		m_Selection.SetAsNothing();
	m_PrimCursor.SetAsBox(2.0f, fCursorHeight);

	m_Selection.SetDisplayOrder(m_Leaf.GetDisplayOrder() - 1);
	m_PrimCursor.SetDisplayOrder(m_Leaf.GetDisplayOrder() + 1);

	//if(m_iCursorIndex == m_Leaf.GetNumShownCharacters())
	//	m_PrimCursor.pos.Set(m_Leaf.GetTextCursorPos());
	//else
	//	m_PrimCursor.pos.Set(m_Leaf.GetGlyphOffset(m_iCursorIndex, 0));

	// 
	m_Selection.alpha.Set(m_BlinkTimer.IsRunning() * 1.0f);
	m_PrimCursor.alpha.Set(m_BlinkTimer.IsRunning() * 1.0f);
}

void HyTextUi::OnTakeKeyboardFocus()
{
	HyEngine::Input().StartTextInput();
	m_BlinkTimer.InitStart(HyTextUi_BLINKDUR);
	alpha.Set(1.0f);
}

void HyTextUi::OnRelinquishKeyboardFocus()
{
	HyEngine::Input().StopTextInput();
	m_BlinkTimer.Reset();
	alpha.Set(0.0f);
}

void HyTextUi::OnUiTextInput(std::string sNewUtf8Text)
{
	std::string sText = m_Leaf.GetUtf8String();

	if(m_iCursorIndex == HyIO::Utf8Length(sText))
		sText += sNewUtf8Text;
	else
	{
		if(m_iSelectionLength > 0)
		{
			HyIO::Utf8Erase(sText, m_iCursorIndex, m_iSelectionLength);
			m_iSelectionLength = 0;
		}
		
		HyIO::Utf8Insert(sText, m_iCursorIndex, sNewUtf8Text);
	}
	m_iCursorIndex += HyIO::Utf8Length(sNewUtf8Text);

	m_Leaf.SetText(sText);

	m_BlinkTimer.InitStart(HyTextUi_BLINKDUR);
	alpha.Set(1.0f);
}

void HyTextUi::OnUiKeyboardInput(HyKeyboardBtn eBtn)
{
	switch(eBtn)
	{
	case HYKEY_Enter:
		break;

	case HYKEY_BackSpace: {
		std::string sText = m_Leaf.GetUtf8String();
		do
		{
			if(sText.empty())
				break;

			if((sText.back() & 0x80) == 0x00) // One byte
			{
				sText.pop_back();
				break;
			}

			if((sText.back() & 0xC0) == 0x80) // Byte from the multi-byte sequence
				sText.pop_back();
			if((sText.back() & 0xC0) == 0xC0) // First byte of multi-byte sequence (now UTF-8 character fully deleted)
			{
				sText.pop_back();
				break;
			}

		} while(true);
		m_Leaf.SetText(sText);
		break; }
	}

	m_BlinkTimer.InitStart(HyTextUi_BLINKDUR);
	alpha.Set(1.0f);
}

/*static*/ void HyTextUi::OnCursorTimer(void *pThisData)
{
	HyTextUi *pThis = static_cast<HyTextUi *>(pThisData);

	if(pThis->alpha.Get() == 0.0f)
		pThis->alpha.Set(1.0f);
	else
		pThis->alpha.Set(0.0f);

	pThis->m_BlinkTimer.InitStart(HyTextUi_BLINKDUR);
}
