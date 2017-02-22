#pragma once

#include "Harmony/HyEngine.h"

class TextTest : public IHyApplication
{
	HyText2d		m_Text;
	HyPrimitive2d	m_TextBox;
public:
	TextTest(HarmonyInit &initStruct);
	virtual ~TextTest();

	virtual bool Initialize();
	virtual bool Update();
	virtual void Shutdown();
};
