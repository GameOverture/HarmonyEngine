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
#include "UI/Widgets/HyMeter.h"
#include "HyEngine.h"

HyMeter::HyMeter(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_SpinText(this)
{
}

HyMeter::HyMeter(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, pParent),
	m_SpinText(this)
{
	OnSetup();
}

HyMeter::HyMeter(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(iWidth, iHeight, iStroke, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_SpinText(this)
{
	OnSetup();
}

HyMeter::HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, pParent),
	m_SpinText(this)
{
	OnSetup();
}

HyMeter::HyMeter(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_SpinText(this)
{
	OnSetup();
}

/*virtual*/ HyMeter::~HyMeter()
{
}

int64 HyMeter::GetValue()
{
	return m_iTargetValue;
}

void HyMeter::SetValue(int64 iPennies, float fRackDuration)
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

void HyMeter::OffsetValue(int64 iPenniesOffsetAmt, float fRackDuration)
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

HyNumberFormat HyMeter::GetNumFormat() const
{
	return m_NumberFormat;
}

void HyMeter::SetNumFormat(HyNumberFormat format)
{
	m_NumberFormat = format;
	FormatDigits();
}

/*virtual*/ void HyMeter::SetTextState(uint32 uiStateIndex) /*override*/
{
	HyLabel::SetTextState(uiStateIndex);
	
	m_SpinText.m_SpinText_Shown.SetState(uiStateIndex);
	m_SpinText.m_SpinText_Padded.SetState(uiStateIndex);
}

/*virtual*/ void HyMeter::ResetTextOnPanel() /*override*/
{
	HyLabel::ResetTextOnPanel();

	m_Text.SetMonospacedDigits(true);

	m_SpinText.pos.Set(m_Text.pos.X(), m_Text.pos.Y());
	m_SpinText.m_SpinText_Shown.SetAsScaleBox(m_Text.GetTextBox().x, m_Text.GetTextBox().y);
	m_SpinText.m_SpinText_Padded.SetAsScaleBox(m_Text.GetTextBox().x, m_Text.GetTextBox().y);

	FormatDigits();
}

/*virtual*/ void HyMeter::SetTextAlignment(HyAlignment eAlignment) /*override*/
{
	HyLabel::SetTextAlignment(eAlignment);

	m_SpinText.m_SpinText_Shown.SetTextAlignment(eAlignment);
	m_SpinText.m_SpinText_Padded.SetTextAlignment(eAlignment);

	FormatDigits();
}

/*virtual*/ void HyMeter::SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB) /*override*/
{
	HyLabel::SetTextLayerColor(uiLayerIndex, fR, fG, fB);

	m_SpinText.m_SpinText_Shown.SetLayerColor(uiLayerIndex, fR, fG, fB);
	m_SpinText.m_SpinText_Padded.SetLayerColor(uiLayerIndex, fR, fG, fB);
}

void HyMeter::FormatDigits()
{
	float fThreshold = m_Text.GetTextBox().y;

	if(m_bShowAsCash)
		m_Text.SetText(HyLocale::Money_Format(m_iCurValue, m_NumberFormat));
	else
		m_Text.SetText(HyLocale::Number_Format(m_iCurValue, m_NumberFormat));

	if(m_bSpinDigits)
	{
		HyAssert(m_Text.GetNumCharacters() != 0, "HyMeter - EMPTY STRING! " << "IsCash: " << (m_bShowAsCash ? "true" : "false") << ", Value: " << m_iCurValue);

		m_SpinText.m_SpinText_Shown.SetText(m_Text.GetUtf8String());
		uint32 uiCharIndexForScissor = m_Text.GetNumCharacters() - 1;

		if(m_iCurValue <= m_iTargetValue)
		{
			if(m_bShowAsCash)
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
			if(m_bShowAsCash)
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

		m_SpinText.SetScissor(0,
			0,
			static_cast<uint32>(m_Text.GetTextBox().x),
			static_cast<uint32>(fThreshold));
	}
	else
	{
		for(uint32 i = 0; i < m_Text.GetNumCharacters(); ++i)
			m_Text.SetGlyphAlpha(i, 1.0f);
	}


	m_SpinText.SetVisible(m_Text.IsVisible() && m_bSpinDigits);
}

/*virtual*/ void HyMeter::OnUpdate() /*override*/
{
	if (m_iCurValue == m_iTargetValue)
	{
		PlayEffects(false, false);
		return;
	}

	m_fElapsedTimeRack = HyClamp(m_fElapsedTimeRack + HyEngine::DeltaTime(), 0.0f, m_fRackingDuration);

	if(m_fElapsedTimeRack == m_fRackingDuration)
	{
		Slam();
		return;
	}

	if(m_bSpinDigits == false)
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
		float fThreshold = m_Text.GetTextBox().y;

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

/*virtual*/ void HyMeter::OnSetup() /*override*/
{
	m_SpinText.Setup(m_Text.GetPrefix(), m_Text.GetName());
	FormatDigits();
}

void HyMeter::PlayEffects(bool bInnerOn, bool bOuterOn)
{
	if (m_InnerEffect.IsLoadDataValid() == true)
	{
	//	if (m_InnerEffect.IsVisible() == true && m_InnerEffect.alpha.IsAnimating() == true && bInnerOn == false)
	//		m_InnerEffect.alpha.StopAnim();

		if (((bInnerOn == false && m_InnerEffect.IsVisible() != false) || (bInnerOn == true && m_InnerEffect.IsVisible() != true)) &&
			m_InnerEffect.alpha.IsAnimating() == false)
		{
			if (bInnerOn == true)
			{
				m_InnerEffect.alpha.Set(0.0f);
				m_InnerEffect.SetVisible(true);
			}

			m_InnerEffect.SetTag(bInnerOn);
			m_InnerEffect.alpha.Tween(((bInnerOn == true) ? 1.0f : 0.0f), 1.0f, HyTween::Linear, [](IHyNode* pSelf)
				{
					HySprite2d* pEffect = reinterpret_cast<HySprite2d*>(pSelf);

					if (pSelf->GetTag() == 0)
						pSelf->SetVisible(false);

					pSelf->SetTag(0);
				});
		}
	}

	if (m_OuterEffect.IsLoadDataValid() == true)
	{
		

	//	if (m_OuterEffect.IsVisible() == true && m_OuterEffect.alpha.IsAnimating() == true && bInnerOn == false)
	//		m_OuterEffect.alpha.StopAnim();

		if (((bInnerOn == false && m_OuterEffect.IsVisible() != false) || (bInnerOn == true && m_OuterEffect.IsVisible() != true)) &&
			m_OuterEffect.alpha.IsAnimating() == false)
		{
			if (bOuterOn == true)
			{
				m_OuterEffect.SetVisible(true);
				m_OuterEffect.alpha.Set(0.0f);
			}

			m_OuterEffect.SetTag(bOuterOn);
			m_OuterEffect.alpha.Tween(((bOuterOn == true) ? 1.0f : 0.0f), 1.0f, HyTween::Linear, [](IHyNode* pSelf)
				{
					HySprite2d* pEffect = reinterpret_cast<HySprite2d*>(pSelf);

					if (pSelf->GetTag() == 0)
						pSelf->SetVisible(false);

					pSelf->SetTag(0);
				});
		}
	}
}
