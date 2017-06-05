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
#include "Renderer/Components/HyGfxComms.h"
#include "Renderer/Components/HyWindow.h"
#include "Diagnostics/Console/HyConsole.h"

#include <queue>

HyInput_Gainput::HyInput_Gainput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef, HyGfxComms &gfxCommsRef) :	IHyInput(uiNumInputMappings, windowListRef, gfxCommsRef),
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

/*virtual*/ void HyInput_Gainput::Update()
{
	// TODO: pass in m_uiRecordCount and wrap logic around this call
	m_Manager.Update();

	std::queue<HyApiMsgInterop> apiMsgQueue;
	m_GfxCommsRef.RxApiMsgs(apiMsgQueue);

	while(apiMsgQueue.empty() == false)
	{
		HyApiMsgInterop msg = apiMsgQueue.front();
		apiMsgQueue.pop();

#ifdef HY_PLATFORM_WINDOWS
		m_Manager.SetDisplaySize(static_cast<int>(m_WindowListRef[0]->GetResolution().x), static_cast<int>(m_WindowListRef[0]->GetResolution().y));
		m_Manager.HandleMessage(msg);
#endif
	}

	// TODO: Don't hardcode '0'
	//if(msg.message >= WM_MOUSEFIRST && msg.message <= WM_MOUSELAST)
		{
			glm::vec2 ptMouseAxisNormalized(m_pInputMaps[0].GetAxis(MOUSEID_X), m_pInputMaps[0].GetAxis(MOUSEID_Y));
			ptMouseAxisNormalized.y = 1.0f - ptMouseAxisNormalized.y; // Invert Y-coordinate

			m_ptLocalMousePos = ptMouseAxisNormalized;
			m_uiMouseWindowIndex = 0;

			//if(msg.message == WM_MOUSEMOVE) {
			//	HyLog("MOUSE MOVE: (" << GetWorldMousePos().x << ", " << GetWorldMousePos().y << ")");
			//}
		}

	m_bMouseLeftDown = m_pInputMaps[0].IsBtnDown(MOUSEID_Left);
	m_bMouseRightDown = m_pInputMaps[0].IsBtnDown(MOUSEID_Right);

	//m_Manager.GetDeviceCountByType(
}
