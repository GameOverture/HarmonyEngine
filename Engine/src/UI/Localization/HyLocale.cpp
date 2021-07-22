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
/*static*/ std::string HyLocale::sm_sIso4217Code("usd");

HyLocale::HyLocale()
{
}

/*virtual*/ HyLocale::~HyLocale()
{
}

/*static*/ void HyLocale::Imbue(std::string sIso639Code, std::string sIso4217Code)
{
	sm_sIso639Code = sIso639Code;
	sm_sIso4217Code = sIso4217Code;
}

/*static*/ std::string HyLocale::Number_Format(int64 iValue, HyNumberFormat format)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus;
	auto sUnicodeStr = AssembleFormatter(format).formatInt(iValue, eStatus).toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else

	std::stringstream str;

	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		str.setf(std::ios_base::showpoint);

	if(format.m_uiSign == HYFMTSIGN_Always)
		str.setf(std::ios_base::showpos);
	
	str << std::put_money(iValue);
	//std::stringstream *pFormattedStr = AllocateFormatter(format);
	//pFormattedStr
	//localizedStr.imbue(std::locale(""));

	//localizedStr << iValue;
	sText = str.str();
#endif

	return sText;
}

/*static*/ std::string HyLocale::Number_Format(double dValue, HyNumberFormat format)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus;
	auto sUnicodeStr = AssembleFormatter(format).formatDouble(dValue, eStatus).toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else
	std::stringstream localizedStr;
	localizedStr.imbue(std::locale(""));
	localizedStr << dValue;
	sText = localizedStr.str();
#endif

	return sText;
}

// 'iValue' is the total number of all fractional units (pennies/pence/etc) in the total value (eg. 150 = $1.50)
/*static*/ std::string HyLocale::Money_Format(int64 iValue, HyNumberFormat format)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus;

	int32 iNumFractionDigits = Money_GetNumFractionalDigits();
	double dDenominator = 1.0;
	for(int32 i = 0; i < iNumFractionDigits; ++i)
		dDenominator *= 10.0;

	auto localizedNumFormatter = AssembleFormatter(format);
	auto sUnicodeStr = localizedNumFormatter.unit(icu::CurrencyUnit(sm_sIso4217Code.c_str(), eStatus)).formatDouble(static_cast<double>(iValue) / dDenominator, eStatus).toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else
	std::stringstream str;
	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		str.setf(std::ios_base::showpoint);
	if(format.m_uiSign == HYFMTSIGN_Always)
		str.setf(std::ios_base::showpos);
	
	str << std::put_money(iValue);
	sText = str.str();
#endif

	return sText;
}

/*static*/ std::string HyLocale::Money_Format(double dValue, HyNumberFormat format)
{
	std::string sText;

#if HY_USE_ICU
	UErrorCode eStatus;
	auto localizedNumFormatter = AssembleFormatter(format);
	auto sUnicodeStr = localizedNumFormatter.unit(icu::CurrencyUnit(sm_sIso4217Code.c_str(), eStatus)).formatDouble(dValue, eStatus).toString(eStatus);
	sUnicodeStr.toUTF8String<std::string>(sText);
#else

	//std::stringstream localizedStr;
	//localizedStr.imbue(std::locale(""));
	//localizedStr << dValue;
	//sText = localizedStr.str();
	//// The stream contains $100.00 (assuming a en_US locale config)
	//ss << std::showbase << std::put_money(basepay);

	//if(iValue < 100 && m_bUseDecimalSymbol)
	//{
	//	sText = std::to_string(iValue);
	//	sText += "\xC2\xA2";	// This is the cent symbol in UTF8
	//}
	//else
	{
		int64 iValue = static_cast<int64>(dValue * 100.0);
		sText = "$";
		int64 iNumDollars = (abs(iValue) / 100);
		sText += std::to_string(iNumDollars);

		int64 iNumCents = (abs(iValue) % 100);
		if(/*m_bAlwaysShowDecimal ||*/ iNumCents != 0)
		{
			if(iNumCents >= 10)
				sText += ".";
			else
				sText += ".0";
			sText += std::to_string(iNumCents);
		}
	}
#endif

	return sText;
}

/*static*/ int32 HyLocale::Money_GetNumFractionalDigits()
{
#if HY_USE_ICU
	UErrorCode eStatus;
	icu::UnicodeString uc;
	uc.fromUTF8(sm_sIso4217Code);
	
	return ucurr_getDefaultFractionDigits(uc.getBuffer(), &eStatus);
#else
	return 2;
#endif
}

