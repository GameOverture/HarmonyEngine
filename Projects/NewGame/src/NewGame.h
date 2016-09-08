#pragma once

#include "Harmony/HyEngine.h"

class NewGame : public IHyApplication
{
	HyCamera2d *		m_pCam;
	HySprite2d			m_TestSprite;
	HyTexturedQuad2d	m_TestQuad;

	HyPrimitive2d		m_primBox;
	HyPrimitive2d		m_HorzLine;
	HyPrimitive2d		m_VertLine;

public:
	NewGame(HarmonyInit &initStruct);
	virtual ~NewGame();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
