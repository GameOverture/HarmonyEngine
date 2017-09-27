/**************************************************************************
 *	HyInputMap.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyInteropAfx.h"
#include "Input/HyInputMap.h"
#include "Input/HyInput.h"
#include "Renderer/Components/HyWindow.h"

HyInputMap::HyInputMap()
{
}

HyInputMap::~HyInputMap(void)
{
}

bool HyInputMap::MapBtn(int32 iUserId, HyKeyboardBtn eBtn)
{
	if(m_ButtonMap.find(iUserId) != m_ButtonMap.end())
		return false;

	m_ButtonMap[iUserId] = ButtonInfo(static_cast<int32>(eBtn), ButtonInfo::TYPE_KB);
	return true;
}

bool HyInputMap::MapBtn(int32 iUserId, HyMouseBtn eBtn)
{
	if(m_ButtonMap.find(iUserId) != m_ButtonMap.end())
		return false;

	m_ButtonMap[iUserId] = ButtonInfo(static_cast<int32>(eBtn), ButtonInfo::TYPE_MO);
	return true;
}

bool HyInputMap::MapBtn(int32 iUserId, HyGamePadBtn eBtn)
{
	if(m_ButtonMap.find(iUserId) != m_ButtonMap.end())
		return false;

	m_ButtonMap[iUserId] = ButtonInfo(static_cast<int32>(eBtn), ButtonInfo::TYPE_JOY);
	return true;
}

bool HyInputMap::MapAxis_GP(int32 iUserId, HyGamePadBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
	return false;
}

bool HyInputMap::Unmap(int32 iUserId)
{
	std::map<int32, ButtonInfo>::iterator iter = m_ButtonMap.find(iUserId);
	if(iter == m_ButtonMap.end())
		return false;

	m_ButtonMap.erase(iter);
	return true;
}

bool HyInputMap::IsMapped(int32 iUserId) const
{
	return (m_ButtonMap.find(iUserId) != m_ButtonMap.end());
}

bool HyInputMap::IsBtnDown(int32 iUserId) const
{
	std::map<int32, ButtonInfo>::const_iterator iter = m_ButtonMap.find(iUserId);
	if(iter == m_ButtonMap.end())
		return false;

	return iter->second.bCurrent;
}

bool HyInputMap::IsBtnReleased(int32 iUserId) const
{
	std::map<int32, ButtonInfo>::const_iterator iter = m_ButtonMap.find(iUserId);
	if(iter == m_ButtonMap.end())
		return false;

	return iter->second.bPrevious == true && iter->second.bCurrent == false;
}

float HyInputMap::GetAxis(int32 iUserId) const
{
	return 0.0f;
}

float HyInputMap::GetAxisDelta(int32 iUserId) const
{
	return 0.0f;
}

void HyInputMap::Update()
{
}
