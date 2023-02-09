/**************************************************************************
*	HyLocale.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Localization/HyLocale.h"
#include "UI/Localization/HyNumpunct.h"
#include "Diagnostics/Console/IHyConsole.h"

#ifdef HY_DEBUG
	#define CHECK_ICU_STATUS(status) if(status > 0) HyLogError("ICU Status Error: " << status)
#else
	#define CHECK_ICU_STATUS(status)
#endif

/*static*/ std::string HyLocale::sm_sIso639Code("en");
/*static*/ std::string HyLocale::sm_sIso3166Code("US");
/*static*/ std::string HyLocale::sm_sIso4217Code("USD");
/*static*/ std::string HyLocale::sm_sMinorCurrencySymbol("\xC2\xA2"); // Cent symbol in UTF8
/*static*/ HyLocale::FallbackNumpunctData<char> HyLocale::sm_FallbackNumpunctData('.', ',', "\003", "$", "+", "-", 2, true);
/*static*/ HyLocale::FallbackNumpunctData<wchar_t> HyLocale::sm_FallbackNumpunctWideData(L'.', L',', "\003", L"$", L"+", L"-", 2, true);
/*static*/ std::map<std::string, int32>	HyLocale::sm_Iso4217Map;

/*static*/ void HyLocale::Imbue(std::string sIso639Code, std::string sIso3166Code, std::string sIso4217Code)
{
	// Break up any combo (lang/country) locale codes
	if(sIso639Code.find('-') != std::string::npos)
	{
		sIso3166Code = sIso639Code.substr(sIso639Code.find('-') + 1);
		sIso639Code = sIso639Code.substr(0, sIso639Code.find('-'));
	}
	else if(sIso639Code.find('_') != std::string::npos)
	{
		sIso3166Code = sIso639Code.substr(sIso639Code.find('_') + 1);
		sIso639Code = sIso639Code.substr(0, sIso639Code.find('_'));
	}

	HyAssert(sIso639Code.empty() || sIso639Code.size() == 2, "ISO 639 code must be empty string or '2' characters");
	HyAssert(sIso3166Code.empty() || sIso3166Code.size() == 2, "ISO 3166 code must be empty string or '2' characters");
	HyAssert(sIso4217Code.empty() || sIso4217Code.size() == 3, "ISO 4217 code must be empty string or '3' characters");

	sm_sIso639Code = sIso639Code;
	sm_sIso3166Code = sIso3166Code;
	sm_sIso4217Code = sIso4217Code;

	if(sm_sIso4217Code.empty())
	{
		std::locale loc(AssembleStdLocale(), new HyMoneypunct<char, true, int64>(HyNumberFormat(), 0, ""));
		sm_sIso4217Code = std::use_facet<HyMoneypunct<char, true, int64>>(loc).curr_symbol();
		sm_sIso4217Code = sm_sIso4217Code.substr(0, 3);
	}

	std::transform(sm_sIso639Code.begin(), sm_sIso639Code.end(), sm_sIso639Code.begin(), ::tolower);
	std::transform(sm_sIso3166Code.begin(), sm_sIso3166Code.end(), sm_sIso3166Code.begin(), ::toupper);
	std::transform(sm_sIso4217Code.begin(), sm_sIso4217Code.end(), sm_sIso4217Code.begin(), ::toupper);

	AssembleFallbackNumpunct();
}

/*static*/ void HyLocale::Imbue(std::string sLangCountryCode, std::string sIso4217Code)
{
	HyLocale::Imbue(sLangCountryCode, "", sIso4217Code);
}

/*static*/ void HyLocale::SetMinorCurrencySymbol(std::string sMinorCurrencySymbolUtf8)
{
	sm_sMinorCurrencySymbol = sMinorCurrencySymbolUtf8;
}

