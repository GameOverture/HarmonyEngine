/**************************************************************************
*	HyLocale.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLocale_h__
#define HyLocale_h__

#include "Afx/HyStdAfx.h"
#include "UI/Localization/HyNumberFormat.h"

#if HY_USE_ICU
	#define U_CHARSET_IS_UTF8 1
	#include <unicode/numberformatter.h>
	
	#ifdef HY_PLATFORM_BROWSER
		using namespace icu_62;
	#else
		using namespace icu;
	#endif
#else
	#include <locale>
	#include <iomanip>
#endif

class HyLocale
{
	static std::string		sm_sIso639Code;
	static std::string		sm_sIso4217Code;
	
	bool					m_bUseDecimalSymbol = true;		// Use the fractional symbol if the value is less than 1 integer unit (aka 50¢)

	//struct FmtGrouping : std::numpunct<char>
	//{
	//	HyNumberFormat		m_format;

	//	FmtGrouping(HyNumberFormat format) :
	//		m_format(format)
	//	{ }

	//	char do_thousands_sep()   const { return ','; }
	//	std::string do_grouping() const { return "\3"; } // groups of 1 digit
	//};

public:
	HyLocale();
	virtual ~HyLocale();

	static void Imbue(std::string sIso639Code, std::string sIso4217Code);

	static std::string Number_Format(int64 iValue, HyNumberFormat format = HyNumberFormat());
	static std::string Number_Format(double dValue, HyNumberFormat format = HyNumberFormat());

	// 'iValue' is the total number of all fractional units (pennies/pence/etc) in the total value (eg. 150 = $1.50)
	static std::string Money_Format(int64 iValue, HyNumberFormat format = HyNumberFormat());
	static std::string Money_Format(double dValue, HyNumberFormat format = HyNumberFormat());
	static int32 Money_GetNumFractionalDigits();

	static std::string Percent_Format(double dValue, HyNumberFormat format = HyNumberFormat());

private:
#ifdef HY_USE_ICU
	static icu::number::LocalizedNumberFormatter AssembleFormatter(HyNumberFormat format);
#else
	static void AssembleFormatter(std::stringstream &ssRef, HyNumberFormat format);
#endif
};

#endif /* HyLocale_h__ */
