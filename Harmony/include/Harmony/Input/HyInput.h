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

class HyInput
{
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
	HyInput(HyInputMapping *pInputMaps, uint32 uiNumInputMaps);
	virtual ~HyInput();

	static void StartRecording();
	static void PlayRecording();

	void Update();
};

#endif /* __HyInput_h__ */
