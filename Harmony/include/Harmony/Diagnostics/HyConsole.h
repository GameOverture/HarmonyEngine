/**************************************************************************
 *	HyConsole.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2017 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyConsole_h__
#define __HyConsole_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Nodes/Draws/HyPrimitive2d.h"

class HyConsole
{
	HyPrimitive2d		m_Backdrop;

public:
	HyConsole();
	~HyConsole();
};

#endif /*__HyConsole_h__*/
