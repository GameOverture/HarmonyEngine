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
	HYFMTDECIMAL_Default = 0,						//  "1", "1.1"
	HYFMTDECIMAL_Always								//  "1.", "1.1"
};

enum HyNumFmtSign // How to denote positive and negative numbers.
{
												// Example outputs in en-US:
	HYFMTSIGN_Default = 0,						// "123", "0", and "-123"
	HYFMTSIGN_Always,							// "+123", "+0", and "-123"
#ifdef HY_USE_ICU
	HYFMTSIGN_Never,							// "123", "0", and "123"
	HYFMTSIGN_Accounting,						// "$123", "$0", and "($123)"
	HYFMTSIGN_AccountingAlways,					// "+$123", "+$0", and "($123)"
	HYFMTSIGN_ExceptZero,						// "+123", "0", and "-123"
	HYFMTSIGN_AccountingExceptZero				// "+123", "0", and "($123)"
#endif
};
enum HyNumFmtGrouping // Use separators to break up larger integer values (aka the comma in $12,345.00)
{
	HYFMTGROUPING_Default = 0,						// Display grouping using the default strategy for all locales.
	HYFMTGROUPING_Off,								// Do not display grouping separators in any locale.
	HYFMTGROUPING_Min2,								// Display grouping using locale defaults, except do not show grouping on values smaller than 10000 (such that there is a minimum of two digits before the first separator)
	HYFMTGROUPING_OnAligned,							// Always display the grouping separator on values of at least 1000.
	HYFMTGROUPING_Thousands							// Use the Western defaults: groups of 3 and enabled for all numbers 1000 or greater. Do not use locale data for determining the grouping strategy.
};
enum HyNumFmtRounding // Whether to round any fractional values to a whole integer
{
	HYFMTROUNDING_None = 0,							// Default. Shows fractional values using the precision specified by HyNumberFormat::SetFractionPrecision() padding values
	HYFMTROUNDING_Ceiling,
	HYFMTROUNDING_Floor,
	HYFMTROUNDING_HalfFloor,
	HYFMTROUNDING_HalfCeiling,
#ifdef HY_USE_ICU
	HYFMTROUNDING_NoneUnlimited,					// Show all fractional digits to the fullest precision
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

	uint32	m_uiTruncateIntegerAt : 7; // Not used
	uint32	m_uiFillIntegerZeros : 7;

	uint32	m_uiMinFraction : 4;
	uint32	m_uiMaxFraction : 4;
	// Total bits used: 32

public:
	HyNumberFormat();
	HyNumberFormat(const HyNumberFormat &copyRef);

	HyNumberFormat &operator=(const HyNumberFormat &rhs);

	// Whether to show the decimal separator after integers when there are one or more digits to display
	void SetDecimalSeparator(HyNumDecimalSeparator eDecimalSeparator);

	// How to denote positive and negative numbers.
	void SetSign(HyNumFmtSign eSign);

	// Use separators to break up larger integer values (aka the comma in $12,345.00)
	void SetGrouping(HyNumFmtGrouping eGrouping);

	// Whether to round any fractional values to a whole integer
	void SetRounding(HyNumFmtRounding eRounding);

	// Values will clamp to [0-15]
	void SetFractionPrecision(int32 iMinFractionPlaces = 0, int32 iMaxFractionPlaces = 6);

	// Values will clamp to [0-127]
	void SetIntegerPaddingWidth(int32 iZeroPaddingPlaces = 1);
};

#endif /* HyNumberFormat_h__ */
