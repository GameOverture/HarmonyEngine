/**************************************************************************
 *	HyWindow_Win.h
 *
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyWindow_Win_h__
#define __HyWindow_Win_h__

#include "Afx/HyStdAfx.h"

#include "Renderer/Interop/OpenGL/HyOpenGL.h"

class HyWindow_Win : public HyOpenGL
{
public:
	HyWindow_Win();
	~HyWindow_Win();
};

#endif /* __HyWindow_Win_h__ */
