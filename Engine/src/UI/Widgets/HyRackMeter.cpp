/**************************************************************************
*	HyRackMeter.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyRackMeter.h"
#include "HyEngine.h"

HyRackMeter::HyRackMeter(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_SpinText(this)
{
}

HyRackMeter::HyRackMeter(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, pParent),
	m_SpinText(this)
{
	OnSetup();
}

HyRackMeter::HyRackMeter(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(initRef, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_SpinText(this)
{
	OnSetup();
}

/*virtual*/ HyRackMeter::~HyRackMeter()
{
}

/*virtual*/ void HyRackMeter::SetAsStacked(HyAlignment eTextAlignment /*= HYALIGN_HCenter*/, bool bUseScaleBox /*= true*/) /*override*/
{
	HyLabel::SetAsStacked(eTextAlignment, bUseScaleBox);

	m_SpinText.m_SpinText_Shown.SetTextAlignment(eTextAlignment);
	m_SpinText.m_SpinText_Padded.SetTextAlignment(eTextAlignment);

	FormatDigits();
}

int64 HyRackMeter::GetValue()
{
	return m_iTargetValue;
}

void HyRackMeter::SetValue(int64 iPennies, float fRackDuration)
{
	m_iTargetValue = iPennies;

	m_fRackingDuration = fRackDuration;
	m_fElapsedTimeRack = 0.0f;
	m_dPrevDistance = 0.0;

	if(m_fRackingDuration <= 0.0f)
	{
		m_iCurValue = m_iPrevValue = m_iTargetValue;
		m_dTotalDistance = 0.0;
		m_fThresholdDist = 0.0f;

		m_SpinText.m_SpinText_Shown.pos.Y(0.0f);
		m_SpinText.m_SpinText_Padded.pos.Y(-GetSpinHeightThreshold());
	}
	else
	{
		m_iPrevValue = m_iCurValue;
		m_dTotalDistance = abs(m_iTargetValue - m_iCurValue) * static_cast<double>(GetSpinHeightThreshold());
		m_dTotalDistance -= m_fThresholdDist;
	}

	FormatDigits();
}

void HyRackMeter::OffsetValue(int64 iPenniesOffsetAmt, float fRackDuration)
{
	SetValue(m_iTargetValue + iPenniesOffsetAmt, fRackDuration);
}

void HyRackMeter::Slam()
{
	SetValue(m_iTargetValue, 0.0f);
}

bool HyRackMeter::IsRacking()
{
	return m_iCurValue != m_iTargetValue;
}

bool HyRackMeter::IsShowAsCash()
{
	return m_uiAttribs & RACKMETERATTRIB_IsMoney;
}

void HyRackMeter::ShowAsCash(bool bShow)
{
	if(bShow)
		m_uiAttribs |= RACKMETERATTRIB_IsMoney;
	else
		m_uiAttribs &= ~RACKMETERATTRIB_IsMoney;

	FormatDigits();
}

bool HyRackMeter::IsSpinningMeter()
{
	return m_uiAttribs & RACKMETERATTRIB_IsSpinDigits;
}

void HyRackMeter::SetAsSpinningMeter(bool bSet)
{
	if(bSet)
		m_uiAttribs |= RACKMETERATTRIB_IsSpinDigits;
	else
		m_uiAttribs &= ~RACKMETERATTRIB_IsSpinDigits;

	FormatDigits();
}

HyNumberFormat HyRackMeter::GetNumFormat() const
{
	return m_NumberFormat;
}

void HyRackMeter::SetNumFormat(HyNumberFormat format)
{
	m_NumberFormat = format;
	FormatDigits();
}

/*virtual*/ void HyRackMeter::SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor) /*override*/
{
	HyLabel::SetTextLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);

	m_SpinText.m_SpinText_Shown.SetLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);
	m_SpinText.m_SpinText_Padded.SetLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);
}

