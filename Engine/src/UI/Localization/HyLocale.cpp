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
#include "Diagnostics/Console/IHyConsole.h"

/*static*/ std::string HyLocale::sm_sIso639Code("en");
/*static*/ std::string HyLocale::sm_sIso3166Code("US");
/*static*/ std::string HyLocale::sm_sIso4217Code("USD");
/*static*/ std::string HyLocale::sm_sMinorCurrencyUnit("");

/*static*/ void HyLocale::Imbue(std::string sIso639Code, std::string sIso3166Code, std::string sIso4217Code /*= ""*/)
{
	HyAssert(sIso639Code.empty() || sIso639Code.size() == 2, "ISO 639 code must be empty string or '2' characters");
	HyAssert(sIso3166Code.empty() || sIso3166Code.size() == 2, "ISO 3166 code must be empty string or '2' characters");
	HyAssert(sIso4217Code.empty() || sIso4217Code.size() == 3, "ISO 4217 code must be empty string or '3' characters");

	sm_sIso639Code = sIso639Code;
	sm_sIso3166Code = sIso3166Code;
	sm_sIso4217Code = sIso4217Code;

	if(sm_sIso4217Code.empty())
	{
		sm_sIso4217Code = std::use_facet<std::moneypunct<char, true>>(std::locale(AssembleStdLocaleString())).curr_symbol();
		sm_sIso4217Code = sm_sIso4217Code.substr(0, 3);
	}

	std::transform(sm_sIso639Code.begin(), sm_sIso639Code.end(), sm_sIso639Code.begin(), ::tolower);
	std::transform(sm_sIso3166Code.begin(), sm_sIso3166Code.end(), sm_sIso3166Code.begin(), ::toupper);
	std::transform(sm_sIso4217Code.begin(), sm_sIso4217Code.end(), sm_sIso4217Code.begin(), ::toupper);
}

/*static*/ std::string HyLocale::Number_Format(int64 iValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus = U_ZERO_ERROR;
	auto localizedNumFormatter = AssembleFormatter(format);
	auto formattedNum = localizedNumFormatter.formatInt(iValue, eStatus);

	auto sUnicodeStr = formattedNum.toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else
	std::stringstream str;
	str.imbue(std::locale(str.getloc(), new HyLocale_numberpunct<char, int64>(format, iValue, AssembleLocaleString())));
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

	auto sUnicodeStr = formattedNum.toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else
	std::stringstream str;
	str.imbue(std::locale(str.getloc(), new HyLocale_numberpunct<char, double>(format, dValue, AssembleLocaleString())));
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
	   sm_sMinorCurrencyUnit.empty() == false &&
	   iValue < 100)
	{
		sText = std::to_string(iValue);
		sText += sm_sMinorCurrencyUnit;
		return sText;
	}

#if HY_USE_ICU
	int32 iNumFractionDigits = Money_GetNumFractionalDigits();
	double dDenominator = 1.0;
	for(int32 i = 0; i < iNumFractionDigits; ++i)
		dDenominator *= 10.0;

	UErrorCode eStatus;
	auto localizedNumFormatter = AssembleFormatter(format);
	auto formattedNum = localizedNumFormatter
		.unit(CurrencyUnit(sm_sIso4217Code.c_str(), eStatus))
		.formatDouble(static_cast<double>(iValue) / dDenominator, eStatus);
	
	auto sUnicodeStr = formattedNum.toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else
	std::stringstream str;
	str.imbue(std::locale(str.getloc(), new HyLocale_moneypunct<char, false, int64>(format, iValue, AssembleLocaleString())));
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
	   sm_sMinorCurrencyUnit.empty() == false &&
	   dValue < 1.0)
	{
		int32 iCents = dValue * 100.0;
		sText = std::to_string(iCents);
		sText += sm_sMinorCurrencyUnit;
		return sText;
	}

#if HY_USE_ICU
	UErrorCode eStatus;
	auto localizedNumFormatter = AssembleFormatter(format);
	auto sUnicodeStr = localizedNumFormatter.unit(CurrencyUnit(sm_sIso4217Code.c_str(), eStatus)).formatDouble(dValue, eStatus).toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else
	std::stringstream str;
	str.imbue(std::locale(str.getloc(), new HyLocale_moneypunct<char, false, double>(format, dValue, AssembleLocaleString())));
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
	UErrorCode eStatus;
	UnicodeString uc;
	uc.fromUTF8(sm_sIso4217Code);
	
	return ucurr_getDefaultFractionDigits(uc.getBuffer(), &eStatus);
#else
	return std::use_facet<std::moneypunct<char>>(std::locale(AssembleLocaleString())).frac_digits();
#endif
}

/*static*/ std::string HyLocale::Percent_Format(double dValue, HyNumberFormat format /*= HyNumberFormat()*/)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus;
	auto localizedNumFormatter = AssembleFormatter(format);
	auto sUnicodeStr = localizedNumFormatter.unit(NoUnit::percent()).formatDouble(dValue, eStatus).toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else
	// Use best guess for formatting
	std::stringstream str;
	str << Number_Format(dValue, format) << "%";
	sText = str.str();
#endif

	return sText;
}

