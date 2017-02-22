#pragma once

#include "Harmony/HyEngine.h"

class TextTest : public IHyApplication
{
public:
	TextTest(HarmonyInit &initStruct);
	virtual ~TextTest();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
