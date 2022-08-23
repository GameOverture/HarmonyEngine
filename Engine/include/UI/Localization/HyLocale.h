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
#include "UI/Localization/HyTranslateTable.h"

#if HY_USE_ICU
	#define U_CHARSET_IS_UTF8 1
	#include <unicode/numberformatter.h>
#endif

class HyLocale
{
	template <typename CharT, typename ValueT>
	friend class HyNumpunct;

	template <typename CharT, bool bIntSyms, typename ValueT>
	friend class HyMoneypunct;

	static std::string					sm_sIso639Code;			// Language Code
	static std::string					sm_sIso3166Code;		// Country Code
	static std::string					sm_sIso4217Code;		// Currency Code

	static std::string					sm_sMinorCurrencySymbol;// Currency "cent" symbol

	template<typename CharT>
	struct FallbackNumpunctData
	{
		CharT							m_cDecimalPoint;
		CharT							m_cThousandsSep;
		std::string						m_sGrouping;			// The groups are stored as binary values: three-digit group is '\3', and 51-digit group is '3'
		std::basic_string<CharT>		m_sCurrSymbol;			// The object of type string_type holding the currency symbol.
		std::basic_string<CharT>		m_sPositiveSign;		// Only the first character of the string returned is the character that appears in the pos_format() position indicated by the value sign. The rest of the characters appear after the rest of the monetary string.
		std::basic_string<CharT>		m_sNegativeSign;		// Only the first character of the string returned is the character that appears in the neg_format() position indicated by the value sign. The rest of the characters appear after the rest of the monetary string.
		int32							m_iFracDigits;			// The number of digits to be displayed after the decimal point (when writing money). In common U.S. locales, this is the value 2

		FallbackNumpunctData(CharT cDecimalPoint, CharT cThousandsSep, std::string sGrouping, std::basic_string<CharT> sCurrSymbol, std::basic_string<CharT> sPositiveSign, std::basic_string<CharT> sNegativeSign, int32 iFracDigits)
		{
			Set(cDecimalPoint, cThousandsSep, sGrouping, sCurrSymbol, sPositiveSign, sNegativeSign, iFracDigits);
		}
		void Set(CharT cDecimalPoint, CharT cThousandsSep, std::string sGrouping, std::basic_string<CharT> sCurrSymbol, std::basic_string<CharT> sPositiveSign, std::basic_string<CharT> sNegativeSign, int32 iFracDigits)
		{
			m_cDecimalPoint = cDecimalPoint;
			m_cThousandsSep = cThousandsSep;
			m_sGrouping = sGrouping;
			m_sCurrSymbol = sCurrSymbol;
			m_sPositiveSign = sPositiveSign;
			m_sNegativeSign = sNegativeSign;
			m_iFracDigits = iFracDigits;
		}
	};
	static FallbackNumpunctData<char>	sm_FallbackNumpunctData;
	static FallbackNumpunctData<wchar_t>sm_FallbackNumpunctWideData;
	static std::map<std::string, int32>	sm_Iso4217Map;

public:
	// Globally sets all HyLocale formatting
	// if no ICU library, specified locale must be supported by the operating system or maintained in Harmony's fallback table
	// sIso639Code: 2 letter lowercase Language Code (example: en)
	// sIso3166Code: 2 letter uppercase Country Code (example: US)
	// sIso4217Code: 3 letter uppercase Currency Code (example: USD)
	static void Imbue(std::string sIso639Code, std::string sIso3166Code, std::string sIso4217Code);
	static void Imbue(std::string sLangCountryCode, std::string sIso4217Code); // sLangCountryCode: a combined ISO639 and ISO3166 code delimited with a '-' or a '_'

	// Once specified, HyNumberFormat's used in Money_Format() can optionally use the minor
	// fractional symbol if the value is less than 1 integer unit (aka 50¢)
	// sMinorCurrencyUnitUtf8: UTF-8 string representing the minor currency symbol to use
	static void SetMinorCurrencySymbol(std::string sMinorCurrencySymbolUtf8);

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
#endif

	static std::locale AssembleStdLocale();
	static void AssembleFallbackNumpunct();

	template<typename CharT>
	static FallbackNumpunctData<CharT> GetFallbackNumpunctData();
};

#endif /* HyLocale_h__ */
