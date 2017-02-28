#pragma once

#include "Harmony/HyEngine.h"

class Text : public IHyApplication
{
	HyCamera2d *	m_pCamera;

	HyText2d		m_Text;
	HyText2d		m_Text2;
	HyPrimitive2d	m_TextBox;
	HyPrimitive2d	m_TextBox2;

	HyPrimitive2d	m_Origin;

public:
	Text(HarmonyInit &initStruct);
	virtual ~Text();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
