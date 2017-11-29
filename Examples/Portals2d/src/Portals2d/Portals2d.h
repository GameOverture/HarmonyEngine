#pragma once

#include "Harmony/HyEngine.h"

class Portals2d : public IHyApplication
{
	HyCamera2d *	m_pCamera;

	HySprite2d		m_Sprite;

public:
	Portals2d(HarmonyInit &initStruct);
	virtual ~Portals2d();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
