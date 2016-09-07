#pragma once

#include "Harmony/HyEngine.h"

class NewGame : public IHyApplication
{
public:
	NewGame(HarmonyInit &initStruct);
	virtual ~NewGame();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
