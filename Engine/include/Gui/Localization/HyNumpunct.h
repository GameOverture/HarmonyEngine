/**************************************************************************
*	HyNumpunct.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyNumpunct_h__
#define HyNumpunct_h__

#include "Afx/HyStdAfx.h"
#include "Gui/Localization/HyLocale.h"

#include <locale>
#include <iomanip>

template <typename CharT, typename ValueT>
class HyNumpunct : public std::numpunct_byname<CharT>
{
	HyNumberFormat				m_NumberFormat;
	ValueT						m_Value;

public:
	explicit HyNumpunct(HyNumberFormat format, ValueT value, std::string sLocaleName, std::size_t refs = 0) :
		std::numpunct_byname<CharT>(sLocaleName, refs),
		m_NumberFormat(format),
		m_Value(value)
	{ }

	virtual ~HyNumpunct() = default;

protected:
	virtual CharT do_decimal_point() const override
	{
		// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
		if constexpr(std::is_same<CharT, char>::value)
		{
			if(std::numpunct_byname<CharT>::do_decimal_point() == '.')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cDecimalPoint;
		}
		else
		{
			if(std::numpunct_byname<CharT>::do_decimal_point() == L'.')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cDecimalPoint;
		}

		return std::numpunct_byname<CharT>::do_decimal_point();
	}

	virtual CharT do_thousands_sep() const override
	{
		// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
		if constexpr(std::is_same<CharT, char>::value)
		{
			if(std::numpunct_byname<CharT>::do_thousands_sep() == ',')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cThousandsSep;
		}
		else
		{
			if(std::numpunct_byname<CharT>::do_thousands_sep() == L',')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cThousandsSep;
		}
		
		return std::numpunct_byname<CharT>::do_thousands_sep();
	}

	// Provides the numbers of digits between each pair of thousands separators
	virtual std::string do_grouping() const override
	{
		switch(m_NumberFormat.GetGrouping())
		{
		default:
		case HYFMTGROUPING_Default:
			if(std::numpunct_byname<CharT>::do_grouping().empty())
				return HyLocale::GetFallbackNumpunctData<CharT>().m_sGrouping;
			return std::numpunct_byname<CharT>::do_grouping();

		case HYFMTGROUPING_Off:
			return "";

		case HYFMTGROUPING_Min2:
			if(m_Value >= 10000)
			{
				if(std::numpunct_byname<CharT>::do_grouping().empty())
					return HyLocale::GetFallbackNumpunctData<CharT>().m_sGrouping;
				return std::numpunct_byname<CharT>::do_grouping();
			}
			else
				return "";

		case HYFMTGROUPING_Thousands:
			return "\003";
		}
	}
};

template <typename CharT, bool bIntSyms, typename ValueT>
class HyMoneypunct : public std::moneypunct_byname<CharT, bIntSyms>
{
	HyNumberFormat				m_NumberFormat;
	ValueT						m_Value;

public:
	explicit HyMoneypunct(HyNumberFormat format, ValueT value, std::string sLocaleName, std::size_t refs = 0) :
		std::moneypunct_byname<CharT, bIntSyms>(sLocaleName, refs),
		m_NumberFormat(format),
		m_Value(value)
	{ }

	virtual ~HyMoneypunct() = default;

protected:

	virtual CharT do_decimal_point() const override
	{
		// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
		if constexpr(std::is_same<CharT, char>::value)
		{
			if(std::moneypunct_byname<CharT, bIntSyms>::do_decimal_point() == 127)//'.')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cDecimalPoint;
		}
		else
		{
			if(std::moneypunct_byname<CharT, bIntSyms>::do_decimal_point() == 127)//L'.')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cDecimalPoint;
		}

		return std::moneypunct_byname<CharT, bIntSyms>::do_decimal_point();
	}

	virtual CharT do_thousands_sep() const override
	{
		// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
		if constexpr(std::is_same<CharT, char>::value)
		{
			if(std::moneypunct_byname<CharT, bIntSyms>::do_thousands_sep() == 127)//',')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cThousandsSep;
		}
		else
		{
			if(std::moneypunct_byname<CharT, bIntSyms>::do_thousands_sep() == 127)//L',')
				return HyLocale::GetFallbackNumpunctData<CharT>().m_cThousandsSep;
		}
		
		return std::moneypunct_byname<CharT, bIntSyms>::do_thousands_sep();
	}

	// Provides the numbers of digits between each pair of thousands separators
	virtual std::string do_grouping() const override
	{
		switch(m_NumberFormat.GetGrouping())
		{
		default:
		case HYFMTGROUPING_Default:
			if(std::moneypunct_byname<CharT, bIntSyms>::do_grouping().empty())
				return HyLocale::GetFallbackNumpunctData<CharT>().m_sGrouping;
			return std::moneypunct_byname<CharT, bIntSyms>::do_grouping();

		case HYFMTGROUPING_Off:
			return "";

		case HYFMTGROUPING_Min2: {
			int32 iNumFractionDigits = this->frac_digits();
			double dDenominator = 1.0;
			for(int32 i = 0; i < iNumFractionDigits; ++i)
				dDenominator *= 10.0;
			if((m_Value / dDenominator) >= 10000)
			{
				if(std::moneypunct_byname<CharT, bIntSyms>::do_grouping().empty())
					return HyLocale::GetFallbackNumpunctData<CharT>().m_sGrouping;
				return std::moneypunct_byname<CharT, bIntSyms>::do_grouping();
			}
			else
				return "";
		}

		case HYFMTGROUPING_Thousands:
			return "\003";
		}
	}

	virtual std::basic_string<CharT> do_curr_symbol() const override
	{
		//std::string sCurrSym = std::moneypunct_byname<CharT, bIntSyms>::do_curr_symbol();
		//if(sCurrSym.empty())
		//{
			if(m_NumberFormat.IsUsingCurrencySymbol())
				return HyLocale::GetFallbackNumpunctData<CharT>().m_sCurrSymbol;
			else
			{
				std::basic_string<CharT> sIsoCodeWithSpace = HyLocale::sm_sIso4217Code;
				sIsoCodeWithSpace += " ";
				return sIsoCodeWithSpace;
			}
		//}

	//	return sCurrSym;
	}

	// Provides the string to indicate a positive value
	virtual std::basic_string<CharT> do_positive_sign() const override
	{
		switch(m_NumberFormat.GetSign())
		{
		default:
		case HYFMTSIGN_Default:
		case HYFMTSIGN_Always:
		case HYFMTSIGN_Accounting:
		case HYFMTSIGN_AccountingAlways:
			// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
			if constexpr(std::is_same<CharT, char>::value)
			{
				if(std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign() == "+")
					return HyLocale::GetFallbackNumpunctData<CharT>().m_sPositiveSign;
			}
			else
			{
				if(std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign() == L"+")
					return HyLocale::GetFallbackNumpunctData<CharT>().m_sPositiveSign;
			}
			return std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign();

		case HYFMTSIGN_AlwaysExceptZero:
		case HYFMTSIGN_AccountingExceptZero:
			if(m_Value != 0)
			{
				// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
				if constexpr(std::is_same<CharT, char>::value)
				{
					if(std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign() == "+")
						return HyLocale::GetFallbackNumpunctData<CharT>().m_sPositiveSign;
				}
				else
				{
					if(std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign() == L"+")
						return HyLocale::GetFallbackNumpunctData<CharT>().m_sPositiveSign;
				}
				return std::moneypunct_byname<CharT, bIntSyms>::do_positive_sign();
			}
			else
				return "";

		case HYFMTSIGN_Never:
			return "";
		}
	}

	// Provides the string to indicate a negative value
	virtual std::basic_string<CharT> do_negative_sign() const override
	{
		switch(m_NumberFormat.GetSign())
		{
		default:
		case HYFMTSIGN_Default:
		case HYFMTSIGN_Always:
			// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
			if constexpr(std::is_same<CharT, char>::value)
			{
				if(std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign() == "-")
					return HyLocale::GetFallbackNumpunctData<CharT>().m_sNegativeSign;
			}
			else
			{
				if(std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign() == L"-")
					return HyLocale::GetFallbackNumpunctData<CharT>().m_sNegativeSign;
			}
			return std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign();

		case HYFMTSIGN_AlwaysExceptZero:
			if(m_Value != 0)
			{
				// TODO: Find a better way of determining whether the default 'C' locale is set, then rely on fallback
				if constexpr(std::is_same<CharT, char>::value)
				{
					if(std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign() == "-")
						return HyLocale::GetFallbackNumpunctData<CharT>().m_sNegativeSign;
				}
				else
				{
					if(std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign() == L"-")
						return HyLocale::GetFallbackNumpunctData<CharT>().m_sNegativeSign;
				}
				return std::moneypunct_byname<CharT, bIntSyms>::do_negative_sign();
			}
			else
				return "";

		case HYFMTSIGN_Never:
			return "";

		case HYFMTSIGN_Accounting:
		case HYFMTSIGN_AccountingAlways:
			if constexpr(std::is_same<CharT, char>::value)
				return "()";
			else
				return L"()";

		case HYFMTSIGN_AccountingExceptZero:
			if(m_Value != 0)
			{
				if constexpr(std::is_same<CharT, char>::value)
					return "()";
				else
					return L"()";
			}
			else
				return "";
		}
	}

	virtual int32 do_frac_digits() const override
	{
		int32 iNumFracDigits = std::moneypunct_byname<CharT, bIntSyms>::do_frac_digits();
		if(iNumFracDigits == 0)
			return HyLocale::GetFallbackNumpunctData<CharT>().m_iFracDigits;
		return iNumFracDigits;
	}

	virtual std::money_base::pattern do_pos_format() const override
	{
		if(HyLocale::GetFallbackNumpunctData<CharT>().m_bSymbolOnLeft)
			return std::moneypunct_byname<CharT, bIntSyms>::do_pos_format();

		std::money_base::pattern posFormat;
		posFormat.field[0] = std::money_base::sign;
		posFormat.field[1] = std::money_base::value;
		posFormat.field[2] = std::money_base::space;
		posFormat.field[3] = std::money_base::symbol;

		return posFormat;
	}

	virtual std::money_base::pattern do_neg_format() const override
	{
		if(HyLocale::GetFallbackNumpunctData<CharT>().m_bSymbolOnLeft)
			return std::moneypunct_byname<CharT, bIntSyms>::do_neg_format();

		std::money_base::pattern posFormat;
		posFormat.field[0] = std::money_base::sign;
		posFormat.field[1] = std::money_base::value;
		posFormat.field[2] = std::money_base::space;
		posFormat.field[3] = std::money_base::symbol;

		return posFormat;
	}
};

#endif /* HyNumpunct_h__ */
