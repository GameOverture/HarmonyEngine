/**************************************************************************
 *	Player.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __Player_h__
#define __Player_h__

#include "stdafx.h"

class Player
{
	HySpine2d		m_spineBoy;
	HyPrimitive2d	m_primBox;
	HyText2d		m_txtName;

	HySprite2d		m_testSprite;

public:
	Player(void);
	virtual ~Player(void);

	void Initialize();
};

#endif /* __Player_h__ */