/*static*/ std::string HyLocale::Number_Format(int64 iValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus = U_ZERO_ERROR;
	auto localizedNumFormatter = AssembleFormatter(format, false);
	auto formattedNum = localizedNumFormatter.formatInt(iValue, eStatus);
	CHECK_ICU_STATUS(eStatus);

	auto sUnicodeStr = formattedNum.toString(eStatus);
	CHECK_ICU_STATUS(eStatus);

	sUnicodeStr.toUTF8String<std::string>(sText);
	if(sText.empty())
		HyLogError("HyLocale::Number_Format(int64) returned empty string! - " << "Value: " << iValue << ", Status: " << eStatus);
#else
	std::locale loc = AssembleStdLocale();
	std::stringstream str;
	str.imbue(std::locale(loc, new HyNumpunct<char, int64>(format, iValue, loc.name())));
	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		str.setf(std::ios_base::showpoint);
	if(format.m_uiSign == HYFMTSIGN_Always ||
		(format.m_uiSign == HYFMTSIGN_AlwaysExceptZero && iValue != 0) ||
		format.m_uiSign == HYFMTSIGN_AccountingAlways ||
		(format.m_uiSign == HYFMTSIGN_AccountingExceptZero && iValue != 0))
	{
		str.setf(std::ios_base::showpos);
	}

	str << iValue;
	sText = str.str();

	if(format.GetRounding() == HYFMTROUNDING_HideZeros)
		HideFractionIfZero(sText, std::use_facet<HyNumpunct<char, int64>>(str.getloc()).decimal_point());
#endif

	return sText;
}

/*static*/ std::string HyLocale::Number_Format(double dValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus = U_ZERO_ERROR;
	auto localizedNumFormatter = AssembleFormatter(format, false);
	auto formattedNum = localizedNumFormatter.formatDouble(dValue, eStatus);
	CHECK_ICU_STATUS(eStatus);

	auto sUnicodeStr = formattedNum.toString(eStatus);
	CHECK_ICU_STATUS(eStatus);

	sUnicodeStr.toUTF8String<std::string>(sText);
	if(sText.empty())
		HyLogError("HyLocale::Number_Format(double) returned empty string! - " << "Value: " << dValue << ", Status: " << eStatus);
#else
	std::locale loc = AssembleStdLocale();
	std::stringstream str;
	str.imbue(std::locale(loc, new HyNumpunct<char, double>(format, dValue, loc.name())));
	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		str.setf(std::ios_base::showpoint);
	if(format.m_uiSign == HYFMTSIGN_Always ||
		(format.m_uiSign == HYFMTSIGN_AlwaysExceptZero && dValue != 0) ||
		format.m_uiSign == HYFMTSIGN_AccountingAlways ||
		(format.m_uiSign == HYFMTSIGN_AccountingExceptZero && dValue != 0))
	{
		str.setf(std::ios_base::showpos);
	}
	str.precision(format.m_uiMaxFraction);

	if(format.IsUsingScientificNotation())
		str << std::scientific << std::fixed << dValue;
	else
		str << std::fixed << dValue;
	
	sText = str.str();

	if(format.GetRounding() == HYFMTROUNDING_HideZeros)
		HideFractionIfZero(sText, std::use_facet<HyNumpunct<char, double>>(str.getloc()).decimal_point());
#endif

	return sText;
}

// 'iValue' is the total number of all fractional units (pennies/pence/etc) in the total value (eg. 150 = $1.50)
/*static*/ std::string HyLocale::Money_Format(int64 iValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

	if(format.IsUsingMinorCurrencySymbol() &&
	   sm_sMinorCurrencySymbol.empty() == false &&
	   iValue < 100)
	{
		sText = std::to_string(iValue);
		sText += sm_sMinorCurrencySymbol;
		return sText;
	}

#if HY_USE_ICU
	int32 iNumFractionDigits = Money_GetNumFractionalDigits();
	double dDenominator = 1.0;
	for(int32 i = 0; i < iNumFractionDigits; ++i)
		dDenominator *= 10.0;

	UErrorCode eStatus = U_ZERO_ERROR;
	auto localizedNumFormatter = AssembleFormatter(format, true);
	localizedNumFormatter = localizedNumFormatter.unit(icu::CurrencyUnit(sm_sIso4217Code.c_str(), eStatus));
	CHECK_ICU_STATUS(eStatus);

	auto formattedNum = localizedNumFormatter.formatDouble(static_cast<double>(iValue) / dDenominator, eStatus);
	CHECK_ICU_STATUS(eStatus);
	
	auto sUnicodeStr = formattedNum.toString(eStatus);
	CHECK_ICU_STATUS(eStatus);

	sUnicodeStr.toUTF8String<std::string>(sText);
	if(sText.empty())
		HyLogError("HyLocale::Money_Format(int64) returned empty string! - " << "Value: " << iValue << ", Status: " << eStatus);
#else
	std::locale loc = AssembleStdLocale();
	std::stringstream str;
	str.imbue(std::locale(loc, new HyMoneypunct<char, false, int64>(format, iValue, loc.name())));
	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		str.setf(std::ios_base::showpoint);
	if(format.m_uiSign == HYFMTSIGN_Always ||
		(format.m_uiSign == HYFMTSIGN_AlwaysExceptZero && iValue != 0) ||
		format.m_uiSign == HYFMTSIGN_AccountingAlways ||
		(format.m_uiSign == HYFMTSIGN_AccountingExceptZero && iValue != 0))
	{
		str.setf(std::ios_base::showpos);
	}

	if(format.IsUsingScientificNotation())
		str << std::scientific << std::showbase << std::put_money(iValue);
	else
		str << std::showbase << std::put_money(iValue);
	sText = str.str();
	
	if(format.GetRounding() == HYFMTROUNDING_HideZeros)
		HideFractionIfZero(sText, std::use_facet<HyMoneypunct<char, false, int64>>(str.getloc()).decimal_point());
#endif

	return sText;
}

