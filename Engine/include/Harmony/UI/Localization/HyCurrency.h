/**************************************************************************
*	HyCurrency.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyCurrency_h__
#define HyCurrency_h__

#include "Afx/HyStdAfx.h"
#include <locale>

class HyCurrency
{
	std::locale		m_Locale;

	// ISO 4217
	char			m_Iso4217Code[3] = { 'U', 'S', 'D' };
	uint32			m_uiDenomination = 100;


public:
	// Cosmedic
	bool			m_bAlwaysShowDecimal = false;	// Don't show fractional portion of the value if it's zero (aka $1.00 or 1,00€)
	bool			m_bShowSeparators = true;		// Use separator symbols to break up larger integer values (aka the comma in $12,345.00)
	bool			m_bUseDecimalSymbol = true;	// Use the fractional symbol if the value is less than 1 integer unit (aka 50¢)

public:
	HyCurrency();
	HyCurrency(std::string sIso4217Code);
	virtual ~HyCurrency();

	std::string FormatToCurrency(int64 iValue) const;
	int64 ParseCurrencyString(std::string sValue) const;
};

#endif /* HyCurrency_h__ */
