#pragma once

#include "Harmony/HyEngine.h"

class %HY_CLASS% : public IHyApplication
{
	HyCamera2d *	m_pCamera;

public:
	%HY_CLASS%(HarmonyInit &initStruct);
	virtual ~%HY_CLASS%();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
