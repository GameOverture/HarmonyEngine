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
	SetMonospacedDigits(true);
}

HyRackMeter::HyRackMeter(const HyPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, pParent),
	m_SpinText(this)
{
	SetMonospacedDigits(true);
}

HyRackMeter::HyRackMeter(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, textNodePath, pParent),
	m_SpinText(this)
{
	SetMonospacedDigits(true);
}

HyRackMeter::HyRackMeter(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, textNodePath, textMargins, pParent),
	m_SpinText(this)
{
	SetMonospacedDigits(true);
}

/*virtual*/ HyRackMeter::~HyRackMeter()
{
}

int64 HyRackMeter::GetValue()
{
	return m_iTargetValue;
}

void HyRackMeter::SetValue(int64 iValue, float fRackDuration)
{
	m_iTargetValue = iValue;

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

	SetAssembleNeeded();
	Assemble(); // Do a full Assemble() right now in order to position the spin text's padded symbols, as HyRackMeter::Update() assumes they are already in place
}

void HyRackMeter::OffsetValue(int64 iOffsetAmt, float fRackDuration)
{
	SetValue(m_iTargetValue + iOffsetAmt, fRackDuration);
}

void HyRackMeter::Slam()
{
	SetValue(m_iTargetValue, 0.0f);
}

bool HyRackMeter::IsRacking() const
{
	return m_iCurValue != m_iTargetValue;
}

bool HyRackMeter::IsShowAsCash() const
{
	return m_uiAttribs & RACKMETERATTRIB_IsMoney;
}

/*virtual*/ void HyRackMeter::ShowAsCash(bool bShow)
{
	if(bShow)
		m_uiAttribs |= RACKMETERATTRIB_IsMoney;
	else
		m_uiAttribs &= ~RACKMETERATTRIB_IsMoney;

	SetAssembleNeeded();
}

bool HyRackMeter::IsSpinningMeter() const
{
	return m_uiAttribs & RACKMETERATTRIB_IsSpinDigits;
}

void HyRackMeter::SetAsSpinningMeter(bool bSet)
{
	if(bSet)
		m_uiAttribs |= RACKMETERATTRIB_IsSpinDigits;
	else
		m_uiAttribs &= ~RACKMETERATTRIB_IsSpinDigits;

	SetAssembleNeeded();
}

HyNumberFormat HyRackMeter::GetNumFormat() const
{
	return m_NumberFormat;
}

void HyRackMeter::SetNumFormat(HyNumberFormat format)
{
	m_NumberFormat = format;
	SetAssembleNeeded();
}

uint32 HyRackMeter::GetDenomination() const
{
	return m_uiDenomination;
}

void HyRackMeter::SetDenomination(uint32 uiDenom)
{
	m_uiDenomination = uiDenom;
	SetAssembleNeeded();
}

/*virtual*/ void HyRackMeter::SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor) /*override*/
{
	HyLabel::SetTextLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);

	m_SpinText.m_SpinText_Shown.SetLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);
	m_SpinText.m_SpinText_Padded.SetLayerColor(uiStateIndex, uiLayerIndex, topColor, botColor);
}

/*virtual*/ void HyRackMeter::Update() /*override*/
{
	HyLabel::Update();

	if(m_iCurValue == m_iTargetValue)
		return;

	m_fElapsedTimeRack = HyMath::Clamp(m_fElapsedTimeRack + HyEngine::DeltaTime(), 0.0f, m_fRackingDuration);
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
			SetAssembleNeeded();
		}
	}
	else // Spinning (analog) digits
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
			SetAssembleNeeded(); //FormatDigits();
	}
}

