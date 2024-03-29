/**************************************************************************
*	HyNumberFormat.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Localization/HyNumberFormat.h"
#include "Utilities/HyMath.h"

HyNumberFormat::HyNumberFormat()
{
	m_uiDecimalSeparator = HYFMTDECIMAL_Default;
	m_uiSign = HYFMTSIGN_Default;
	m_uiGrouping = HYFMTGROUPING_Default;
	m_uiRounding = HYFMTROUNDING_None;

	m_uiUseMinorCurrencySymbol = 0;
	m_uiUseScientificNotation = 0;
	m_uiUseCurrencySymbol = 1;
	
	m_uiFillIntegerZeros = 1;

	m_uiMinFraction = 0;
	m_uiMaxFraction = 6;
}

HyNumberFormat::HyNumberFormat(const HyNumberFormat &copyRef)
{
	m_uiDecimalSeparator = copyRef.m_uiDecimalSeparator;
	m_uiSign = copyRef.m_uiSign;
	m_uiGrouping = copyRef.m_uiGrouping;
	m_uiRounding = copyRef.m_uiRounding;

	m_uiUseMinorCurrencySymbol = copyRef.m_uiUseMinorCurrencySymbol;
	m_uiUseScientificNotation = copyRef.m_uiUseScientificNotation;
	m_uiUseCurrencySymbol = copyRef.m_uiUseCurrencySymbol;

	m_uiFillIntegerZeros = copyRef.m_uiFillIntegerZeros;

	m_uiMinFraction = copyRef.m_uiMinFraction;
	m_uiMaxFraction = copyRef.m_uiMaxFraction;
}

HyNumberFormat &HyNumberFormat::operator=(const HyNumberFormat &rhs)
{
	m_uiDecimalSeparator = rhs.m_uiDecimalSeparator;
	m_uiSign = rhs.m_uiSign;
	m_uiGrouping = rhs.m_uiGrouping;
	m_uiRounding = rhs.m_uiRounding;

	m_uiUseMinorCurrencySymbol = rhs.m_uiUseMinorCurrencySymbol;
	m_uiUseScientificNotation = rhs.m_uiUseScientificNotation;
	m_uiUseCurrencySymbol = rhs.m_uiUseCurrencySymbol;

	m_uiFillIntegerZeros = rhs.m_uiFillIntegerZeros;

	m_uiMinFraction = rhs.m_uiMinFraction;
	m_uiMaxFraction = rhs.m_uiMaxFraction;

	return *this;
}

HyNumDecimalSeparator HyNumberFormat::GetDecimalSeparator() const
{
	return static_cast<HyNumDecimalSeparator>(m_uiDecimalSeparator);
}

HyNumberFormat HyNumberFormat::SetDecimalSeparator(HyNumDecimalSeparator eDecimalSeparator)
{
	m_uiDecimalSeparator = eDecimalSeparator;
	return *this;
}

HyNumFmtSign HyNumberFormat::GetSign() const
{
	return static_cast<HyNumFmtSign>(m_uiSign);
}

HyNumberFormat HyNumberFormat::SetSign(HyNumFmtSign eSign)
{
	m_uiSign = eSign;
	return *this;
}

HyNumFmtGrouping HyNumberFormat::GetGrouping() const
{
	return static_cast<HyNumFmtGrouping>(m_uiGrouping);
}

HyNumberFormat HyNumberFormat::SetGrouping(HyNumFmtGrouping eGrouping)
{
	m_uiGrouping = eGrouping;
	return *this;
}

HyNumFmtRounding HyNumberFormat::GetRounding() const
{
	return static_cast<HyNumFmtRounding>(m_uiRounding);
}

HyNumberFormat HyNumberFormat::SetRounding(HyNumFmtRounding eRounding)
{
	m_uiRounding = eRounding;
	return *this;
}

// Only used when 'HYFMTROUNDING_None'. Values will clamp to [0-15]
HyNumberFormat HyNumberFormat::SetFractionPrecision(int32 iMinFractionPlaces /*= 0*/, int32 iMaxFractionPlaces /*= 6*/)
{
	m_uiMinFraction = HyMath::Clamp(iMinFractionPlaces, 0, 15);
	m_uiMaxFraction = HyMath::Clamp(iMaxFractionPlaces, 0, 15);
	return *this;
}

bool HyNumberFormat::IsUsingMinorCurrencySymbol() const
{
	return (m_uiUseMinorCurrencySymbol == 1);
}

HyNumberFormat HyNumberFormat::SetUsingMinorCurrencySymbol(bool bUseMinorCurrencySymbol)
{
	m_uiUseMinorCurrencySymbol = bUseMinorCurrencySymbol ? 1 : 0;
	return *this;
}

bool HyNumberFormat::IsUsingScientificNotation() const
{
	return (m_uiUseScientificNotation == 1);
}

HyNumberFormat HyNumberFormat::SetUsingScientificNotation(bool bUseScientificNotation)
{
	m_uiUseScientificNotation = bUseScientificNotation ? 1 : 0;
	return *this;
}

bool HyNumberFormat::IsUsingCurrencySymbol() const
{
	return (m_uiUseCurrencySymbol == 1);
}

HyNumberFormat HyNumberFormat::SetUsingCurrencySymbol(bool bUseCurrencySymbol)
{
	m_uiUseCurrencySymbol = bUseCurrencySymbol ? 1 : 0;
	return *this;
}

// Values will clamp to [0-127]
HyNumberFormat HyNumberFormat::SetIntegerPaddingWidth(int32 iZeroPaddingPlaces /*= 1*/)
{
	m_uiFillIntegerZeros = HyMath::Clamp(iZeroPaddingPlaces, 0, 127);
	return *this;
}
