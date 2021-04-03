/**************************************************************************
*	HyMeter.cpp
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyMeter.h"
#include "HyEngine.h"

HyMeter::HyMeter(HyEntity2d *pParent /*= nullptr*/) :
	HyWidget(pParent),
	m_SpinText(this)
{
}

HyMeter::HyMeter(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyWidget(fWidth, fHeight, fStroke, sTextPrefix, sTextName, pParent),
	m_SpinText(this)
{
	OnSetup("", "", sTextPrefix, sTextName, 0, 0, 0, 0);
}

HyMeter::HyMeter(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyWidget(fWidth, fHeight, fStroke, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_SpinText(this)
{
	OnSetup("", "", sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

HyMeter::HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyWidget(sPanelPrefix, sPanelName, sTextPrefix, sTextName, pParent),
	m_SpinText(this)
{
	
	OnSetup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0);
}

HyMeter::HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent /*= nullptr*/) :
	HyWidget(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY, pParent),
	m_SpinText(this)
{
	OnSetup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

/*virtual*/ HyMeter::~HyMeter()
{
}

int32 HyMeter::GetValue()
{
	return m_iTargetValue;
}

void HyMeter::SetValue(int32 iPennies, float fRackDuration)
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
		m_SpinText.m_SpinText_Padded.pos.Y(-m_Text.GetTextBox().y);
	}
	else
	{
		m_iPrevValue = m_iCurValue;
		m_dTotalDistance = abs(m_iTargetValue - m_iCurValue) * static_cast<double>(m_Text.GetTextBox().y);
		m_dTotalDistance -= m_fThresholdDist;
	}

	FormatDigits();
}

void HyMeter::OffsetValue(int32 iPenniesOffsetAmt, float fRackDuration)
{
	SetValue(m_iTargetValue + iPenniesOffsetAmt, fRackDuration);
}

void HyMeter::SetDenomination(int32 iDenom)
{
	m_iDenomination = iDenom;
}

void HyMeter::Slam()
{
	SetValue(m_iTargetValue, 0.0f);
}

bool HyMeter::IsRacking()
{
	return m_iCurValue != m_iTargetValue;
}

bool HyMeter::IsShowAsCash()
{
	return m_bShowAsCash;
}

void HyMeter::ShowAsCash(bool bShow)
{
	m_bShowAsCash = bShow;
	FormatDigits();
}

bool HyMeter::IsSpinningMeter()
{
	return m_bSpinDigits;
}

void HyMeter::SetAsSpinningMeter(bool bSet)
{
	m_bSpinDigits = bSet;
	FormatDigits();
}

bool HyMeter::IsUsingCommas()
{
	return m_bUseCommas;
}

void HyMeter::SetAsUsingCommas(bool bSet)
{
	m_bUseCommas = bSet;
	FormatDigits();
}

/*virtual*/ void HyMeter::SetTextState(uint32 uiStateIndex) /*override*/
{
	HyWidget::SetTextState(uiStateIndex);
	
	m_SpinText.m_SpinText_Shown.SetState(uiStateIndex);
	m_SpinText.m_SpinText_Padded.SetState(uiStateIndex);
}

/*virtual*/ void HyMeter::SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY) /*override*/
{
	HyWidget::SetTextLocation(iWidth, iHeight, iOffsetX, iOffsetY);

	m_Text.SetMonospacedDigits(true);

	m_SpinText.pos.Set(m_Text.pos.X(), m_Text.pos.Y());
	m_SpinText.m_SpinText_Shown.SetAsScaleBox(m_Text.GetTextBox().x, m_Text.GetTextBox().y);
	m_SpinText.m_SpinText_Padded.SetAsScaleBox(m_Text.GetTextBox().x, m_Text.GetTextBox().y);

	FormatDigits();
}

/*virtual*/ void HyMeter::SetTextAlignment(HyTextAlign eAlignment) /*override*/
{
	HyWidget::SetTextAlignment(eAlignment);

	m_SpinText.m_SpinText_Shown.SetTextAlignment(eAlignment);
	m_SpinText.m_SpinText_Padded.SetTextAlignment(eAlignment);

	FormatDigits();
}

/*virtual*/ void HyMeter::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB) /*override*/
{
	HyWidget::SetTextLayerColor(uiLayerIndex, fR, fG, fB);

	m_SpinText.m_SpinText_Shown.SetLayerColor(uiLayerIndex, fR, fG, fB);
	m_SpinText.m_SpinText_Padded.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

std::string HyMeter::ToStringWithCommas(int32 iValue)
{
	std::string sStr = std::to_string(iValue);

	if(sStr.size() < 4)
		return sStr;
	else
	{
		std::string sAppend = "," + sStr.substr(sStr.size() - 3, sStr.size());
		return ToStringWithCommas(iValue / 1000) + sAppend;
	}
}

std::string HyMeter::FormatString(int32 iValue)
{
	std::string returnStr;

	if(m_bShowAsCash)
	{
		returnStr = (iValue < 0) ? "-$" : "$";

		int iNumCents = (abs(iValue) % 100);
		int iNumDollars = (abs(iValue) / 100);

		if(m_bUseCommas)
			returnStr += ToStringWithCommas(iNumDollars);
		else
			returnStr += std::to_string(iNumDollars);

		if(iNumCents >= 10)
			returnStr += ".";
		else
			returnStr += ".0";
		returnStr += std::to_string(iNumCents);
	}
	else
	{
		if(m_bUseCommas)
			returnStr = ToStringWithCommas(iValue / m_iDenomination);
		else
			returnStr = std::to_string(iValue / m_iDenomination);
	}

	return returnStr;
}

void HyMeter::FormatDigits()
{
	float fThreshold = m_Text.GetTextBox().y;

	m_Text.SetText(FormatString(m_iCurValue));

	if(m_bSpinDigits)
	{
		std::string sShownString = m_Text.GetText();
		m_SpinText.m_SpinText_Shown.SetText(sShownString);

		HyAssert(sShownString.empty() == false, "FormatString() returned an empty string");
		uint32 uiCharIndexForScissor = static_cast<uint32>(sShownString.size()) - 1;

		if(m_iCurValue <= m_iTargetValue)
		{
			m_SpinText.m_SpinText_Padded.SetText(FormatString(m_iCurValue + 1));
			m_SpinText.m_SpinText_Padded.pos.Y(m_SpinText.m_SpinText_Shown.pos.Y() - fThreshold);

			for(; uiCharIndexForScissor > 0; --uiCharIndexForScissor)
			{
				char cChar = sShownString[uiCharIndexForScissor];
				if(cChar >= 48 && cChar <= 57)
				{
					if(cChar != '9')
						break;
				}
			}
		}
		else
		{
			m_SpinText.m_SpinText_Padded.SetText(FormatString(m_iCurValue - 1));
			m_SpinText.m_SpinText_Padded.pos.Y(m_SpinText.m_SpinText_Shown.pos.Y() + fThreshold);

			for(; uiCharIndexForScissor > 0; --uiCharIndexForScissor)
			{
				char cChar = sShownString[uiCharIndexForScissor];
				if(cChar >= 48 && cChar <= 57)
				{
					if(cChar != '0')
						break;
				}
			}
		}

		for(uint32 i = 0; i < sShownString.size(); ++i)
		{
			char cChar = sShownString[i];
			if(cChar >= 48 && cChar <= 57)
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

		m_SpinText.SetScissor(0,
			0,
			static_cast<uint32>(m_Text.GetTextBox().x),
			static_cast<uint32>(fThreshold));
	}
	else
	{
		for(uint32 i = 0; i < m_Text.GetText().size(); ++i)
			m_Text.SetGlyphAlpha(i, 1.0f);
	}


	m_SpinText.SetVisible(m_Text.IsVisible() && m_bSpinDigits);
}

/*virtual*/ void HyMeter::OnUpdate() /*override*/
{
	if(m_iCurValue == m_iTargetValue)
		return;

	m_fElapsedTimeRack = HyClamp(m_fElapsedTimeRack + HyEngine::DeltaTime(), 0.0f, m_fRackingDuration);

	if(m_fElapsedTimeRack == m_fRackingDuration)
	{
		Slam();
		return;
	}

	if(m_bSpinDigits == false)
	{
		// Standard non-spinning rack
		int32 iCurPennies = static_cast<uint32>((m_iTargetValue - m_iPrevValue) * (m_fElapsedTimeRack / m_fRackingDuration)) + m_iPrevValue;
		if(iCurPennies != m_iCurValue)
		{
			m_iCurValue = iCurPennies;
			FormatDigits();
		}
	}
	else	// Spinning (analog) digits
	{
		float fThreshold = m_Text.GetTextBox().y;

		double dTravelDist = m_dTotalDistance * (m_fElapsedTimeRack / m_fRackingDuration);
		m_fThresholdDist += static_cast<float>(dTravelDist - m_dPrevDistance);
		m_dPrevDistance = dTravelDist;

		int iTimesPastThreshold = static_cast<int>(m_fThresholdDist / fThreshold);
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

/*virtual*/ void HyMeter::OnSetup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY) /*override*/
{
	HyWidget::OnSetup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
	
	m_SpinText.Setup(sTextPrefix, sTextName);
	FormatDigits();
}
