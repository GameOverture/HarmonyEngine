/**************************************************************************
*	HyMain.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyMain_h__
#define HyMain_h__

#include "Afx/HyStdAfx.h"

#if !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)
	
	#define main Hy_main
	extern "C"
	{
		// The prototype for the application's main() function
		extern int Hy_main(int argc, char *argv[]);
	}

#endif // !defined(HY_USE_SDL2) && defined(HY_PLATFORM_WINDOWS)

#endif /* HyMain_h__ */
