#pragma once

#include "Harmony/HyEngine.h"

class HyTemplate : public IHyApplication
{
	HyCamera2d *	m_pCamera;

public:
	HyTemplate(HarmonyInit &initStruct);
	virtual ~HyTemplate();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
