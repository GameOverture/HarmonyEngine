#pragma once

#include "Harmony/HyEngine.h"

class Portals2d : public IHyApplication
{
	HyCamera2d *	m_pCamera;

	HySprite2d		m_Sprite;
	HyText2d		m_Text;

	HyPortal2d *	m_pPortal;

	enum Controls
	{
		MoveUp = 0,
		MoveDown,
		MoveRight,
		MoveLeft
	};

public:
	Portals2d(HarmonyInit &initStruct);
	virtual ~Portals2d();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
