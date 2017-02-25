#pragma once

#include "Harmony/HyEngine.h"

class HyTemplate : public IHyApplication
{
public:
	HyTemplate(HarmonyInit &initStruct);
	virtual ~HyTemplate();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
