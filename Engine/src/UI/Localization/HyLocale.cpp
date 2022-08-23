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
/*static*/ HyLocale::FallbackNumpunctData<char> HyLocale::sm_FallbackNumpunctData('.', ',', "\003", "$", "+", "-", 2);
/*static*/ HyLocale::FallbackNumpunctData<wchar_t> HyLocale::sm_FallbackNumpunctWideData(L'.', L',', "\003", L"$", L"+", L"-", 2);
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
	auto localizedNumFormatter = AssembleFormatter(format);
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
#endif

	return sText;
}

/*static*/ std::string HyLocale::Number_Format(double dValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus = U_ZERO_ERROR;
	auto localizedNumFormatter = AssembleFormatter(format);
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
	auto localizedNumFormatter = AssembleFormatter(format);
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
	auto localizedNumFormatter = AssembleFormatter(format);
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

	int32 iNumFractionDigits = Money_GetNumFractionalDigits();
	double dDenominator = 1.0;
	for(int32 i = 0; i < iNumFractionDigits; ++i)
		dDenominator *= 10.0;

	if(format.IsUsingScientificNotation())
		str << std::scientific << std::showbase << std::put_money(dValue * dDenominator);
	else
		str << std::showbase << std::put_money(dValue * dDenominator);

	sText = str.str();
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
	auto localizedNumFormatter = AssembleFormatter(format);
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
/*static*/ icu::number::LocalizedNumberFormatter HyLocale::AssembleFormatter(HyNumberFormat format)
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

/*static*/ void HyLocale::AssembleFallbackNumpunct()
{
	if(sm_Iso4217Map.empty())
	{
		sm_Iso4217Map["AED"] = 784;
		sm_Iso4217Map["GBP"] = 826;
		sm_Iso4217Map["EUR"] = 978;
	}

	if(sm_Iso4217Map.find(sm_sIso4217Code) == sm_Iso4217Map.end())
	{
		sm_FallbackNumpunctData.Set('.', ',', "\003", "$", "+", "-", 2);
		sm_FallbackNumpunctWideData.Set(L'.', L',', "\003", L"$", L"+", L"-", 2);
		return;
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

	switch(sm_Iso4217Map[sm_sIso4217Code])
	{
	case 784: // AED - United Arab Emirates dirham
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "$", "+", "-", 2);
		break;

	case 826: // GBP - Pound Sterling
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xC2\xA3", "+", "-", 2); // \xC2\xA3 is the "Pound" symbol
		break;

	case 978: // EUR - Euro
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "\xE2\x82\xAC", "+", "-", 2); // \xE2\x82\xAC is the "Euro" symbol
		break;

	default:
		sm_FallbackNumpunctData.Set(cDecimalPoint, cThousandsSep, "\003", "$", "+", "-", 2);
		sm_FallbackNumpunctWideData.Set(L'.', L',', "\003", L"$", L"+", L"-", 2);
		break;
	}
}

template<typename CharT>
/*static*/ HyLocale::FallbackNumpunctData<CharT> HyLocale::GetFallbackNumpunctData()
{
	if constexpr(std::is_same<CharT, char>::value)
		return sm_FallbackNumpunctData;
	else
		return sm_FallbackNumpunctWideData;
}

