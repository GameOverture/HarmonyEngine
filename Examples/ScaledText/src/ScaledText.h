#pragma once

#include "Harmony/HyEngine.h"

class ScaledText : public IHyApplication
{
	HyCamera2d *	m_pCamera;

	HyText2d		m_Text;
	HyPrimitive2d	m_TextBox;

public:
	ScaledText(HarmonyInit &initStruct);
	virtual ~ScaledText();

	virtual bool Initialize() override;
	virtual bool Update() override;
	virtual void Shutdown() override;
};
