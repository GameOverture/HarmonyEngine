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

#include <vector>
#include <map>
using std::vector;
using std::map;

class HyInput
{
	friend class ITimeApi;

	HyInputMapping *				m_pInputMappings;
	uint32							m_uiNumInputMappings;

	float							m_fDeadZoneAmt;
	map<uint32, tInputState>		m_mapGamePads;

	uint64							m_ui64KeyFlags[HYINPUT_KEYBOARD_CODE_NUMFLAGS];		// Offset of '32', Offset of '256', Offset of '320'

public:
	HyInput(uint32 uiNumInputMaps, HyInputMapping *pInputMapping);
	virtual ~HyInput(void);

	map<uint32, tInputState> &		GetGamePadMapRef()	{ return m_mapGamePads; }

	void ProcessInputs();

	void SaveInputs(uint32 uiUpdateIndex);
	void ApplyInputs(uint32 uiUpdateIndex);
	static void OnGamepadAttached(struct Gamepad_device * device, void * context);
	static void OnGamepadRemoved(struct Gamepad_device * device, void * context);

	static void OnButtonDown(struct Gamepad_device * device, unsigned int buttonID, void * context);
	static void OnButtonUp(struct Gamepad_device * device, unsigned int buttonID, void * context);
	static void OnAxisMove(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, void * context);
};

#endif /* __HyInput_h__ */
