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

HyInputMap_Gainput::HyInputMap_Gainput(IHyInput *pInputManager) : IHyInputMap(pInputManager)
{
	m_pGainputMap = HY_NEW gainput::InputMap(static_cast<HyInput_Gainput *>(m_pInputManager)->GetGainputManager());
}

HyInputMap_Gainput::~HyInputMap_Gainput()
{
	delete m_pGainputMap;
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn)
{
	HyAssert(m_pGainputMap, "HyInputMap_Gainput::MapBtn_KB() used before manager initialized");

	return m_pGainputMap->MapBool(iUserId, static_cast<HyInput_Gainput *>(m_pInputManager)->GetKeyboardDeviceId(), eBtn);
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn)
{
	return m_pGainputMap->MapBool(iUserId, static_cast<HyInput_Gainput *>(m_pInputManager)->GetMouseDeviceId(), eBtn);
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn, uint32 uiGamePadIndex)
{
	return m_pGainputMap->MapBool(iUserId, static_cast<HyInput_Gainput *>(m_pInputManager)->GetGamePadDeviceId(uiGamePadIndex), eBtn);
}

/*virtual*/ bool HyInputMap_Gainput::MapAxis_MO(uint32 iUserId, HyMouseAxis eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
	return m_pGainputMap->MapFloat(iUserId, static_cast<HyInput_Gainput *>(m_pInputManager)->GetMouseDeviceId(), eAxis, fMin, fMax);
}

/*virtual*/ bool HyInputMap_Gainput::MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
	return false;
}

/*virtual*/ void HyInputMap_Gainput::Unmap(uint32 iUserId)
{
}

/*virtual*/ bool HyInputMap_Gainput::IsMapped(uint32 iUserId) const
{
	return m_pGainputMap->IsMapped(iUserId);
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnDown(uint32 iUserId) const
{
	return m_pGainputMap->GetBool(iUserId);
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnDownBuffered(uint32 iUserId) const
{
	return m_pGainputMap->GetBoolIsNew(iUserId);
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnReleased(uint32 iUserId) const
{
	return true;
}

/*virtual*/ float HyInputMap_Gainput::GetAxis(uint32 iUserId) const
{
	return m_pGainputMap->GetFloat(iUserId);
}

/*virtual*/ float HyInputMap_Gainput::GetAxisDelta(uint32 iUserId) const
{
	return 0.0f;
}