/*virtual*/ void HyRackMeter::OnUpdate() /*override*/
{
	if(m_iCurValue == m_iTargetValue)
		return;

	m_fElapsedTimeRack = HyClamp(m_fElapsedTimeRack + HyEngine::DeltaTime(), 0.0f, m_fRackingDuration);

	if(m_fElapsedTimeRack == m_fRackingDuration)
	{
		Slam();
		return;
	}

	if(IsSpinningMeter() == false)
	{
		// Standard non-spinning rack
		int64 iCurPennies = static_cast<int64>(static_cast<double>(m_iTargetValue - m_iPrevValue) * (m_fElapsedTimeRack / m_fRackingDuration) + m_iPrevValue);
		if(iCurPennies != m_iCurValue)
		{
			m_iCurValue = iCurPennies;
			FormatDigits();
		}
	}
	else	// Spinning (analog) digits
	{
		float fThreshold = GetSpinHeightThreshold();

		double dTravelDist = m_dTotalDistance * (m_fElapsedTimeRack / m_fRackingDuration);
		m_fThresholdDist += static_cast<float>(dTravelDist - m_dPrevDistance);
		m_dPrevDistance = dTravelDist;

		int32 iTimesPastThreshold = static_cast<int32>(m_fThresholdDist / fThreshold);
		m_fThresholdDist -= (iTimesPastThreshold * fThreshold);

		if(m_iTargetValue >= m_iCurValue)
		{
			m_SpinText.m_SpinText_Shown.pos.Y(m_fThresholdDist);
			m_SpinText.m_SpinText_Padded.pos.Y(m_fThresholdDist);

			m_iCurValue += iTimesPastThreshold;
			m_SpinText.m_SpinText_Padded.pos.Offset(0.0f, -fThreshold);
		}
		else
		{
			m_SpinText.m_SpinText_Shown.pos.Y(-m_fThresholdDist);
			m_SpinText.m_SpinText_Padded.pos.Y(-m_fThresholdDist);

			m_iCurValue -= iTimesPastThreshold;
			m_SpinText.m_SpinText_Padded.pos.Offset(0.0f, fThreshold);
		}

		if(iTimesPastThreshold > 0)
			FormatDigits();
	}
}

/*virtual*/ void HyRackMeter::OnSetup() /*override*/
{
	m_SpinText.Setup(m_Text.GetPrefix(), m_Text.GetName());
	FormatDigits();
}

/*virtual*/ void HyRackMeter::ResetTextAndPanel() /*override*/
{
	HyLabel::ResetTextAndPanel();

	m_SpinText.m_SpinText_Shown.SetState(m_Text.GetState());
	m_SpinText.m_SpinText_Padded.SetState(m_Text.GetState());

	m_Text.SetMonospacedDigits(true);

	m_SpinText.pos.Set(m_Text.pos);
	if(m_Text.IsScaleBox())
	{
		m_SpinText.m_SpinText_Shown.SetAsScaleBox(m_Text.GetTextBoxDimensions().x, m_Text.GetTextBoxDimensions().y);
		m_SpinText.m_SpinText_Padded.SetAsScaleBox(m_Text.GetTextBoxDimensions().x, m_Text.GetTextBoxDimensions().y);
	}
	else
	{
		m_SpinText.m_SpinText_Shown.SetAsLine();
		m_SpinText.m_SpinText_Padded.SetAsLine();
	}

	FormatDigits();
}

float HyRackMeter::GetSpinHeightThreshold()
{
	if(m_Text.IsScaleBox())
		return m_Text.GetTextBoxDimensions().y;

	return m_Text.GetLineHeight();
}

