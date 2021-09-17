/**************************************************************************
 *	HyFlags.h
 *
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyFlags_h__
#define HyFlags_h__

//class HyFlag
//{
//    int i;
//public:
//    constexpr inline HyFlag(int value) noexcept : i(value) {}
//    constexpr inline operator int() const noexcept { return i; }
//
//    constexpr inline HyFlag(uint32_t value) noexcept : i(int(value)) {}
//    constexpr inline HyFlag(int16_t value) noexcept : i(int(value)) {}
//    constexpr inline HyFlag(uint16_t value) noexcept : i(int(uint32_t(value))) {}
//    constexpr inline operator uint32_t() const noexcept { return uint32_t(i); }
//};
//
//template<typename Enum>
//class HyFlags
//{
//	static_assert((sizeof(Enum) <= sizeof(int)), "HyFlags uses an int as storage, so an enum with underlying long long will overflow.");
//	static_assert((std::is_enum<Enum>::value), "HyFlags is only usable on enumeration types.");
//
//public:
//#if 1//HY_COMPILER_MSVC
//	// Microsoft Visual Studio has buggy behavior when it comes to
//	// unsigned enums: even if the enum is unsigned, the enum tags are
//	// always signed
//	typedef int Int;
//#else
//	typedef typename std::conditional<std::is_unsigned<typename std::underlying_type<Enum>::type>::value, unsigned int, signed int>::type Int;
//#endif
//
//	// compiler-generated copy/move ctor/assignment operators are fine!
//	constexpr inline HyFlags() noexcept : i(0) {}
//	constexpr inline HyFlags(Enum flags) noexcept : i(Int(flags)) {}
//	constexpr inline HyFlags(HyFlag flag) noexcept : i(flag) {}
//
//	//constexpr inline HyFlags(std::initializer_list<Enum> flags) noexcept
//	//	: i(initializer_list_helper(flags.begin(), flags.end())) {}
//
//	constexpr inline HyFlags &operator&=(int mask) noexcept { i &= mask; return *this; }
//	constexpr inline HyFlags &operator&=(uint32_t mask) noexcept { i &= mask; return *this; }
//	constexpr inline HyFlags &operator&=(Enum mask) noexcept { i &= Int(mask); return *this; }
//	constexpr inline HyFlags &operator|=(HyFlags other) noexcept { i |= other.i; return *this; }
//	constexpr inline HyFlags &operator|=(Enum other) noexcept { i |= Int(other); return *this; }
//	constexpr inline HyFlags &operator^=(HyFlags other) noexcept { i ^= other.i; return *this; }
//	constexpr inline HyFlags &operator^=(Enum other) noexcept { i ^= Int(other); return *this; }
//
//	constexpr inline operator Int() const noexcept { return i; }
//
//	constexpr inline HyFlags operator|(HyFlags other) const noexcept { return HyFlags(HyFlag(i | other.i)); }
//	constexpr inline HyFlags operator|(Enum other) const noexcept { return HyFlags(HyFlag(i | Int(other))); }
//	constexpr inline HyFlags operator^(HyFlags other) const noexcept { return HyFlags(HyFlag(i ^ other.i)); }
//	constexpr inline HyFlags operator^(Enum other) const noexcept { return HyFlags(HyFlag(i ^ Int(other))); }
//	constexpr inline HyFlags operator&(int mask) const noexcept { return HyFlags(HyFlag(i & mask)); }
//	constexpr inline HyFlags operator&(uint32_t mask) const noexcept { return HyFlags(HyFlag(i & mask)); }
//	constexpr inline HyFlags operator&(Enum other) const noexcept { return HyFlags(HyFlag(i & Int(other))); }
//	constexpr inline HyFlags operator~() const noexcept { return HyFlags(HyFlag(~i)); }
//
//	constexpr inline bool operator!() const noexcept { return !i; }
//
//	constexpr inline bool testFlag(Enum flag) const noexcept { return (i & Int(flag)) == Int(flag) && (Int(flag) != 0 || i == Int(flag)); }
//	constexpr inline HyFlags &setFlag(Enum flag, bool on = true) noexcept
//	{
//		return on ? (*this |= flag) : (*this &= ~Int(flag));
//	}
//
//private:
//	//constexpr static inline Int initializer_list_helper(typename std::initializer_list<Enum>::const_iterator it,
//	//	typename std::initializer_list<Enum>::const_iterator end)
//	//	noexcept
//	//{
//	//	return (it == end ? Int(0) : (Int(*it) | initializer_list_helper(it + 1, end)));
//	//}
//
//	Int i;
//};
//#define HY_DECLARE_FLAGS(Flags, Enum) typedef HyFlags<Enum> Flags;

#endif // HyFlags_h__
