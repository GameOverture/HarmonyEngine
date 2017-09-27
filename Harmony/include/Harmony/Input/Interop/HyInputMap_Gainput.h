///**************************************************************************
//*	HyInputMap_Gainput.h
//*
//*	Harmony Engine
//*	Copyright (c) 2015 Jason Knobler
//*
//*	The zlib License (zlib)
//*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
//*************************************************************************/
//#ifndef __HyInputMap_Gainput_h__
//#define __HyInputMap_Gainput_h__
//
//#include "Afx/HyStdAfx.h"
//#include "Input/HyInputMap.h"
//#include "Input/Interop/HyInput_Gainput.h"
//
//#include "gainput/gainput.h"
//
//class HyInputMap_Gainput : public HyInputMap
//{
//	friend class HyInput_Gainput;
//
//	gainput::InputMap *			m_pGainputMap;
//
//	uint32						m_uiMouseX;
//	uint32						m_uiMouseY;
//
//public:
//	HyInputMap_Gainput(HyInput *pInputManager);
//	virtual ~HyInputMap_Gainput();
//
//	virtual bool MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn) override;
//	virtual bool MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn) override;
//	virtual bool MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn, uint32 uiGamePadIndex) override;
//
//	virtual bool MapAxis_MO(uint32 iUserId, HyMouseAxis eAxis, float fMin = 0.0f, float fMax = 1.0f) override;
//	virtual bool MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin = 0.0f, float fMax = 1.0f) override;
//
//	virtual void Unmap(uint32 iUserId) override;
//	virtual bool IsMapped(uint32 iUserId) const override;
//
//	virtual bool IsBtnDown(uint32 iUserId) const override;
//	virtual bool IsBtnDownBuffered(uint32 iUserId) const override;		// If the button was just pressed
//	virtual bool IsBtnReleased(uint32 iUserId) const override;
//
//	virtual float GetAxis(uint32 iUserId) const override;
//	virtual float GetAxisDelta(uint32 iUserId) const override;
//};
//
//#endif /* __HyInputMap_Gainput_h__ */
