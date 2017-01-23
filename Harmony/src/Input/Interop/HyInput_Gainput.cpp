/**************************************************************************
 *	HyInput_Gainput.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Input/Interop/HyInput_Gainput.h"
#include "Input/Interop/HyInputMap_Gainput.h"

HyInput_Gainput::HyInput_Gainput(uint32 uiNumInputMappings) :	IHyInput(uiNumInputMappings),
																m_uiKeyboardId(gainput::InvalidDeviceId),
																m_uiMouseId(gainput::InvalidDeviceId),
																m_eRecordState(RECORD_Off),
																m_uiRecordCount(0)
{
	static_cast<HyInputMap_Gainput *>(m_pInputMaps) = reinterpret_cast<HyInputMap_Gainput *>(HY_NEW unsigned char[sizeof(HyInputMap_Gainput) * m_uiNUM_INPUT_MAPS]);
	HyInputMap_Gainput *pWriteLoc = static_cast<HyInputMap_Gainput *>(m_pInputMaps);
	for(uint32 i = 0; i < m_uiNUM_INPUT_MAPS; ++i, ++pWriteLoc)
		new (pWriteLoc) HyInputMap_Gainput(this);
}

HyInput_Gainput::~HyInput_Gainput()
{
}

gainput::InputManager &HyInput_Gainput::GetGainputManager()
{
	return m_Manager;
}

/*virtual*/ void HyInput_Gainput::Update()
{
	// TODO: pass in m_uiRecordCount and wrap logic around this call
	m_Manager.Update();

	//m_Manager.GetDeviceCountByType(
}

/*virtual*/ void HyInput_Gainput::StartRecording()
{
}

/*virtual*/ void HyInput_Gainput::StopRecording()
{
}

/*virtual*/ void HyInput_Gainput::SerializeRecording()
{
}

/*virtual*/ void HyInput_Gainput::StartPlayback()
{
}

/*virtual*/ void HyInput_Gainput::StopPlayback()
{
}

#ifdef HY_PLATFORM_WINDOWS
void HyInput_Gainput::HandleMsg(glm::ivec2 vResolution, const MSG& msg)
{
	m_Manager.SetDisplaySize(vResolution.x, vResolution.y);
	m_Manager.HandleMessage(msg);
}
#endif

gainput::DeviceId HyInput_Gainput::GetKeyboardDeviceId()
{
	if(m_uiKeyboardId == gainput::InvalidDeviceId)
		m_uiKeyboardId = m_Manager.CreateDevice<gainput::InputDeviceKeyboard>();

	return m_uiKeyboardId;
}

gainput::DeviceId HyInput_Gainput::GetMouseDeviceId()
{
	if(m_uiMouseId == gainput::InvalidDeviceId)
		m_uiMouseId = m_Manager.CreateDevice<gainput::InputDeviceMouse>();
	
	return m_uiMouseId;
}

gainput::DeviceId HyInput_Gainput::GetGamePadDeviceId(uint32 uiIndex)
{
	return -1;
}
