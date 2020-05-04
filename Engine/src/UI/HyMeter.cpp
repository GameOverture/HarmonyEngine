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
	HyInfoPanel(pParent),
	m_SpinText(this)
{
}

HyMeter::HyMeter(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent) :
	HyInfoPanel(nullptr),
	m_SpinText(this)
{
	Init(szPanelPrefix, szPanelName, pParent);
}

HyMeter::HyMeter(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyInfoPanel(nullptr),
	m_SpinText(this)
{
	Init(szTextPrefix, szTextName, iTextDimensionsX, iTextDimensionsY, pParent);
}

HyMeter::HyMeter(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyInfoPanel(nullptr),
	m_SpinText(this)
{
	Init(szPanelPrefix, szPanelName, szTextPrefix, szTextName, iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY, pParent);
}

/*virtual*/ HyMeter::~HyMeter()
{
}

/*virtual*/ void HyMeter::Init(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent) /*override*/
{
	Init(szPanelPrefix, szPanelName, nullptr, nullptr, 0, 0, 0, 0, pParent);
}

/*virtual*/ void HyMeter::Init(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) /*override*/
{
	Init(nullptr, nullptr, szTextPrefix, szTextName, 0, 0, iTextDimensionsX, iTextDimensionsY, pParent);
}

/*virtual*/ void HyMeter::Init(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) /*override*/
{
	m_SpinText.Init(szTextPrefix, szTextName);

	HyInfoPanel::Init(szPanelPrefix, szPanelName, szTextPrefix, szTextName, iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY, pParent);
	HyAssert(m_pText, "HyMeter was constructed with a null m_pText");

	if(m_pText)
	{
		m_pText->TextSetMonospacedDigits(true);
		SetTextLocation(iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY);

		FormatDigits();
	}
}

int32 HyMeter::GetValue()
{
	return m_iTargetValue;
}

void HyMeter::SetValue(int32 iPennies, float fRackDuration)
{
	if(m_pText == nullptr)
		return;

	m_iTargetValue = iPennies;

	m_fRackingDuration = fRackDuration;
	m_fElapsedTimeRack = 0.0f;
	m_dPrevDistance = 0.0;

	if(m_fRackingDuration <= 0.0f)
	{
		m_iCurValue = m_iPrevValue = m_iTargetValue;
		m_dTotalDistance = 0.0;
		m_fThresholdDist = 0.0f;

		m_SpinText.m_pSpinText_Shown->pos.Y(0.0f);
		m_SpinText.m_pSpinText_Padded->pos.Y(-m_pText->TextGetBox().y);
	}
	else
	{
		m_iPrevValue = m_iCurValue;
		m_dTotalDistance = abs(m_iTargetValue - m_iCurValue) * static_cast<double>(m_pText->TextGetBox().y);
		m_dTotalDistance -= m_fThresholdDist;
	}

	FormatDigits();
}

void HyMeter::OffsetValue(int32 iPenniesOffsetAmt, float fRackDuration)
{
	SetValue(m_iTargetValue + iPenniesOffsetAmt, fRackDuration);
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

void HyMeter::TextSetLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB)
{
	if(m_pText == nullptr)
		return;

	m_pText->TextSetLayerColor(uiLayerIndex, fR, fG, fB);
	m_SpinText.m_pSpinText_Shown->TextSetLayerColor(uiLayerIndex, fR, fG, fB);
	m_SpinText.m_pSpinText_Padded->TextSetLayerColor(uiLayerIndex, fR, fG, fB);
}

void HyMeter::TextSetState(uint32 uiAnimState)
{
	if(m_pText == nullptr)
		return;

	m_pText->TextSetState(uiAnimState);
	m_SpinText.m_pSpinText_Shown->TextSetState(uiAnimState);
	m_SpinText.m_pSpinText_Padded->TextSetState(uiAnimState);
}


/*virtual*/ std::string HyMeter::GetStr() /*override*/
{
	return FormatString(m_iCurValue);
}

/*virtual*/ void HyMeter::SetStr(std::string sText) /*override*/
{
	HyError("HyMeter::SetStr is not implemented");
}

/*virtual*/ void HyMeter::SetTextLocation(int32 iOffsetX, int32 iOffsetY, int32 iWidth, int32 iHeight) /*override*/
{
	if(m_pText == nullptr)
		return;

	HyInfoPanel::SetTextLocation(iOffsetX, iOffsetY, iWidth, iHeight);

	m_SpinText.pos.Set(m_pText->pos.X(), m_pText->pos.Y());
	m_SpinText.m_pSpinText_Shown->SetAsScaleBox(m_pText->TextGetBox().x, m_pText->TextGetBox().y);
	m_SpinText.m_pSpinText_Padded->SetAsScaleBox(m_pText->TextGetBox().x, m_pText->TextGetBox().y);

	FormatDigits();
}

/*virtual*/ void HyMeter::SetTextAlignment(HyTextAlign eAlignment) /*override*/
{
	if(m_pText == nullptr)
		return;

	HyMeter::SetTextAlignment(eAlignment);

	m_SpinText.m_pSpinText_Shown->TextSetAlignment(eAlignment);
	m_SpinText.m_pSpinText_Padded->TextSetAlignment(eAlignment);

	FormatDigits();
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
			returnStr = ToStringWithCommas(iValue);
		else
			returnStr = std::to_string(iValue);
	}

	return returnStr;
}

void HyMeter::FormatDigits()
{
	if(m_pText == nullptr)
		return;

	float fThreshold = m_pText->TextGetBox().y;

	m_pText->TextSet(FormatString(m_iCurValue));

	if(m_bSpinDigits)
	{
		std::string sShownString = m_pText->TextGet();
		m_SpinText.m_pSpinText_Shown->TextSet(sShownString);

		HyAssert(sShownString.empty() == false, "FormatString() returned an empty string");
		uint32 uiCharIndexForScissor = static_cast<uint32>(sShownString.size()) - 1;

		if(m_iCurValue <= m_iTargetValue)
		{
			m_SpinText.m_pSpinText_Padded->TextSet(FormatString(m_iCurValue + 1));
			m_SpinText.m_pSpinText_Padded->pos.Y(m_SpinText.m_pSpinText_Shown->pos.Y() - fThreshold);

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
			m_SpinText.m_pSpinText_Padded->TextSet(FormatString(m_iCurValue - 1));
			m_SpinText.m_pSpinText_Padded->pos.Y(m_SpinText.m_pSpinText_Shown->pos.Y() + fThreshold);

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
					m_pText->TextSetGlyphAlpha(i, 1.0f);
					m_SpinText.m_pSpinText_Shown->TextSetGlyphAlpha(i, 0.0f);
					m_SpinText.m_pSpinText_Padded->TextSetGlyphAlpha(i, 0.0f);
				}
				else
				{
					m_pText->TextSetGlyphAlpha(i, 0.0f);
					m_SpinText.m_pSpinText_Shown->TextSetGlyphAlpha(i, 1.0f);
					m_SpinText.m_pSpinText_Padded->TextSetGlyphAlpha(i, 1.0f);
				}
			}
			else
			{
				m_pText->TextSetGlyphAlpha(i, 1.0f);
				m_SpinText.m_pSpinText_Shown->TextSetGlyphAlpha(i, 0.0f);
				m_SpinText.m_pSpinText_Padded->TextSetGlyphAlpha(i, 0.0f);
			}
		}

		m_SpinText.SetScissor(0,
			0,
			static_cast<uint32>(m_pText->TextGetBox().x),
			static_cast<uint32>(fThreshold));
	}
	else
	{
		for(uint32 i = 0; i < m_pText->TextGet().size(); ++i)
			m_pText->TextSetGlyphAlpha(i, 1.0f);
	}


	m_SpinText.SetVisible(m_pText->IsVisible() && m_bSpinDigits);
}