//double HyLocale::ConvertCentsToDbl(int64 iTotalFractionUnits)
//{
//	int32 iNumFractionDigits = Money_GetNumFractionalDigits();
//	double dDenominator = 1.0;
//	for(int32 i = 0; i < iNumFractionDigits; ++i)
//		dDenominator *= 10.0;
//
//	return static_cast<double>(iTotalFractionUnits) / dDenominator;
//}

#ifdef HY_USE_ICU
/*static*/ icu::number::LocalizedNumberFormatter HyLocale::AssembleFormatter(HyNumberFormat format)
{
	auto localizedNumFormatter = icu::number::NumberFormatter::withLocale(icu::Locale(sm_sIso639Code.c_str()));

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
	case HYFMTSIGN_Never:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_NEVER);
		break;
	case HYFMTSIGN_Accounting:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_ACCOUNTING);
		break;
	case HYFMTSIGN_AccountingAlways:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_ACCOUNTING_ALWAYS);
		break;
	case HYFMTSIGN_ExceptZero:
		localizedNumFormatter = localizedNumFormatter.sign(UNumberSignDisplay::UNUM_SIGN_EXCEPT_ZERO);
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
	case HYFMTGROUPING_OnAligned:
		localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_ON_ALIGNED);
		break;
	case HYFMTGROUPING_Thousands:
		localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_THOUSANDS);
		break;
	}

	switch(format.m_uiRounding)
	{
	case HYFMTROUNDING_None:
	default:
		break;
	case HYFMTROUNDING_NoneUnlimited:
		
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
	}

	return localizedNumFormatter;
}
#else
/*static*/ std::stringstream *HyLocale::AllocateFormatter(HyNumberFormat format)
{

	

	//try {
	//	auto loc = std::locale("en_US");
	//	try {
	//		pNewStr->imbue(loc);
	//	}
	//	catch(std::runtime_error &ex) {
	//		puts("imbue");
	//		puts(ex.what());
	//	}
	//}
	//catch(std::runtime_error &ex) {
	//	puts("construct locale en_US");
	//	puts(ex.what());
	//}


	std::stringstream *pNewStr = HY_NEW std::stringstream();

	if(format.m_uiDecimalSeparator == HYFMTDECIMAL_Always)
		pNewStr->setf(std::ios_base::showpoint);

	if(format.m_uiSign == HYFMTSIGN_Always)
		pNewStr->setf(std::ios_base::showpos);
	
	return pNewStr;

	//switch(format.m_uiGrouping)
	//{
	//case HYFMTGROUPING_Default:
	//default:
	//	break;
	//case HYFMTGROUPING_Off:
	//	localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_OFF);
	//	break;
	//case HYFMTGROUPING_Min2:
	//	localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_MIN2);
	//	break;
	//case HYFMTGROUPING_OnAligned:
	//	localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_ON_ALIGNED);
	//	break;
	//case HYFMTGROUPING_Thousands:
	//	localizedNumFormatter = localizedNumFormatter.grouping(UNumberGroupingStrategy::UNUM_GROUPING_THOUSANDS);
	//	break;
	//}

	//switch(format.m_uiRounding)
	//{
	//case HYFMTROUNDING_None:
	//default:
	//	break;
	//case HYFMTROUNDING_NoneUnlimited:
	//	
	//	break;
	//case HYFMTROUNDING_Ceiling:
	//	localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_CEILING);
	//	break;
	//case HYFMTROUNDING_Floor:
	//	localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_FLOOR);
	//	break;
	//case HYFMTROUNDING_HalfFloor:
	//	localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_HALF_FLOOR);
	//	break;
	//case HYFMTROUNDING_HalfCeiling:
	//	localizedNumFormatter = localizedNumFormatter.roundingMode(UNumberFormatRoundingMode::UNUM_ROUND_HALF_CEILING);
	//	break;
	//}

	//return localizedNumFormatter;
}
#endif

///*static*/ double HyLocale::Number_Parse(std::string sValue)
//{
//	double dReturnValue = 0.0;
//
//#if HY_USE_ICU
//	UErrorCode eStatus = U_ZERO_ERROR;
//
//	// Ensure that the decimal separatator is '.' for standard parsing with 'std::stod()' below
//	icu::DecimalFormatSymbols decimalFormatSyms(icu::Locale(sm_sIso639Code.c_str()), eStatus);
//	decimalFormatSyms.setSymbol(icu::DecimalFormatSymbols::kDecimalSeparatorSymbol, '.');
//	
//	auto localizedNumFormatter = AssembleFormatter(HyNumberFormat())
//		.symbols(decimalFormatSyms)
//		.unit(icu::CurrencyUnit(sm_sIso4217Code.c_str(), eStatus));
//	auto formattedNum = localizedNumFormatter.formatDecimal(icu::StringPiece(sValue.c_str()), eStatus);
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
