#pragma once

#include "Harmony/HyEngine.h"
#include "LgSlotAfx.h"
#include "LgMeter.h"

#include "LgSlotGame.h"

class ScaledText : public LgSlotGame
{
	HyCamera2d *	m_pCamera;

	HyText2d		m_Text;
	HyPrimitive2d	m_TextBox;

	LgMeter *		m_pMeter;

	LgTilt							m_Tilt;
	LgTelnet						m_Telnet;

public:
	ScaledText(HarmonyInit &initStruct);
	virtual ~ScaledText();

	virtual void OnConstruct() override;

	virtual ILgSubGame *OnSubGame_Allocate(const ILgxGameData *pXmlGameData) override;
};
