#pragma once

#include "Harmony/HyEngine.h"
#include "TestGrid.h"

class NewGame : public IHyApplication
{
	HyCamera2d *		m_pCam;
	//HySprite2d			m_TestSprite;
	//HyTexturedQuad2d	m_TestQuad;

	//HyText2d			m_TestText;

	//HyPrimitive2d		m_primBox;
	//HyPrimitive2d		m_HorzLine;
	HyPrimitive2d		m_VertLine;

	//HyAudio2d			m_AudioTest;

	//TestGrid			m_TestGrid;

	//HyPrimitive2d		m_BoxTest;

public:
	NewGame(HarmonyInit &initStruct);
	virtual ~NewGame();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
