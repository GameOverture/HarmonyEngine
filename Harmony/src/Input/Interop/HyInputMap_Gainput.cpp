/**************************************************************************
*	HyInputMap_Gainput.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Input/Interop/HyInputMap_Gainput.h"

#define HY_GAINPUT_MANAGER (*static_cast<HyInput_Gainput *>(sm_pInputManager))

HyInputMap_Gainput::HyInputMap_Gainput() : m_pInputMap(NULL)
{
}

HyInputMap_Gainput::~HyInputMap_Gainput()
{
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn)
{
	HyAssert(m_pInputMap, "HyInputMap_Gainput::MapBtn_KB() used before manager initialized");

	return m_pInputMap->MapBool(iUserId, HY_GAINPUT_MANAGER.GetKeyboardDeviceId(), eBtn);
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn)
{
	return m_pInputMap->MapBool(iUserId, HY_GAINPUT_MANAGER.GetMouseDeviceId(), eBtn);
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn, uint32 uiGamePadIndex)
{
	return m_pInputMap->MapBool(iUserId, HY_GAINPUT_MANAGER.GetGamePadDeviceId(uiGamePadIndex), eBtn);
}

/*virtual*/ bool HyInputMap_Gainput::MapAxis_MO(uint32 iUserId, HyMouseBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
	return true;
}

/*virtual*/ bool HyInputMap_Gainput::MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
	return true;
}

/*virtual*/ void HyInputMap_Gainput::Unmap(uint32 iUserId)
{
}

/*virtual*/ bool HyInputMap_Gainput::IsMapped(uint32 iUserId) const
{
	return true;
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnDown(uint32 iUserId) const
{
	return m_pInputMap->GetBool(iUserId);
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnDownBuffered(uint32 iUserId) const
{
	return m_pInputMap->GetBoolIsNew(iUserId);
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnReleased(uint32 iUserId) const
{
	return true;
}

/*virtual*/ float HyInputMap_Gainput::GetAxis(uint32 iUserId) const
{
	return 0.0f;
}

/*virtual*/ float HyInputMap_Gainput::GetAxisDelta(uint32 iUserId) const
{
	return 0.0f;
}

/*virtual*/ void HyInputMap_Gainput::Initialize()
{
	m_pInputMap = HY_NEW gainput::InputMap(HY_GAINPUT_MANAGER.GetGainputManager());
}