/*virtual*/ void HyRackMeter::OnAssemble() /*override*/
{
	HyLabel::OnAssemble();

	m_SpinText.Setup(m_Text.GetPath());

	m_SpinText.m_SpinText_Shown.SetMonospacedDigits(m_Text.IsMonospacedDigits());
	m_SpinText.m_SpinText_Padded.SetMonospacedDigits(m_Text.IsMonospacedDigits());

	m_SpinText.m_SpinText_Shown.SetState(m_Text.GetState());
	m_SpinText.m_SpinText_Shown.SetAlignment(m_Text.GetAlignment());

	m_SpinText.m_SpinText_Padded.SetState(m_Text.GetState());
	m_SpinText.m_SpinText_Padded.SetAlignment(m_Text.GetAlignment());

	m_SpinText.pos.Set(m_Text.pos);
	if(m_Text.IsScaleBox())
	{
		m_SpinText.m_SpinText_Shown.SetAsScaleBox(m_Text.GetTextBoxDimensions().x, m_Text.GetTextBoxDimensions().y);
		m_SpinText.m_SpinText_Padded.SetAsScaleBox(m_Text.GetTextBoxDimensions().x, m_Text.GetTextBoxDimensions().y);
	}
	else // TODO: Implement this properly
	{
		m_SpinText.m_SpinText_Shown.SetAsLine();
		m_SpinText.m_SpinText_Padded.SetAsLine();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Format Digits
	m_NumberFormat.SetUsingCurrencySymbol(m_Text.IsCharacterAvailable(HyLocale::Money_GetCurrencySymbol()));

	if(IsShowAsCash())
		m_Text.SetText(HyLocale::Money_Format(m_iCurValue, m_NumberFormat));
	else
		m_Text.SetText(HyLocale::Number_Format(static_cast<int64>(m_iCurValue / m_uiDenomination), m_NumberFormat));

	// HyLocale::*_Format() should not produce empty strings
	HyAssert(m_Text.GetNumCharacters() != 0, "HyRackMeter - EMPTY STRING! " << "IsCash: " << (IsShowAsCash() ? "true" : "false") << ", Value: " << m_iCurValue);

	if(IsSpinningMeter())
	{
		float fThreshold = m_Text.IsScaleBox() ? m_Text.GetTextBoxDimensions().y : m_Text.GetLineBreakHeight();

		m_SpinText.m_SpinText_Shown.SetText(m_Text.GetUtf8String());
		uint32 uiCharIndexForScissor = m_Text.GetNumCharacters() - 1;

		if(m_iCurValue <= m_iTargetValue)
		{
			if(IsShowAsCash())
				m_SpinText.m_SpinText_Padded.SetText(HyLocale::Money_Format(m_iCurValue + 1, m_NumberFormat));
			else
				m_SpinText.m_SpinText_Padded.SetText(HyLocale::Number_Format(static_cast<int64>((m_iCurValue + 1) / m_uiDenomination), m_NumberFormat));

			m_SpinText.m_SpinText_Padded.pos.Y(m_SpinText.m_SpinText_Shown.pos.Y() - fThreshold);

			// Find and assign `uiCharIndexForScissor` value
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
				m_SpinText.m_SpinText_Padded.SetText(HyLocale::Number_Format(static_cast<int64>((m_iCurValue - 1) / m_uiDenomination), m_NumberFormat));

			m_SpinText.m_SpinText_Padded.pos.Y(m_SpinText.m_SpinText_Shown.pos.Y() + fThreshold);

			// Find and assign `uiCharIndexForScissor` value
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
					m_Text.SetCharacterAlpha(i, 1.0f);
					m_SpinText.m_SpinText_Shown.SetCharacterAlpha(i, 0.0f);
					m_SpinText.m_SpinText_Padded.SetCharacterAlpha(i, 0.0f);
				}
				else
				{
					m_Text.SetCharacterAlpha(i, 0.0f);
					m_SpinText.m_SpinText_Shown.SetCharacterAlpha(i, 1.0f);
					m_SpinText.m_SpinText_Padded.SetCharacterAlpha(i, 1.0f);
				}
			}
			else
			{
				m_Text.SetCharacterAlpha(i, 1.0f);
				m_SpinText.m_SpinText_Shown.SetCharacterAlpha(i, 0.0f);
				m_SpinText.m_SpinText_Padded.SetCharacterAlpha(i, 0.0f);
			}
		}
	}
	else
	{
		for(uint32 i = 0; i < m_Text.GetNumCharacters(); ++i)
			m_Text.SetCharacterAlpha(i, 1.0f);
	}

	m_SpinText.SetVisible(m_Text.IsVisible() && IsSpinningMeter());

	if (IsSpinningMeter())
		m_SpinText.SetScissor(HyRect(/*GetWidth()*/99999, m_Text.GetHeight()));
	else
		m_SpinText.ClearScissor(true);
}

float HyRackMeter::GetSpinHeightThreshold()
{
	Assemble();

	if(m_Text.IsScaleBox())
		return m_Text.GetTextBoxDimensions().y;

	return m_Text.GetLineBreakHeight();
}