/*static*/ std::string HyLocale::Money_Format(double dValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

	if(format.IsUsingMinorCurrencySymbol() &&
	   sm_sMinorCurrencySymbol.empty() == false &&
	   dValue < 1.0)
	{
		int32 iCents = static_cast<int32>(dValue * 100.0); // Truncate any fractional values
		sText = std::to_string(iCents);
		sText += sm_sMinorCurrencySymbol;
		return sText;
	}

#if HY_USE_ICU
	UErrorCode eStatus = U_ZERO_ERROR;
	auto localizedNumFormatter = AssembleFormatter(format, true);
	localizedNumFormatter = localizedNumFormatter.unit(icu::CurrencyUnit(sm_sIso4217Code.c_str(), eStatus));
	CHECK_ICU_STATUS(eStatus);

	auto formattedNum = localizedNumFormatter.formatDouble(dValue, eStatus);
	CHECK_ICU_STATUS(eStatus);

	auto sUnicodeStr = formattedNum.toString(eStatus);
	CHECK_ICU_STATUS(eStatus);

	sUnicodeStr.toUTF8String<std::string>(sText);
	if(sText.empty())
		HyLogError("HyLocale::Money_Format(double) returned empty string! - " << "Value: " << dValue << ", Status: " << eStatus);
#else

	std::locale loc = AssembleStdLocale();

	std::stringstream str;
	str.imbue(std::locale(loc, new HyMoneypunct<char, false, double>(format, dValue, loc.name())));
	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		str.setf(std::ios_base::showpoint);
	if(format.m_uiSign == HYFMTSIGN_Always ||
		(format.m_uiSign == HYFMTSIGN_AlwaysExceptZero && dValue != 0.0) ||
		format.m_uiSign == HYFMTSIGN_AccountingAlways ||
		(format.m_uiSign == HYFMTSIGN_AccountingExceptZero && dValue != 0.0))
	{
		str.setf(std::ios_base::showpos);
	}

	// TODO: is this working?
	int32 iNumFractionDigits = Money_GetNumFractionalDigits();
	double dDenominator = 1.0;
	for(int32 i = 0; i < iNumFractionDigits; ++i)
		dDenominator *= 10.0;
	// This is backup for now
	int32 iCents = static_cast<int32>(dValue * 100.0); // Truncate any fractional values

	if(format.IsUsingScientificNotation())
		str << std::scientific << std::showbase << std::put_money(iCents);
	else
		str << std::showbase << std::put_money(iCents);

	sText = str.str();

	if(format.GetRounding() == HYFMTROUNDING_HideZeros)
		HideFractionIfZero(sText, std::use_facet<HyMoneypunct<char, false, double>>(str.getloc()).decimal_point());
#endif

	return sText;
}

/*static*/ int32 HyLocale::Money_GetNumFractionalDigits()
{
#if HY_USE_ICU
	UChar szCurrency[4];
	szCurrency[0] = sm_sIso4217Code[0];
	szCurrency[1] = sm_sIso4217Code[1];
	szCurrency[2] = sm_sIso4217Code[2];
	szCurrency[3] = '\0';
	
	UErrorCode eStatus = U_ZERO_ERROR;
	int32 iNumFracDigits = ucurr_getDefaultFractionDigits(szCurrency, &eStatus);
	CHECK_ICU_STATUS(eStatus);

	return iNumFracDigits;
#else
	return std::use_facet<std::moneypunct<char>>(AssembleStdLocale()).frac_digits();
#endif
}

