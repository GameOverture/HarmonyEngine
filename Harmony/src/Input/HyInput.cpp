/**************************************************************************
 *	HyInput.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Input/HyInput.h"
//#include "Gamepad/Gamepad.h"

HyInput::HyInput(uint32 uiNumInputMaps, HyInputMapping *pInputMapping) :	m_fDeadZoneAmt(0.2f),
																			m_uiNumInputMappings(uiNumInputMaps),
																			m_pInputMappings(pInputMapping)
{
	HyAssert(m_uiNumInputMappings > 0, "HyInput uiNumInputMaps is <= 0");

	m_idKB = m_Manager.CreateDevice<gainput::InputDeviceKeyboard>();
	m_idMouse = m_Manager.CreateDevice<gainput::InputDeviceMouse>();
	//m_idGamePad = m_Manager.CreateDevice<gainput::InputDevicePad>();
	m_idTouch = m_Manager.CreateDevice<gainput::InputDeviceTouch>();

	for(uint32 i = 0; i < m_uiNumInputMappings; ++i)
		m_pInputMappings[i].SetHyInputPtr(this);

//#if defined(HY_PLATFORM_WINDOWS) || defined(HY_PLATFORM_OSX) || defined(HY_PLATFORM_LINUX)
//	Gamepad_deviceAttachFunc(OnGamepadAttached, this);
//	Gamepad_deviceRemoveFunc(OnGamepadRemoved, this);
//	Gamepad_buttonDownFunc(OnButtonDown, this);
//	Gamepad_buttonUpFunc(OnButtonUp, this);
//	Gamepad_axisMoveFunc(OnAxisMove, this);
//
//	Gamepad_init();
//	Gamepad_detectDevices();
//#endif
}

HyInput::~HyInput(void)
{
}

gainput::InputManager &HyInput::GetManager()
{
	return m_Manager;
}

void HyInput::GiveDeviceIds(gainput::DeviceId &idKBOut, gainput::DeviceId &idMouseOut, gainput::DeviceId &idGamePadOut, gainput::DeviceId &idTouchOut)
{
	idKBOut = m_idKB;
	idMouseOut = m_idMouse;
	idGamePadOut = m_idGamePad;
	idTouchOut = m_idTouch;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HyInput::ProcessInputs()
{
//#if defined(HY_PLATFORM_WINDOWS) || defined(HY_PLATFORM_OSX) || defined(HY_PLATFORM_LINUX)
//	// Get gamepad input
//	Gamepad_processEvents();
//
//#if !defined(HY_PLATFORM_GUI)
//	// Get mouse input
//	// Get keyboard input
//	HyGlfwInput::GetKeyboardState(m_ui64KeyFlags);
//#endif
//
//	// TODO: Call intermittently
//	//Gamepad_detectDevices()
//#endif

	// TODO: This needs to be 
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		m_Manager.HandleMessage(msg);
	}

	m_Manager.Update();
		
	//for(uint32 i = 0; i < m_uiNumInputMappings; ++i)
	//{
	//	tInputState &convertedInputStateRef = m_pInputMappings[i].m_tCurConvertedInput;
	//	m_pInputMappings[i].m_uiPrevConvertedBtnFlags = convertedInputStateRef.uiButtonFlags;
	//	
	//	vector<std::pair<eActionButton, HyInputKey> > &vBtnMappings = m_pInputMappings[i].m_vBtnMappings;
	//	convertedInputStateRef.uiButtonFlags = 0;
	//	size_t uiNumBtnMappings = vBtnMappings.size();
	//	for(size_t j = 0; j < uiNumBtnMappings; ++j)
	//	{
	//		switch(vBtnMappings[j].second.m_eType)
	//		{
	//		case HyInputKey::InputType_GP:
	//			if(m_mapGamePads[vBtnMappings[j].second.m_iAux].uiButtonFlags & (1 << vBtnMappings[j].second.m_uiKey))
	//				convertedInputStateRef.uiButtonFlags |= (1 << vBtnMappings[j].first);
	//			break;
	//		case HyInputKey::InputType_AXIS:
	//			if(m_mapGamePads[vBtnMappings[j].second.m_iAux].pAxisStates[vBtnMappings[j].second.m_uiKey] != 0.0f)
	//				convertedInputStateRef.uiButtonFlags |= (1 << vBtnMappings[j].first);
	//			break;
	//		case HyInputKey::InputType_KB:
	//			if(m_ui64KeyFlags[vBtnMappings[j].second.m_iAux] & static_cast<uint64>(1 << vBtnMappings[j].second.m_uiKey))
	//				convertedInputStateRef.uiButtonFlags |= (1 << vBtnMappings[j].first);
	//			break;
	//		case HyInputKey::InputType_MO:
	//			break;
	//		}
	//	}

	//	vector<std::pair<eActionAxis, HyInputKey> > &vAxisMappings = m_pInputMappings[i].m_vAxisMappings;
	//	size_t uiNumAxisMappings = vAxisMappings.size();
	//	
	//	for(uint32 j = 0; j < Action_MaxAxes; ++j)
	//		convertedInputStateRef.pAxisStates[j] = 0.0f;

	//	for(size_t j = 0; j < uiNumAxisMappings; ++j)
	//	{
	//		switch(vAxisMappings[j].second.m_eType)
	//		{
	//		case HyInputKey::InputType_GP:
	//			if(m_mapGamePads[vAxisMappings[j].second.m_iAux].uiButtonFlags & (1 << vAxisMappings[j].second.m_uiKey))
	//				convertedInputStateRef.pAxisStates[vAxisMappings[j].first] = (vAxisMappings[j].second.m_bPositiveAxis ? 1.0f : -1.0f);
	//			break;
	//		case HyInputKey::InputType_AXIS:
	//			if(m_mapGamePads[vAxisMappings[j].second.m_iAux].pAxisStates[vAxisMappings[j].second.m_uiKey] != 0.0f)
	//				convertedInputStateRef.pAxisStates[vAxisMappings[j].first] = m_mapGamePads[vAxisMappings[j].second.m_iAux].pAxisStates[vAxisMappings[j].second.m_uiKey];
	//			break;
	//		case HyInputKey::InputType_KB:
	//			if(m_ui64KeyFlags[vAxisMappings[j].second.m_iAux] & static_cast<uint64>(1 << vAxisMappings[j].second.m_uiKey))
	//				convertedInputStateRef.pAxisStates[vAxisMappings[j].first] = (vAxisMappings[j].second.m_bPositiveAxis ? 1.0f : -1.0f);
	//			break;
	//		case HyInputKey::InputType_MO:
	//			break;
	//		}
	//	}
	//}
}

//void HyInput::SaveInputs(uint32 uiUpdateIndex)
//{
//
//}
//
//void HyInput::ApplyInputs(uint32 uiUpdatesIndex)
//{
//
//}
//
//#if defined(HY_PLATFORM_WINDOWS) || defined(HY_PLATFORM_OSX) || defined(HY_PLATFORM_LINUX)
//	/*static*/ void HyInput::OnGamepadAttached(struct Gamepad_device * device, void *context)
//	{
//		HyInput *pThis = reinterpret_cast<HyInput *>(context);
//
//		pThis->m_mapGamePads[device->deviceID].uiButtonFlags = 0;
//		memset(pThis->m_mapGamePads[device->deviceID].pAxisStates, 0, 10 * sizeof(float));
//	}
//
//	/*static*/ void HyInput::OnGamepadRemoved(struct Gamepad_device * device, void * context)
//	{
//		reinterpret_cast<HyInput *>(context)->m_mapGamePads.erase(reinterpret_cast<HyInput *>(context)->m_mapGamePads.find(device->deviceID));
//	}
//
//	/*static*/ void HyInput::OnButtonDown(struct Gamepad_device * device, unsigned int buttonID, void * context)
//	{
//		HyAssert(buttonID < Action_MaxBtns, "HyInput::OnButtonDown() recieved a buttonID >= 32");
//		reinterpret_cast<HyInput *>(context)->m_mapGamePads[device->deviceID].uiButtonFlags |= 1 << buttonID;
//	}
//
//	/*static*/ void HyInput::OnButtonUp(struct Gamepad_device * device, unsigned int buttonID, void * context)
//	{
//		HyAssert(buttonID < Action_MaxBtns, "HyInput::OnButtonUp() recieved a buttonID >= 32");
//		reinterpret_cast<HyInput *>(context)->m_mapGamePads[device->deviceID].uiButtonFlags &= ~(1 << buttonID);
//	}
//
//	/*static*/ void HyInput::OnAxisMove(struct Gamepad_device * device, unsigned int axisID, float value, float lastValue, void * context)
//	{
//		HyAssert(axisID < Action_MaxAxes, "HyInput::OnAxisMove9) recieved an axisID >= 10");
//		reinterpret_cast<HyInput *>(context)->m_mapGamePads[device->deviceID].pAxisStates[axisID] = (abs(value) < reinterpret_cast<HyInput *>(context)->m_fDeadZoneAmt) ? 0.0f : value;
//	}
//#endif