void HyMeter::OnUpdate()
{
	if(m_iCurValue == m_iTargetValue || m_pText == nullptr)
		return;

	m_fElapsedTimeRack = HyClamp(m_fElapsedTimeRack + Hy_UpdateStep(), 0.0f, m_fRackingDuration);

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
		float fThreshold = m_pText->TextGetBox().y;

		double dTravelDist = m_dTotalDistance * (m_fElapsedTimeRack / m_fRackingDuration);
		m_fThresholdDist += static_cast<float>(dTravelDist - m_dPrevDistance);
		m_dPrevDistance = dTravelDist;

		int iTimesPastThreshold = static_cast<int>(m_fThresholdDist / fThreshold);
		m_fThresholdDist -= (iTimesPastThreshold * fThreshold);

		if(m_iTargetValue >= m_iCurValue)
		{
			m_SpinText.m_pSpinText_Shown->pos.Y(m_fThresholdDist);
			m_SpinText.m_pSpinText_Padded->pos.Y(m_fThresholdDist);

			m_iCurValue += iTimesPastThreshold;
			m_SpinText.m_pSpinText_Padded->pos.Offset(0.0f, -fThreshold);
		}
		else
		{
			m_SpinText.m_pSpinText_Shown->pos.Y(-m_fThresholdDist);
			m_SpinText.m_pSpinText_Padded->pos.Y(-m_fThresholdDist);

			m_iCurValue -= iTimesPastThreshold;
			m_SpinText.m_pSpinText_Padded->pos.Offset(0.0f, fThreshold);
		}

		if(iTimesPastThreshold > 0)
			FormatDigits();
	}
}

