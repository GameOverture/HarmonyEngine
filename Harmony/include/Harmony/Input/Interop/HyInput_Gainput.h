/**************************************************************************
 *	HyInput_Gainput.h
 *
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyInput_Gainput_h__
#define __HyInput_Gainput_h__

#include "Input/IHyInput.h"

#include "gainput/gainput.h"

class HyWindow;

class HyInput_Gainput : public IHyInput
{
	gainput::InputManager			m_Manager;

	enum eMouseAxisId
	{
		MOUSEID_X = 0xFFFF,
		MOUSEID_Y,
		MOUSEID_Left,
		MOUSEID_Right
	};

	gainput::DeviceId				m_uiKeyboardId;
	gainput::DeviceId				m_uiMouseId;
	std::vector<gainput::DeviceId>	m_GamePadIdList;

	enum eRecordState
	{
		RECORD_Off = 0,
		RECORD_Saving,
		RECORD_Replaying
	};
	eRecordState					m_eRecordState;
	uint64							m_uiRecordCount;


public:
	HyInput_Gainput(uint32 uiNumInputMappings, std::vector<HyWindow *> &windowListRef);
	~HyInput_Gainput();

	gainput::InputManager &GetGainputManager();

	virtual void StartRecording() override;
	virtual void StopRecording() override;
	virtual void SerializeRecording() override;

	virtual void StartPlayback() override;
	virtual void StopPlayback() override;

	gainput::DeviceId GetKeyboardDeviceId();

	gainput::DeviceId GetMouseDeviceId();

	gainput::DeviceId GetGamePadDeviceId(uint32 uiIndex);

	virtual void SetWindowIndex(uint32 uiCurrentWindowIndex) override;
	virtual void HandleMsg(void *pMsg) override;

	virtual void Update() override;
};

#endif /* __HyInput_Gainput_h__ */
