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

#include "Harmony/HyEngine.h"

class Player : public HyEntity2d
{
	//HySpine2d		m_SpineBoy;
	//HySpine2d		m_SpineBoy2;
	//HyPrimitive2d	m_BoxTest;

	//HySprite2d		m_Sprite;

public:
	Player(void);
	virtual ~Player(void);

	void Initialize();
};

#endif /* __Player_h__ */
