/**************************************************************************
 *	HyPlatform.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyPlatform_h__
#define HyPlatform_h__

#include "Afx/Platforms/HyPlatAfx.h"

//-----------------------------------------------------------------------------------------
// Endian - Don't use these functions directly, use the #define's below 'em
//-----------------------------------------------------------------------------------------
HY_INLINE uint16 HyInternal_EndianSwap16__(uint16 var)
{
	return (uint16)((((var)&0xff)<<8) | ( ((var)>>8)&0xff));
}

HY_INLINE uint32 HyInternal_EndianSwap32__(uint32 var)
{
	return ((((var)&0xff)<<24) | ((((var)>>8)&0xff)<<16) |
		((((var)>>16)&0xff)<<8) | ((((var)>>24)&0xff)) );
}

#if defined(HY_ENDIAN_LITTLE)
	#define Hy_EndianToBig16(_v)	HyInternal_EndianSwap16__(_v)
	#define Hy_EndianToLittle16(_v)
	#define Hy_EndianToBig32(_v)	HyInternal_EndianSwap32__(_v)
	#define Hy_EndianToLittle32(_v)
#elif defined(HY_ENDIAN_BIG)
	#define Hy_EndianToBig16(_v)
	#define Hy_EndianToLittle16(_v)	HyInternal_EndianSwap16__(_v)
	#define Hy_EndianToBig32(_v)
	#define Hy_EndianToLittle32(_v)	HyInternal_EndianSwap32__(_v)
#else
	#error No Endian is set.
#endif

//-----------------------------------------------------------------------------------------
// Misc
//-----------------------------------------------------------------------------------------
#define HY_TCP_PORT 1313

// The size of an enum can vary, so add this element to the end
// of any enumeration we use that's intended for saving to disc
// which should force it to 4-bytes
#define HY_ENUM_PADDING	0x7fffffff

//-----------------------------------------------------------------------------------------
// Safety checks
//-----------------------------------------------------------------------------------------
#if (defined(HY_PLATFORM_32BIT) && defined(HY_PLATFORM_64BIT)) || (!defined(HY_PLATFORM_32BIT) && !defined(HY_PLATFORM_64BIT))
#error HyPlatform.h: Invalid Architecture definition.
#endif

#if defined(HY_PLATFORM_UNKNOWN)
#error HyPlatform.h: Could not determine build platform.
#endif

#ifndef HY_MAXWINDOWS
#error HY_MAXWINDOWS hasn't been defined by the platform
#endif

#ifndef HyAssert
#error HyPlatform.h: HyAssert is not defined!
#endif

#ifndef HyError
#error HyPlatform.h: HyError is not defined!
#endif

#endif // HyPlatform_h__
