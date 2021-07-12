/**************************************************************************
*	HyCurrency.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Localization/HyCurrency.h"

HyCurrency::HyCurrency()
{
}

/*virtual*/ HyCurrency::~HyCurrency()
{
}

std::string HyCurrency::FormatToCurrency(int64 iValue) const
{
	std::string sText;
	if(iValue < 100 && m_bUseDecimalSymbol)
	{
		sText = std::to_string(iValue);
		sText += "\xC2\xA2";	// This is the cent symbol in UTF8
	}
	else
	{
		sText = "$";
		int64 iNumDollars = (abs(iValue) / 100);
		sText += std::to_string(iNumDollars);

		int64 iNumCents = (abs(iValue) % 100);
		if(m_bAlwaysShowDecimal || iNumCents != 0)
		{
			if(iNumCents >= 10)
				sText += ".";
			else
				sText += ".0";
			sText += std::to_string(iNumCents);
		}
	}

	return sText;
}

int64 HyCurrency::ParseCurrencyString(std::string sValue) const
{
	return 0;
}
