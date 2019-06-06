/**************************************************************************
*	HyRand.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyRand_h__
#define HyRand_h__

#include "Afx/HyStdAfx.h"

#include <random>
#include <chrono>

template<typename T>
struct hy_is_type_integer
{
	static constexpr bool value = std::is_same<T, int16>::value ||
		std::is_same<T, int32>::value ||
		std::is_same<T, int64>::value ||
		std::is_same<T, uint16>::value ||
		std::is_same<T, uint32>::value ||
		std::is_same<T, uint64>::value;
};

template<typename T>
struct hy_is_type_real
{
	static constexpr bool value = std::is_same<T, float>::value ||
		std::is_same<T, double>::value ||
		std::is_same<T, long double>::value;
};

template<typename T>
struct hy_is_type_byte
{
	static constexpr bool value = std::is_same<T, int8>::value ||
		std::is_same<T, uint8>::value;
};

template<typename T>
struct hy_is_type_character
{
	static constexpr bool value = std::is_same<T, char>::value ||
		std::is_same<T, wchar_t>::value ||
		std::is_same<T, char16_t>::value ||
		std::is_same<T, char32_t>::value;
};

class HyRand
{
public:
	static void Reseed(const std::mt19937::result_type seedValue = std::mt19937::default_seed)
	{
		GetPseudoRandInstance().seed(seedValue);
	}

	static void Reseed(const std::seed_seq &seedSeqValueRef)
	{
		GetPseudoRandInstance().seed(seedSeqValueRef);
	}

	// Generate a random value based on template parameter
	template<typename T>
	static T Get()
	{
		return Range<T>(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
	}

	// Generate a random value in a [min; max] range
	template<typename T>
	static typename std::enable_if<hy_is_type_integer<T>::value, T>::type Range(T min, T max)
	{
		if(min < max)
			return std::uniform_int_distribution<T>{ min, max }(GetPseudoRandInstance());

		return std::uniform_int_distribution<T>{ max, min }(GetPseudoRandInstance());
	}

	// Generate a random value in a [min; max] range
	template<typename T>
	static typename std::enable_if<hy_is_type_real<T>::value, T>::type Range(T min, T max)
	{
		if(min < max)
			return std::uniform_real_distribution<T>{ min, max }(GetPseudoRandInstance());

		return std::uniform_real_distribution<T>{ max, min }(GetPseudoRandInstance());
	}

	// Generate a random value in a [min; max] range
	template<typename T>
	static typename std::enable_if<hy_is_type_byte<T>::value, T>::type Range(T min, T max)
	{
		// Choose between short and unsigned short for byte conversion
		using short_t = typename std::conditional<std::is_signed<T>::value, short, unsigned short>::type;
		return static_cast<T>( Range<short_t>(min, max));
	}

	// Generate a random value in a [min; max] range
	template<typename T>
	static typename std::enable_if<hy_is_type_character<T>::value, T>::type Range(T min, T max)
	{
		if(min < max)
			return static_cast<T>(std::uniform_int_distribution<std::int64_t>{ static_cast<std::int64_t>(min), static_cast<std::int64_t>(max) }(GetPseudoRandInstance()));

		return static_cast<T>(std::uniform_int_distribution<std::int64_t>{ static_cast<std::int64_t>(max), static_cast<std::int64_t>(min) }(GetPseudoRandInstance()));
	}

	// Generate a bool value with a weighted probability of being true, using std::bernoulli_distribution
	static bool Boolean(const double dProbability = 0.5)
	{
		HyAssert(0 <= dProbability && 1 >= dProbability, "HyRand::Boolean was passed an invalid probability: " << dProbability);
		return std::bernoulli_distribution{ dProbability }(GetPseudoRandInstance());
	}

private:
	static std::mt19937 &GetPseudoRandInstance()
	{
		static std::mt19937 s_engine(static_cast<uint32>(std::chrono::steady_clock::now().time_since_epoch().count()));
		return s_engine;
	}
};

#endif /* HyRand_h__ */
