/**************************************************************************
*	HyNumberFormat.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyNumberFormat_h__
#define HyNumberFormat_h__

#include "Afx/HyStdAfx.h"

class HyLocale;

enum HyNumDecimalSeparator // Whether to show the decimal separator after integers when there are one or more digits to display
{
	HYFMTDECIMAL_Default = 0,				//  "1", "1.1"
	HYFMTDECIMAL_Always						//  "1.", "1.1"
};
enum HyNumFmtSign // How to denote positive and negative numbers.
{
											// Example outputs in en-US:
	HYFMTSIGN_Default = 0,					// "123", "0", and "-123"
	HYFMTSIGN_Always,						// "+123", "+0", and "-123"
	HYFMTSIGN_AlwaysExceptZero,				// "+123", "0", and "-123"
	HYFMTSIGN_Never,						// "123", "0", and "123"

	HYFMTSIGN_Accounting,					// "$123", "$0", and "($123)"
	HYFMTSIGN_AccountingAlways,				// "+$123", "+$0", and "($123)"
	HYFMTSIGN_AccountingExceptZero			// "+123", "0", and "($123)"
};
enum HyNumFmtGrouping // Use separators to break up larger integer values (aka the comma in $12,345.00)
{
	HYFMTGROUPING_Default = 0,				// Display grouping using the default strategy for all locales.
	HYFMTGROUPING_Off,						// Do not display grouping separators in any locale.
	HYFMTGROUPING_Min2,						// Display grouping using locale defaults, except do not show grouping on values smaller than 10000 (such that there is a minimum of two digits before the first separator)
	HYFMTGROUPING_Thousands					// Use the Western defaults: groups of 3 and enabled for all numbers 1000 or greater. Do not use locale data for determining the grouping strategy.
};
enum HyNumFmtRounding // Decides how to round any fractional values to a whole integer
{
	HYFMTROUNDING_None = 0,					// Default. Shows fractional values using the precision specified by HyNumberFormat::SetFractionPrecision() padding values
	HYFMTROUNDING_HideZeros,				// Hides trailing zeros in the fractional display of numbers (unless it's money). If showing money, will hide entire fraction if value is whole number
	HYFMTROUNDING_Ceiling,
	HYFMTROUNDING_Floor,
#ifdef HY_USE_ICU
	HYFMTROUNDING_NoneUnlimited,			// Show all fractional digits to the fullest precision
#endif
};

class HyNumberFormat
{
	friend class HyLocale;

	// Store all formatting information in a single 32bit integer
	uint32	m_uiDecimalSeparator : 1;
	uint32	m_uiSign : 3;
	uint32	m_uiGrouping : 3;
	uint32	m_uiRounding : 3;

	uint32	m_uiUseMinorCurrencySymbol : 1;
	uint32	m_uiUseScientificNotation : 1;
	uint32	m_uiUseCurrencySymbol : 1;

	uint32	m_uiFillIntegerZeros : 7;

	uint32	m_uiMinFraction : 4;
	uint32	m_uiMaxFraction : 4;
	// Total bits used: 28

public:
	HyNumberFormat();
	HyNumberFormat(const HyNumberFormat &copyRef);

	HyNumberFormat &operator=(const HyNumberFormat &rhs);

	// Whether to show the decimal separator after integers when there are one or more digits to display
	HyNumDecimalSeparator GetDecimalSeparator() const;
	// Whether to show the decimal separator after integers when there are one or more digits to display
	HyNumberFormat SetDecimalSeparator(HyNumDecimalSeparator eDecimalSeparator);

	// How to denote positive and negative numbers.
	HyNumFmtSign GetSign() const;
	// How to denote positive and negative numbers.
	HyNumberFormat SetSign(HyNumFmtSign eSign);

	// Use separators to break up larger integer values (aka the comma in $12,345.00)
	HyNumFmtGrouping GetGrouping() const;
	// Use separators to break up larger integer values (aka the comma in $12,345.00)
	HyNumberFormat SetGrouping(HyNumFmtGrouping eGrouping);

	// Whether to round any fractional values to a whole integer
	HyNumFmtRounding GetRounding() const;
	// Whether to round any fractional values to a whole integer
	HyNumberFormat SetRounding(HyNumFmtRounding eRounding);

	// Only used when 'HYFMTROUNDING_None'. Values will clamp to [0-15]
	HyNumberFormat SetFractionPrecision(int32 iMinFractionPlaces = 0, int32 iMaxFractionPlaces = 6);

	// Whether to optionally use the minor fractional symbol when formatting currencies if the value is less than 1 integer unit (aka 50�)
	bool IsUsingMinorCurrencySymbol() const;
	// Whether to optionally use the minor fractional symbol when formatting currencies if the value is less than 1 integer unit (aka 50�)
	HyNumberFormat SetUsingMinorCurrencySymbol(bool bUseMinorCurrencySymbol);

	// Whether to optionally format floating-point values using scientific notation
	bool IsUsingScientificNotation() const;
	// Whether to optionally format floating-point values using scientific notation
	HyNumberFormat SetUsingScientificNotation(bool bUseScientificNotation);

	// Whether to use the currency symbol (e.g. $), otherwise it will try to use the ISO currency code (e.g. USD)
	bool IsUsingCurrencySymbol() const;
	// Whether to use the currency symbol (e.g. $), otherwise it will try to use the ISO currency code (e.g. USD)
	HyNumberFormat SetUsingCurrencySymbol(bool bUseCurrencySymbol);

	// Minimum number of integer digits to display. Will pad with zeros.
	// Valid values for 'iZeroPaddingPlaces' [0-127]
	HyNumberFormat SetIntegerPaddingWidth(int32 iZeroPaddingPlaces = 1);
};

#endif /* HyNumberFormat_h__ */
