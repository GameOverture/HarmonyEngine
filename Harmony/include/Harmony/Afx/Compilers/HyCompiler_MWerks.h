/**************************************************************************
 *	HyCompiler_mwerks.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyCompiler_mwerks_h__
#define __HyCompiler_mwerks_h__

#define HY_PACKDATA_BEGIN(align)
#define HY_PACKDATA_END(align)		__attribute__ ((aligned(align)))

#define HY_INLINE					__inline

#endif /* __HyCompiler_mwerks_h__ */
