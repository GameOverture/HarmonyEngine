/**************************************************************************
 *	HyConole.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyConole_h__
#define __HyConole_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Nodes/Draws/HyPrimitive2d.h"

class HyConsole
{
	HyPrimitive2d		m_Backdrop;

public:
	HyConsole();
	~HyConsole();
};

#endif /*__HyConole_h__*/
