/**************************************************************************
 *	HyGlfwInput.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Renderer/Interop/GLFW/HyGlfwInput.h"

#ifndef HY_PLATFORM_GUI

/*static*/ BasicSection HyGlfwInput::sm_cs;

/*static*/ uint64		HyGlfwInput::sm_ui64StdKeyFlags = 0;		// Offset of '32'
/*static*/ uint64		HyGlfwInput::sm_ui64ExtKeyFlags = 0;		// Offset of '256'
/*static*/ uint32		HyGlfwInput::sm_ui32NumPadKeyFlags = 0;	// Offset of '320'


HyGlfwInput::HyGlfwInput(void)
{
}


HyGlfwInput::~HyGlfwInput(void)
{
}

/*static*/ void HyGlfwInput::GetKeyboardState(uint64 *pKeyFlags)
{
	sm_cs.Lock();
	pKeyFlags[0] = sm_ui64StdKeyFlags;
	pKeyFlags[1] = sm_ui64ExtKeyFlags;
	pKeyFlags[2] = sm_ui32NumPadKeyFlags;
	sm_cs.Unlock();
}

/*static*/ void HyGlfwInput::SetKey(int iKey, int iAction, int iMod)
{
	if(iKey == GLFW_KEY_UNKNOWN)
		return;

	if(iAction == GLFW_PRESS || iAction == GLFW_REPEAT)
	{
		if(iKey >= 32 && iKey < 256)
			sm_ui64StdKeyFlags |= static_cast<uint64>(1 << (iKey - 32));
		else if(iKey < 320)
			sm_ui64ExtKeyFlags |= static_cast<uint64>(1 << (iKey - 256));
		else if(iKey <= GLFW_KEY_LAST)
			sm_ui32NumPadKeyFlags |= static_cast<uint32>(1 << (iKey - 320));
	}
	else
	{
		if(iKey >= 32 && iKey < 256)
			sm_ui64StdKeyFlags &= ~static_cast<uint64>(1 << (iKey - 32));
		else if(iKey < 320)
			sm_ui64ExtKeyFlags &= ~static_cast<uint64>(1 << (iKey - 256));
		else if(iKey <= GLFW_KEY_LAST)
			sm_ui32NumPadKeyFlags &= ~static_cast<uint32>(1 << (iKey - 320));
	}
}

#endif /* !HY_PLATFORM_GUI */
