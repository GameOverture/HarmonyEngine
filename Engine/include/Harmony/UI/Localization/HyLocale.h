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

#include <locale>
#include <iomanip>

#if HY_USE_ICU
	#define U_CHARSET_IS_UTF8 1
	#include <unicode/numberformatter.h>
	
	#ifdef HY_PLATFORM_BROWSER
		using namespace icu_62;
	#else
		using namespace icu;
	#endif
#endif

class HyLocale
{
	static std::string		sm_sIso639Code;			// Language Code
	static std::string		sm_sIso3166Code;		// Country Code
	static std::string		sm_sIso4217Code;		// Currency Code

	static std::string		sm_sMinorCurrencySymbol;// Currency "cent" symbol

public:
	// Globally sets all HyLocale formatting
	// if no ICU library, specified locale must be supported by the operating system
	// sIso639Code: 2 letter lowercase Language Code (example: en)
	// sIso3166Code: 2 letter uppercase Country Code (example: US)
	// sIso4217Code: ***ONLY USED WITH ICU LIBRARY*** 3 letter uppercase Currency Code (example: USD)
	static void Imbue(std::string sIso639Code, std::string sIso3166Code, std::string sIso4217Code = "");

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
	static std::string AssembleStdLocaleString();

#ifdef HY_USE_ICU
	static icu::number::LocalizedNumberFormatter AssembleFormatter(HyNumberFormat format);
#else
	template <typename CharT, typename ValueT>
	class HyLocale_numberpunct : public std::numpunct_byname<CharT>
	{
		HyNumberFormat				m_NumberFormat;
		ValueT						m_Value;

	public:
		explicit HyLocale_numberpunct(HyNumberFormat format, ValueT value, std::string sLocaleName, std::size_t refs = 0) :
			std::numpunct_byname<CharT>(sLocaleName, refs),
			m_NumberFormat(format),
			m_Value(value)
		{ }

		virtual ~HyLocale_numberpunct() = default;

	protected:
		// Provides the numbers of digits between each pair of thousands separators
		virtual string_type do_grouping() const override
		{
			switch(m_NumberFormat.GetGrouping())
			{
			default:
			case HYFMTGROUPING_Default:
				return std::numpunct_byname<CharT>::do_grouping();

			case HYFMTGROUPING_Off:
				return "";

			case HYFMTGROUPING_Min2:
				if(m_Value >= 10000)
					return std::numpunct_byname<CharT>::do_grouping();
				else
					return "";

			case HYFMTGROUPING_Thousands:
				return "\003";
			}
		}
	};

	template <typename CharT, bool bIntSyms, typename ValueT>
	class HyLocale_moneypunct : public std::moneypunct_byname<CharT, bIntSyms>
	{
		HyNumberFormat				m_NumberFormat;
		ValueT						m_Value;

	public:
		explicit HyLocale_moneypunct(HyNumberFormat format, ValueT value, std::string sLocaleName, std::size_t refs = 0) :
			std::moneypunct_byname<CharT, bIntSyms>(sLocaleName, refs),
			m_NumberFormat(format),
			m_Value(value)
		{ }

		virtual ~HyLocale_moneypunct() = default;

	protected:
		// Provides the numbers of digits between each pair of thousands separators
		virtual string_type do_grouping() const override
		{
			switch(m_NumberFormat.GetGrouping())
			{
			default:
			case HYFMTGROUPING_Default:
				return std::moneypunct_byname<CharT, bIntSyms>::do_grouping();

			case HYFMTGROUPING_Off:
				return "";

			case HYFMTGROUPING_Min2: {
				int32 iNumFractionDigits = frac_digits();
				double dDenominator = 1.0;
				for(int32 i = 0; i < iNumFractionDigits; ++i)
					dDenominator *= 10.0;
				if((m_Value / dDenominator) >= 10000)
					return std::moneypunct_byname<CharT, bIntSyms>::do_grouping();
				else
					return "";
			}

			case HYFMTGROUPING_Thousands:
				return "\003";
			}
		}

		// Provides the string to indicate a positive value
		virtual string_type do_positive_sign() const override
		{
			switch(m_NumberFormat.GetSign())
			{
			default:
			case HYFMTSIGN_Default:
			case HYFMTSIGN_Always:
			case HYFMTSIGN_Accounting:
			case HYFMTSIGN_AccountingAlways:
				return std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign();

			case HYFMTSIGN_AlwaysExceptZero:
			case HYFMTSIGN_AccountingExceptZero:
				if(m_Value != 0)
					return std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign();
				else
					return "";

			case HYFMTSIGN_Never:
				return "";
			}
		}

		// Provides the string to indicate a negative value
		virtual string_type do_negative_sign() const override
		{
			switch(m_NumberFormat.GetSign())
			{
			default:
			case HYFMTSIGN_Default:
			case HYFMTSIGN_Always:
				return std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign();

			case HYFMTSIGN_AlwaysExceptZero:
				if(m_Value != 0)
					return std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign();
				else
					return "";

			case HYFMTSIGN_Never:
				return "";

			case HYFMTSIGN_Accounting:
			case HYFMTSIGN_AccountingAlways:
				return "()";

			case HYFMTSIGN_AccountingExceptZero:
				if(m_Value != 0)
					return "()";
				else
					return "";
			}
		}
	};
#endif
};

#endif /* HyLocale_h__ */
