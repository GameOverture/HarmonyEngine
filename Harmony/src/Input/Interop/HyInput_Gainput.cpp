/**************************************************************************
 *	HyInput_Gainput.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Input/Interop/HyInput_Gainput.h"
#include "Input/Interop/HyInputMap_Gainput.h"
#include "Renderer/Components/HyWindow.h"
#include "Diagnostics/Console/HyConsole.h"

#include <queue>

HyInput_Gainput::HyInput_Gainput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef) :	IHyInput(uiNumInputMappings, windowListRef),
																										m_uiKeyboardId(gainput::InvalidDeviceId),
																										m_uiMouseId(gainput::InvalidDeviceId),
																										m_eRecordState(RECORD_Off),
																										m_uiRecordCount(0)
{
	if(uiNumInputMappings > 0)
	{
		static_cast<HyInputMap_Gainput *>(m_pInputMaps)[0].MapAxis_MO(MOUSEID_X, HYMOUSE_AxisX);
		static_cast<HyInputMap_Gainput *>(m_pInputMaps)[0].MapAxis_MO(MOUSEID_Y, HYMOUSE_AxisY);

		static_cast<HyInputMap_Gainput *>(m_pInputMaps)[0].MapBtn_MO(MOUSEID_Left, HYMOUSE_ButtonLeft);
		static_cast<HyInputMap_Gainput *>(m_pInputMaps)[0].MapBtn_MO(MOUSEID_Right, HYMOUSE_ButtonRight);
	}
}

HyInput_Gainput::~HyInput_Gainput()
{
}

gainput::InputManager &HyInput_Gainput::GetGainputManager()
{
	return m_Manager;
}

/*virtual*/ bool HyInput_Gainput::IsMouseLeftDown() /*override*/
{
	return m_pInputMaps[0].IsBtnDown(MOUSEID_Left) || m_pInputMaps[0].IsBtnReleased(MOUSEID_Left) || m_pInputMaps[0].IsBtnDownBuffered(MOUSEID_Left);
}

/*virtual*/ bool HyInput_Gainput::IsMouseRightDown() /*override*/
{
	return m_pInputMaps[0].IsBtnDown(MOUSEID_Right) || m_pInputMaps[0].IsBtnReleased(MOUSEID_Right) || m_pInputMaps[0].IsBtnDownBuffered(MOUSEID_Right);
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

/*virtual*/ void HyInput_Gainput::SetWindowIndex(uint32 uiCurrentWindowIndex) /*override*/
{
	m_uiMouse_WindowIndex = uiCurrentWindowIndex;
	m_Manager.SetDisplaySize(static_cast<int>(m_WindowListRef[m_uiMouse_WindowIndex]->GetResolution().x),
							 static_cast<int>(m_WindowListRef[m_uiMouse_WindowIndex]->GetResolution().y));
}

/*virtual*/ void HyInput_Gainput::HandleMsg(void *pMsg) /*override*/
{
#ifdef HY_PLATFORM_WINDOWS
	const MSG &msgRef = *reinterpret_cast<HyApiMsgInterop *>(pMsg);
	m_Manager.HandleMessage(msgRef);
#endif
}

/*virtual*/ void HyInput_Gainput::Update() /*override*/
{
	// TODO: pass in m_uiRecordCount and wrap logic around this call
	m_Manager.Update();

	// TODO: Don't hardcode '0'
	m_ptMouse_CurNormalizedPos.x = m_pInputMaps[0].GetAxis(MOUSEID_X);
	m_ptMouse_CurNormalizedPos.y = 1.0f - m_pInputMaps[0].GetAxis(MOUSEID_Y); // Invert Y-coordinate
}
