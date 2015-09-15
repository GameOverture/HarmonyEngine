/**************************************************************************
 *	HyInput.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyInput_h__
#define __HyInput_h__

#include "Afx/HyStdAfx.h"

#include "Mappings/HyInputMapping.h"
#include "gainput/gainput.h"

//#include <vector>
//#include <map>
//using std::vector;
//using std::map;

class HyInput
{
	friend class ITimeApi;

	gainput::InputManager			m_Manager;

	gainput::DeviceId				m_idKB;
	gainput::DeviceId				m_idMouse;
	gainput::DeviceId				m_idGamePad;
	gainput::DeviceId				m_idTouch;

	enum eReplayState
	{
		REPLAY_Off = 0,
		REPLAY_Saving,
		REPLAY_Replaying
	};
	eReplayState					m_eReplayState;
	uint64							m_uiRecordCount;


	HyInputMapping *				m_pInputMappings;
	uint32							m_uiNumInputMappings;

	float							m_fDeadZoneAmt;

public:
	HyInput(uint32 uiNumInputMaps, HyInputMapping *pInputMapping);
	virtual ~HyInput(void);

	gainput::InputManager &GetManager();

	void GiveDeviceIds(gainput::DeviceId &idKBOut, gainput::DeviceId &idMouseOut, gainput::DeviceId &idGamePadOut, gainput::DeviceId &idTouchOut);

	//map<uint32, tInputState> &		GetGamePadMapRef()	{ return m_mapGamePads; }

	void Update();

	//void SaveInputs(uint32 uiUpdateIndex);
	//void ApplyInputs(uint32 uiUpdateIndex);
	//static void OnGamepadAttached(struct Gamepad_device * device, void * context);
	//static void OnGamepadRemoved(struct Gamepad_device * device, void * context);

	//static void OnButtonDown(struct Gamepad_device * device, unsigned int buttonID, void * context);
	//static void OnButtonUp(struct Gamepad_device * device, unsigned int buttonID, void * context);
	//static void OnAxisMove(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, void * context);
};

#endif /* __HyInput_h__ */
