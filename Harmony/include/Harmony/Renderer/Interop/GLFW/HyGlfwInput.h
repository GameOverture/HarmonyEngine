/**************************************************************************
 *	HyGlfwInput.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyGlfwInput_h__
#define __HyGlfwInput_h__

#include "Afx/HyStdAfx.h"

#ifndef HY_PLATFORM_GUI

#include "Threading/BasicSync.h"

class HyGlfwInput
{
	static uint64			sm_ui64StdKeyFlags;		// Offset of '32'
	static uint64			sm_ui64ExtKeyFlags;		// Offset of '256'
	static uint32			sm_ui32NumPadKeyFlags;	// Offset of '320'

	static BasicSection		sm_cs;

public:
	HyGlfwInput(void);
	virtual ~HyGlfwInput(void);
	
	static void SetKey(int iKey, int iAction, int iMod);
	static void GetKeyboardState(uint64 *pKeyFlags);

	static void Lock()		{ sm_cs.Lock(); }
	static void Unlock()	{ sm_cs.Unlock(); }
};

#endif /* !HY_PLATFORM_GUI */

#endif /* __HyGlfwInput_h__ */
