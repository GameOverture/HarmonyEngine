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

#include "Harmony/HyEngine.h"

class TestBed : public IHyApplication
{
	enum eControls
	{
		CAM_LEFT,
		CAM_RIGHT,
		CAM_UP,
		CAM_DOWN,

		SEND_LOG,
		ACTION_1,
		ACTION_2,
		ACTION_3
	};

	HyCamera2d *	m_pCam;
	HyCamera2d *	m_pCam2;

	HyCamera2d *	m_pCam_Viewport2;

	vector<uint32>	m_vGamePadIds;

	HySpine2d			m_spineBoy;
	HyPrimitive2d		m_primBox;
	HyText2d			m_txtName;

	HyPrimitive2d		m_HorzLine;
	HyPrimitive2d		m_VertLine;

	//HyTexturedQuad2d	m_testSprite;


public:
	TestBed(HarmonyInit &initStruct);
	~TestBed(void);

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};

#endif /* __RenderTest_h__ */
