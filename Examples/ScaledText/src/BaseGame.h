#pragma once

#include "LgReelGame.h"

class BaseGame : public LgReelGame
{
public:
	BaseGame(LgSlotGame &gameRef, const ILgxGameData *pXmlGameData);
	virtual ~BaseGame();
};

