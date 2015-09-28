/**************************************************************************
 *	TestBed.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __RenderTest_h__
#define __RenderTest_h__

#include "stdafx.h"

#include "Player.h"

class TestBed : public IHyApplication
{
	HyCamera2d *	m_pCam;
	vector<uint32>	m_vGamePadIds;

	Player			m_Player;

public:
	TestBed(HarmonyInit &initStruct);
	~TestBed(void);

	virtual bool Initialize();
	virtual bool Update();
	virtual bool Shutdown();
};

#endif /* __RenderTest_h__ */