/*static*/ std::string HyLocale::Percent_Format(double dValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus = U_ZERO_ERROR;
	auto localizedNumFormatter = AssembleFormatter(format, false);
	localizedNumFormatter = localizedNumFormatter.unit(icu::NoUnit::percent());

	auto formattedNum = localizedNumFormatter.formatDouble(dValue, eStatus);
	CHECK_ICU_STATUS(eStatus);

	auto sUnicodeStr = formattedNum.toString(eStatus);
	CHECK_ICU_STATUS(eStatus);

	sUnicodeStr.toUTF8String<std::string>(sText);
	if(sText.empty())
		HyLogError("HyLocale::Percent_Format(double) returned empty string! - " << "Value: " << dValue << ", Status: " << eStatus);
#else
	// Use best guess for formatting
	std::stringstream str;
	str << Number_Format(dValue, format) << "%";
	sText = str.str();
#endif

	return sText;
}

#ifdef HY_USE_ICU
/*static*/ icu::number::LocalizedNumberFormatter HyLocale::AssembleFormatter(HyNumberFormat format, bool bIsMoney)
{
	auto localizedNumFormatter = icu::number::NumberFormatter::withLocale(icu::Locale(sm_sIso639Code.c_str(), sm_sIso3166Code.c_str()));

	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		localizedNumFormatter = localizedNumFormatter.decimal(UNumberDecimalSeparatorDisplay::UNUM_DECIMAL_SEPARATOR_ALWAYS);
	switch(format.m_uiSign)
	{
	case HYFMTSIGN_Default:
	default:
		break;
	case HYFMTSIGN_Always:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_ALWAYS);
		break;
	case HYFMTSIGN_AlwaysExceptZero:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_EXCEPT_ZERO);
		break;
	case HYFMTSIGN_Never:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_NEVER);
		break;
	case HYFMTSIGN_Accounting:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_ACCOUNTING);
		break;
	case HYFMTSIGN_AccountingAlways:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_ACCOUNTING_ALWAYS);
		break;
	case HYFMTSIGN_AccountingExceptZero:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_ACCOUNTING_EXCEPT_ZERO);
		break;
	}

	switch(format.m_uiGrouping)
	{
	case HYFMTGROUPING_Default:
	default:
		break;
	case HYFMTGROUPING_Off:
		localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_OFF);
		break;
	case HYFMTGROUPING_Min2:
		localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_MIN2);
		break;
	case HYFMTGROUPING_Thousands:
		localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_THOUSANDS);
		break;
	}

	localizedNumFormatter = localizedNumFormatter.integerWidth(icu::number::IntegerWidth::zeroFillTo(format.m_uiFillIntegerZeros));

	switch(format.m_uiRounding)
	{
	case HYFMTROUNDING_None:
		//localizedNumFormatter = localizedNumFormatter.precision(number::Precision::minMaxFraction(format.m_uiMinFraction, format.m_uiMaxFraction));
	default:
		break;

	case HYFMTROUNDING_HideZeros:
		if(bIsMoney)
			localizedNumFormatter = localizedNumFormatter.precision(icu::number::Precision::currency(UCURR_USAGE_STANDARD).trailingZeroDisplay(UNUM_TRAILING_ZERO_HIDE_IF_WHOLE));
		else
			localizedNumFormatter = localizedNumFormatter.precision(icu::number::Precision::minMaxFraction(format.m_uiMinFraction, format.m_uiMaxFraction).trailingZeroDisplay(UNUM_TRAILING_ZERO_HIDE_IF_WHOLE));
		break;
	case HYFMTROUNDING_Ceiling:
		localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_CEILING);
		break;
	case HYFMTROUNDING_Floor:
		localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_FLOOR);
		break;
	case HYFMTROUNDING_NoneUnlimited:
		localizedNumFormatter = localizedNumFormatter.precision(icu::number::Precision::unlimited());
		break;
	}

	if(format.IsUsingScientificNotation())
		localizedNumFormatter = localizedNumFormatter.notation(icu::number::Notation::scientific());

	return localizedNumFormatter;
}
#endif

