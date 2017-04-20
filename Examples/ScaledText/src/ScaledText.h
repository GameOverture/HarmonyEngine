#pragma once

#include "Harmony/HyEngine.h"

class ScaledText : public IHyApplication
{
	HyCamera2d *	m_pCamera;

	HyText2d		m_Text;

public:
	ScaledText(HarmonyInit &initStruct);
	virtual ~ScaledText();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
