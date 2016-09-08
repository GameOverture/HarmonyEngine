#pragma once

#include "Harmony/HyEngine.h"

class NewGame : public IHyApplication
{
	HySprite2d m_Wild;

public:
	NewGame(HarmonyInit &initStruct);
	virtual ~NewGame();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
