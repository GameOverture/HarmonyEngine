/**************************************************************************
 *	HyCompiler_msvc.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyCompiler_msvc_h__
#define __HyCompiler_msvc_h__

#ifdef _DEBUG
  #define HY_DEBUG
#else
  #define HY_RELEASE
#endif

#define HY_PACKDATA_BEGIN(align)	__declspec(align(align)) 
#define HY_PACKDATA_END(align)

#define HY_INLINE					__inline
#define HY_FORCE_INLINE				__forceinline
    
//
// Switch off the most annoying warnings
//
//#pragma warning(disable : 4100)      // unreferenced formal parameter
//#pragma warning(disable : 4511)      // copy constructor could not be generated
//#pragma warning(disable : 4512)      // assignment constructor could not be generated
//#pragma warning(disable : 4663)      // C++ language change
//#pragma warning(disable : 4786)      // identifier was truncated to '255' characters

// This undefines the macros MIN and MAX which are specified in the windows headers. Use the stl versions instead.
#define NOMINMAX

#endif /* __HyCompiler_msvc_h__ */
