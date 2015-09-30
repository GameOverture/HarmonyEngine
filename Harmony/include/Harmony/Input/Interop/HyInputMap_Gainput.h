/**************************************************************************
*	HyInputMap_Gainput.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyInputMap_Gainput_h__
#define __HyInputMap_Gainput_h__

#include "Input/IHyInputMap.h"
#include "Input/Interop/HyInput_Gainput.h"

#include "gainput/gainput.h"

class HyInputMap_Gainput : public IHyInputMap
{
	friend class HyInput_Gainput;

	gainput::InputMap *			m_pInputMap;

public:
	HyInputMap_Gainput();
	virtual ~HyInputMap_Gainput();

	virtual bool MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn);
	virtual bool MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn);
	virtual bool MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn, uint32 uiGamePadIndex);

	virtual bool MapAxis_MO(uint32 iUserId, HyMouseBtn eAxis, float fMin = 0.0f, float fMax = 1.0f);
	virtual bool MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f);

	virtual void Unmap(uint32 iUserId);
	virtual bool IsMapped(uint32 iUserId) const;

	virtual bool IsBtnDown(uint32 iUserId) const;
	virtual bool IsBtnDownBuffered(uint32 iUserId) const;		// If the button was just pressed
	virtual bool IsBtnReleased(uint32 iUserId) const;

	virtual float GetAxis(uint32 iUserId) const;
	virtual float GetAxisDelta(uint32 iUserId) const;

private:
	virtual void Initialize();
};

#endif /* __HyInputMap_Gainput_h__ */