#ifdef HY_USE_ICU
/*static*/ number::LocalizedNumberFormatter HyLocale::AssembleFormatter(HyNumberFormat format)
{
	auto localizedNumFormatter = number::NumberFormatter::withLocale(Locale(sm_sIso639Code.c_str(), sm_sIso3166Code.c_str()));

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

	localizedNumFormatter = localizedNumFormatter.integerWidth(number::IntegerWidth::zeroFillTo(format.m_uiFillIntegerZeros));

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
	case HYFMTROUNDING_HalfFloor:
		localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_HALF_FLOOR);
		break;
	case HYFMTROUNDING_HalfCeiling:
		localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_HALF_CEILING);
		break;
	case HYFMTROUNDING_NoneUnlimited:
		localizedNumFormatter = localizedNumFormatter.precision(number::Precision::unlimited());
		break;
	}

	if(format.IsUsingScientificNotation())
		localizedNumFormatter = localizedNumFormatter.notation(number::Notation::scientific());

	return localizedNumFormatter;
}
#endif

/*static*/ std::string HyLocale::AssembleStdLocaleString()
{
	std::string sStdName;
	if(sm_sIso639Code.empty() == false)
	{
		sStdName = sm_sIso639Code;
		if(sm_sIso3166Code.empty() == false)
			sStdName += "_" + sm_sIso3166Code;
	}
	sStdName += ".UTF-8";
	return sStdName;
}

//if(iValue < 100 && m_bUseDecimalSymbol)
//{
//	sText = std::to_string(iValue);
//	sText += "\xC2\xA2";	// This is the cent symbol in UTF8
//}
//else
//{
//	int64 iValue = static_cast<int64>(dValue * 100.0);
//	sText = "$";
//	int64 iNumDollars = (abs(iValue) / 100);
//	sText += std::to_string(iNumDollars);

//	int64 iNumCents = (abs(iValue) % 100);
//	if(/*m_bAlwaysShowDecimal ||*/ iNumCents != 0)
//	{
//		if(iNumCents >= 10)
//			sText += ".";
//		else
//			sText += ".0";
//		sText += std::to_string(iNumCents);
//	}
//}

///*static*/ double HyLocale::Number_Parse(std::string sValue)
//{
//	double dReturnValue = 0.0;
//
//#if HY_USE_ICU
//	UErrorCode eStatus = U_ZERO_ERROR;
//
//	// Ensure that the decimal separatator is '.' for standard parsing with 'std::stod()' below
//	DecimalFormatSymbols decimalFormatSyms(Locale(sm_sIso639Code.c_str()), eStatus);
//	decimalFormatSyms.setSymbol(DecimalFormatSymbols::kDecimalSeparatorSymbol, '.');
//	
//	auto localizedNumFormatter = AssembleFormatter(HyNumberFormat())
//		.symbols(decimalFormatSyms)
//		.unit(CurrencyUnit(sm_sIso4217Code.c_str(), eStatus));
//	auto formattedNum = localizedNumFormatter.formatDecimal(StringPiece(sValue.c_str()), eStatus);
//	auto sUnicodeStr = formattedNum.toString(eStatus);
//
//	sValue.clear();
//	sUnicodeStr.toUTF8String<std::string>(sValue);
//#endif
//
//	// Strip any formatting the string may have
//	sValue.erase(std::remove_if(sValue.begin(), sValue.end(), [](char c)->bool { return !std::isdigit(c) && c != '.'; }), sValue.end());
//
//	try {
//		dReturnValue = std::stod(sValue);
//	}
//	catch(const std::invalid_argument &) {
//		HyLogError("HyLocale::Money_Parse - std::stod() Argument is invalid");
//	}
//	catch(const std::out_of_range &) {
//		HyLogError("HyLocale::Money_Parse - std::stod() Argument is out of range for a double");
//	}
//
//
//	return dReturnValue;
//}



//std::string HyMeter::FormatString(int64 iValue)
//{
//	std::string returnStr;
//
//	if(m_bShowAsCash)
//	{
//		returnStr = (iValue < 0) ? "-$" : "$";
//
//		int64 iNumCents = (abs(iValue) % 100);
//		int64 iNumDollars = (abs(iValue) / 100);
//
//		if(m_bUseCommas)
//			returnStr += ToStringWithCommas(iNumDollars);
//		else
//			returnStr += std::to_string(iNumDollars);
//
//		if(iNumCents >= 10)
//			returnStr += ".";
//		else
//			returnStr += ".0";
//		returnStr += std::to_string(iNumCents);
//	}
//	else
//	{
//		if(m_bUseCommas)
//			returnStr = ToStringWithCommas(iValue / m_iDenomination);
//		else
//			returnStr = std::to_string(iValue / m_iDenomination);
//	}
//
//	return returnStr;
//}

//std::string HyMeter::ToStringWithCommas(int64 iValue)
//{
//	std::string sStr = std::to_string(iValue);
//
//	if(sStr.size() < 4)
//		return sStr;
//	else
//	{
//		std::string sAppend = "," + sStr.substr(sStr.size() - 3, sStr.size());
//		return ToStringWithCommas(iValue / 1000) + sAppend;
//	}
//}
