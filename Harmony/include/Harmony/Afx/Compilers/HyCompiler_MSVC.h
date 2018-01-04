/**************************************************************************
 *	HyCompiler_MSVC.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyCompiler_MSVC_h__
#define HyCompiler_MSVC_h__

#ifdef _DEBUG
	#define HY_DEBUG
#else
	#define HY_RELEASE
#endif

#define HY_PACKDATA_BEGIN(align)	__declspec(align(align)) 
#define HY_PACKDATA_END(align)

#define HY_INLINE					__inline
#define HY_FORCE_INLINE				__forceinline

#if defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023026
	#define HY_NOEXCEPT noexcept
#else
	#define HY_NOEXCEPT
#endif

// This undefines the macros MIN and MAX which are specified in the windows headers. Use the stl versions instead.
#define NOMINMAX

#ifdef HY_DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>

	#define HY_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)	// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the allocations to be of _CLIENT_BLOCK type

	#define  HY_SET_CRT_DEBUG_FIELD(a)		_CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
	#define  HY_CLEAR_CRT_DEBUG_FIELD(a)	_CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
	#define HY_NEW new
#endif

#if defined(_WIN64)
	#define HY_ENV_64
#else
	#define HY_ENV_32
#endif

#define HY_COMPILER HY_COMPILER_MSVC

#endif /* HyCompiler_MSVC_h__ */