void HyRackMeter::FormatDigits()
{
	if(IsShowAsCash())
		m_Text.SetText(HyLocale::Money_Format(m_iCurValue, m_NumberFormat));
	else
		m_Text.SetText(HyLocale::Number_Format(m_iCurValue, m_NumberFormat));

	// HyLocale::*_Format() should not produce empty strings
	HyAssert(m_Text.GetNumCharacters() != 0, "HyRackMeter - EMPTY STRING! " << "IsCash: " << (IsShowAsCash() ? "true" : "false") << ", Value: " << m_iCurValue);

	if(IsSpinningMeter())
	{
		float fThreshold = GetSpinHeightThreshold();

		m_SpinText.m_SpinText_Shown.SetText(m_Text.GetUtf8String());
		uint32 uiCharIndexForScissor = m_Text.GetNumCharacters() - 1;

		if(m_iCurValue <= m_iTargetValue)
		{
			if(IsShowAsCash())
				m_SpinText.m_SpinText_Padded.SetText(HyLocale::Money_Format(m_iCurValue + 1, m_NumberFormat));
			else
				m_SpinText.m_SpinText_Padded.SetText(HyLocale::Number_Format(m_iCurValue + 1, m_NumberFormat));

			m_SpinText.m_SpinText_Padded.pos.Y(m_SpinText.m_SpinText_Shown.pos.Y() - fThreshold);

			for(; uiCharIndexForScissor > 0; --uiCharIndexForScissor)
			{
				uint32 uiCharCode = m_Text.GetCharacterCode(uiCharIndexForScissor);
				if(uiCharCode >= 48 && uiCharCode <= 57)
				{
					if(uiCharCode != '9')
						break;
				}
			}
		}
		else
		{
			if(IsShowAsCash())
				m_SpinText.m_SpinText_Padded.SetText(HyLocale::Money_Format(m_iCurValue - 1, m_NumberFormat));
			else
				m_SpinText.m_SpinText_Padded.SetText(HyLocale::Number_Format(m_iCurValue - 1, m_NumberFormat));

			m_SpinText.m_SpinText_Padded.pos.Y(m_SpinText.m_SpinText_Shown.pos.Y() + fThreshold);

			for(; uiCharIndexForScissor > 0; --uiCharIndexForScissor)
			{
				uint32 uiCharCode = m_Text.GetCharacterCode(uiCharIndexForScissor);
				if(uiCharCode >= 48 && uiCharCode <= 57)
				{
					if(uiCharCode != '0')
						break;
				}
			}
		}

		for(uint32 i = 0; i < m_Text.GetNumCharacters(); ++i)
		{
			uint32 uiCharCode = m_Text.GetCharacterCode(i);
			if(uiCharCode >= 48 && uiCharCode <= 57)
			{
				if(i < uiCharIndexForScissor)
				{
					m_Text.SetGlyphAlpha(i, 1.0f);
					m_SpinText.m_SpinText_Shown.SetGlyphAlpha(i, 0.0f);
					m_SpinText.m_SpinText_Padded.SetGlyphAlpha(i, 0.0f);
				}
				else
				{
					m_Text.SetGlyphAlpha(i, 0.0f);
					m_SpinText.m_SpinText_Shown.SetGlyphAlpha(i, 1.0f);
					m_SpinText.m_SpinText_Padded.SetGlyphAlpha(i, 1.0f);
				}
			}
			else
			{
				m_Text.SetGlyphAlpha(i, 1.0f);
				m_SpinText.m_SpinText_Shown.SetGlyphAlpha(i, 0.0f);
				m_SpinText.m_SpinText_Padded.SetGlyphAlpha(i, 0.0f);
			}
		}

		uint32 uiWidth;
		if(m_Text.IsScaleBox())
			uiWidth = m_Text.GetTextBoxDimensions().x;
		else
			uiWidth = m_Text.GetTextWidth(true);

		m_SpinText.SetScissor(0, 0, uiWidth, static_cast<uint32>(fThreshold));
	}
	else
	{
		for(uint32 i = 0; i < m_Text.GetNumCharacters(); ++i)
			m_Text.SetGlyphAlpha(i, 1.0f);
	}

	m_SpinText.SetVisible(m_Text.IsVisible() && IsSpinningMeter());
}