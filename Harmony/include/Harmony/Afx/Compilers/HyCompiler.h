/**************************************************************************
 *	HyCompiler.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyCompiler_h__
#define __HyCompiler_h__

#include <string>

// These macros use the (compiler-specific) project settings
// to create independent versions for the game.
#if !defined(HARMONY_COMPILER_DEFINED)

	#if defined(_UNICODE)

		#define HY_UNICODE
		typedef std::wstring HyString;
		typedef std::wostringstream HyStringStream;
		typedef wchar_t HyChar;
		#define HyStr(x) L ## x
	#else
		#define HY_ASCII
		typedef std::string HyString;
		typedef std::ostringstream HyStringStream;
		typedef char HyChar;
		#define HyStr(x) x
	#endif

	#if defined(_MSC_VER)
		#include "HyCompiler_MSVC.h"
	#elif defined(__MWERKS__)
		#include "HyCompiler_MWerks.h"
	#elif defined(__GNUC__)
		#include "HyCompiler_GNU.h"
	#endif

	#define HARMONY_COMPILER_DEFINED
#endif

#if defined(_HARMONYGUI)
	#define HY_PLATFORM_GUI
#endif

#ifndef _HARMONYSINGLETHREAD
	#define HY_MULTITHREADING
#endif

//
// Add any generic macros the compiler doesn't support
//

// For manually aligning data
#ifndef HY_PACKDATA_BEGIN
  #define HY_PACKDATA_BEGIN(align)
#endif

#ifndef HY_PACKDATA_END
  #define HY_PACKDATA_END(align)
#endif

// Compiler support
#ifndef HY_INLINE
  #define HY_INLINE        inline
#endif

#ifndef HY_FORCE_INLINE
  #define HY_FORCE_INLINE  inline
#endif

//#ifndef SGXAPI
//  #define SGXAPI
//#endif


//-----------------------------------------------------------------------------------------
// Safety checks
//-----------------------------------------------------------------------------------------
#if (defined(HY_DEBUG) && defined(HY_RELEASE)) || (!defined(HY_DEBUG) && !defined(HY_RELEASE))
	#error HyCompiler.h: Invalid Build Configuration.
#endif

#if (defined(HY_ENV_32) && defined(HY_ENV_64)) || (!defined(HY_ENV_32) && !defined(HY_ENV_64))
	#error HyCompiler.h: Invalid Platform Environment.
#endif

#endif /* __HyCompiler_h__ */