/*static*/ std::locale HyLocale::AssembleStdLocale()
{
	std::string sStdName;
	if(sm_sIso639Code.empty() == false)
	{
		sStdName = sm_sIso639Code;
		if(sm_sIso3166Code.empty() == false)
			sStdName += "_" + sm_sIso3166Code;

		sStdName += ".UTF-8";
	}

	std::locale assembledLocale;
	try
	{
		assembledLocale = std::locale(sStdName);
	}
	catch(const std::exception &)
	{
		//HyLogWarning("HyLocale::AssembleStdLocale() failed with \"" << sStdName << "\". Using fallback locale.");
		assembledLocale = std::locale();
	}
	
	return assembledLocale;
}

/*static*/ void HyLocale::HideFractionIfZero(std::string &sTextRef, char cDecimalChar)
{
	size_t uiDecIndex = sTextRef.find_last_of(cDecimalChar);
	if(uiDecIndex != std::string::npos)
	{
		bool bAllZeros = true;
		for(size_t i = uiDecIndex + 1; i < sTextRef.size(); ++i)
		{
			if(sTextRef[i] != '0')
			{
				bAllZeros = false;
				break;
			}
		}
		if(bAllZeros)
			sTextRef = sTextRef.substr(0, uiDecIndex);
	}
}

