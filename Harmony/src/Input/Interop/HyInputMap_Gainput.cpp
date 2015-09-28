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

HyInputMap_Gainput::HyInputMap_Gainput()
{
}

HyInputMap_Gainput::~HyInputMap_Gainput()
{
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_KB(uint32 iUserId, HyKeyboardBtn eBtn)
{
	//m_pInputMap->MapBool(
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_MO(uint32 iUserId, HyMouseBtn eBtn)
{
}

/*virtual*/ bool HyInputMap_Gainput::MapBtn_GP(uint32 iUserId, HyGamePadBtn eBtn)
{
}

/*virtual*/ bool HyInputMap_Gainput::MapAxis_MO(uint32 iUserId, HyMouseBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
}

/*virtual*/ bool HyInputMap_Gainput::MapAxis_GP(uint32 iUserId, HyGamePadBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
}

/*virtual*/ void HyInputMap_Gainput::Unmap(uint32 iUserId)
{
}

/*virtual*/ bool HyInputMap_Gainput::IsMapped(uint32 iUserId) const
{
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnDown(uint32 iUserId) const
{
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnDownBuffered(uint32 iUserId) const
{
}

/*virtual*/ bool HyInputMap_Gainput::IsBtnReleased(uint32 iUserId) const
{
}

/*virtual*/ float HyInputMap_Gainput::GetAxis(uint32 iUserId) const
{
}

/*virtual*/ float HyInputMap_Gainput::GetAxisDelta(uint32 iUserId) const
{
}
