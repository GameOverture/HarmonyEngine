#pragma once

class BaseGame : public LgReelGame
{
	LgMeter *		m_pMeter;
	HyText2d		m_Text;
	HyPrimitive2d	m_TextBox;

	enum eDebugKeys
	{
		DEBUGKEY_MeterSpin = 89,
		DEBUGKEY_MeterToggleSpin,
		DEBUGKEY_MeterToggleCash,

		DEBUGKEY_MeterBoxSizeVertGrow,
		DEBUGKEY_MeterBoxSizeVertShrink,
		DEBUGKEY_MeterBoxSizeHorzGrow,
		DEBUGKEY_MeterBoxSizeHorzShrink
	};

public:
	BaseGame(LgSlotGame &gameRef, const ILgxGameData *pXmlGameData);
	virtual ~BaseGame();

	virtual void OnUpdate() override;
};