/*static*/ void HyLocale::AssembleFallbackNumpunct()
{
	if(sm_Iso4217Map.empty())
	{
		sm_Iso4217Map["PTS"] = 000;		// Points
		sm_Iso4217Map["USD"] = 840;
		sm_Iso4217Map["EUR"] = 978;
		sm_Iso4217Map["JPY"] = 392;
		sm_Iso4217Map["GBP"] = 826;
		sm_Iso4217Map["AUD"] = 036;
		sm_Iso4217Map["CHF"] = 756;
		sm_Iso4217Map["CAD"] = 124;
		sm_Iso4217Map["CNY"] = 156;
		sm_Iso4217Map["HKD"] = 344;
		sm_Iso4217Map["SGD"] = 702;
		sm_Iso4217Map["NZD"] = 554;
		sm_Iso4217Map["KRW"] = 410;
		sm_Iso4217Map["NOK"] = 578;
		sm_Iso4217Map["SEK"] = 752;
		sm_Iso4217Map["DKK"] = 208;
		sm_Iso4217Map["ZAR"] = 710;
		sm_Iso4217Map["MXN"] = 484;
		sm_Iso4217Map["BRL"] = 986;
		sm_Iso4217Map["INR"] = 356;
		sm_Iso4217Map["RUB"] = 643;
		sm_Iso4217Map["TRY"] = 949;
		sm_Iso4217Map["MYR"] = 458;
		sm_Iso4217Map["PHP"] = 608;
		sm_Iso4217Map["THB"] = 764;
		sm_Iso4217Map["IDR"] = 360;
		sm_Iso4217Map["SAR"] = 682;
		sm_Iso4217Map["AED"] = 784;
		sm_Iso4217Map["ILS"] = 376;
		sm_Iso4217Map["UAH"] = 980;
		sm_Iso4217Map["BYN"] = 933;
		sm_Iso4217Map["CRC"] = 188;
		sm_Iso4217Map["GYD"] = 328;
		sm_Iso4217Map["NGN"] = 566;
		sm_Iso4217Map["PEN"] = 604;
		sm_Iso4217Map["KES"] = 404;
	}

	char cDecimalPoint, cThousandsSep;
	if(sm_sIso639Code == "en")
	{
		cDecimalPoint = '.';
		cThousandsSep = ',';
	}
	else
	{
		cDecimalPoint = ',';
		cThousandsSep = '.';
	}

	int32 iIso4217Code = -1;
	if(sm_Iso4217Map.find(sm_sIso4217Code) != sm_Iso4217Map.end())
		iIso4217Code = sm_Iso4217Map[sm_sIso4217Code];

	switch(iIso4217Code)
	{
	case 000: // PTS - Points or game credits (not actual currency)
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "PT", "+", "-", 2, true);
		break;

	case 840: // USD - United States dollar (US$)
	case 036: // AUD - Australian dollar (A$)
	case 124: // CAD - Canadian dollar (C$)
	case 702: // SGD - Singapore dollar (S$)
	case 554: // NZD - New Zealand dollar (NZ$)
	case 484: // MXN - Mexican peso ($)
	case 986: // BRL - Brazilian real (R$)
	case 328: // GYD - Guyanese dollar (G$)
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "$", "+", "-", 2, true);
		break;

	case 978: // EUR - Euro
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xAC", "+", "-", 2, true); // \xE2\x82\xAC is the "Euro" symbol
		break;

	case 392: // JPY - Japanese yen
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xC2\xA5", "+", "-", 0, true); // \xC2\xA5 is the "Yen" or "Yuan" symbol
		break;

	case 826: // GBP - Pound Sterling
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xC2\xA3", "+", "-", 2, true); // \xC2\xA3 is the "Pound" symbol
		break;

	case 756: // CHF - Swiss franc
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "CHF"/*sm_sIso639Code == "de" ? "Fr" : "fr"*/, "+", "-", 2, true);
		break;

	case 156: // CNY - Renminbi, China
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xC2\xA5", "+", "-", 2, true); // \xC2\xA5 is the "Yen" or "Yuan" symbol
		break;

	case 344: // HKD - Hong Kong dollar
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "HK$", "+", "-", 2, true);
		break;

	case 410: // KRW - South Korean won
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xA9", "+", "-", 2, true); // \xE2\x82\xA9 is the South Korean "Won" symbol
		break;

	case 578: // NOK - Norwegian krone
	case 752: // SEK - Swedish krona
	case 208: // DKK - Danish krone
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "kr", "+", "-", 2, false);
		break;

	case 710: // ZAR - South African rand
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "R", "+", "-", 2, true);
		break;

	case 356: // INR - Indian rupee
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xB9", "+", "-", 2, true); // \xE2\x82\xB9 is the Indian "rupee" symbol
		break;

	case 643: // RUB - Russian ruble
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xBD", "+", "-", 2, false); // \xE2\x82\xBD is the "Ruble" symbol
		break;

	case 949: // TRY - Turkish lira
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xBA", "+", "-", 2, true); // \xE2\x82\xBA is the Turkish "Lira" symbol
		break;

	case 458: // MYR - Malaysian ringgit
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "RM", "+", "-", 2, true);
		break;

	case 608: // PHP - Philippine peso
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xB1", "+", "-", 2, true); // \xE2\x82\xB1 is the Philippine "peso" symbol
		break;

	case 764: // THB - Thai baht
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE0\xB8\xBF", "+", "-", 2, true); // \xE0\xB8\xBF is the Thai "baht" symbol
		break;

	case 360: // IDR - Indonesian rupiah
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "Rp", "+", "-", 2, true);
		break;

	case 682: // SAR - Saudi riyal
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "SAR", "+", "-", 2, false);
		break;

	case 784: // AED - United Arab Emirates dirham
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "Dhs", "+", "-", 2, true);
		break;

	case 376: // ILS - Israeli new shekel
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xAA", "+", "-", 2, true); // \xE2\x82\xAA is the Israeli "new shekel" symbol
		break;

	case 980: // UAH - Ukrainian hryvnia
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xB4", "+", "-", 2, true); // \xE2\x82\xB4 is the Ukrainian "hryvnia" symbol
		break; 

	case 933: // BYN - Belarusian ruble
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "Rbls", "+", "-", 2, true);
		break;

	case 188: // CRC - Costa Rican colon
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xA1", "+", "-", 2, true); // \xE2\x82\xA1 is the Costa Rican "colon" symbol
		break;

	case 566: // NGN - Nigerian naira
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xA6", "+", "-", 2, true); // \xE2\x82\xA6 is the Nigerian "naira" symbol
		break;

	case 604: // PEN - Peruvian sol
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "S/", "+", "-", 2, true);
		break;

	case 404: // KES - Kenyan shilling
		sm_FallbackNumpunctData.Set('/', cThousandsSep, "\003", "KSh", "+", "-", 2, true);
		break;

	default:
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", sm_sIso4217Code.empty() ? "$" : sm_sIso4217Code.c_str(), "+", "-", 2, true);
		break;
	}

	// TODO: Set wide fallback as above, but using wide characters
	sm_FallbackNumpunctWideData.Set(L'.', L',', sm_FallbackNumpunctData.m_sGrouping, L"$", L"+", L"-", sm_FallbackNumpunctData.m_iFracDigits, sm_FallbackNumpunctData.m_bSymbolOnLeft);
}

template<typename CharT>
/*static*/ HyLocale::FallbackNumpunctData<CharT> HyLocale::GetFallbackNumpunctData()
{
	if constexpr(std::is_same<CharT, char>::value)
		return sm_FallbackNumpunctData;
	else
		return sm_FallbackNumpunctWideData;
}

