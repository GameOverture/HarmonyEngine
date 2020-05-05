/**************************************************************************
 *	HyCompiler_gnu.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyCompiler_GNU_h__
#define HyCompiler_GNU_h__

#define HY_PACKDATA_BEGIN(align)
#define HY_PACKDATA_END(align)		__attribute__ ((__aligned__(align)))

#define HY_INLINE					__inline__

#ifdef DEBUG
	#define HY_DEBUG
#else
	#define HY_RELEASE
#endif

//#if __has_feature(cxx_noexcept)
//	#define HY_NOEXCEPT noexcept
//#else
	#define HY_NOEXCEPT
//#endif


#if UINTPTR_MAX == 0xffffffff
	#define HY_ENV_32
#else
	#define HY_ENV_64
#endif

#endif /* HyCompiler_GNU_h__ */
