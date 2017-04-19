#pragma once

#include "Harmony/HyEngine.h"

class ScaledText : public IHyApplication
{
	HyCamera2d *	m_pCamera;

public:
	ScaledText(HarmonyInit &initStruct);
	virtual ~ScaledText();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
