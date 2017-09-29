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

int32 HyInputMap::MapBtn(int32 iActionId, HyKeyboardBtn eBtn)
{
	auto iter = m_ActionIndexMap.find(iActionId);
	if(iter != m_ActionIndexMap.end())
	{
		ActionInfo &actionRef = m_ActionList[iter->second];

		// Don't allow for both iBtn and iBtnAlternative to have same value
		if(actionRef.iBtnAlternative == eBtn)
			actionRef.iBtnAlternative = HYKEY_Unknown;

		actionRef.iBtn = eBtn;
	}
	else
	{
		m_ActionList.emplace_back(iActionId);
		m_ActionList.back().iBtn = eBtn;
		
		m_ActionIndexMap[iActionId] = static_cast<uint32>(m_ActionList.size() - 1);
	}

	// Determine if eBtn was already used for a different action. If so, remove that button,
	// and return the other action id it was assigned to.
	for(uint32 i = 0; i < m_ActionList.size(); ++i)
	{
		if(m_ActionList[i].iID == iActionId)
			continue;

		if(m_ActionList[i].iBtn == eBtn)
		{
			m_ActionList[i].iBtn = HYKEY_Unknown;
			return m_ActionList[i].iID;
		}
		else if(m_ActionList[i].iBtnAlternative == eBtn)
		{
			m_ActionList[i].iBtnAlternative = HYKEY_Unknown;
			return m_ActionList[i].iID;
		}
	}

	return -1;
}

int32 HyInputMap::MapBtn(int32 iActionId, HyMouseBtn eBtn)
{
	return MapBtn(iActionId, static_cast<HyKeyboardBtn>(eBtn));
}

int32 HyInputMap::MapAlternativeBtn(int32 iActionId, HyKeyboardBtn eBtn)
{
	auto iter = m_ActionIndexMap.find(iActionId);
	if(iter != m_ActionIndexMap.end())
	{
		ActionInfo &actionRef = m_ActionList[iter->second];

		// Don't allow for both iBtn and iBtnAlternative to have same value
		if(actionRef.iBtn == eBtn)
			actionRef.iBtn = HYKEY_Unknown;

		actionRef.iBtnAlternative = eBtn;
	}
	else
	{
		m_ActionList.emplace_back(iActionId);
		m_ActionList.back().iBtnAlternative = eBtn;

		m_ActionIndexMap[iActionId] = static_cast<uint32>(m_ActionList.size() - 1);
	}

	// Determine if eBtn was already used for a different action. If so, remove that button,
	// and return the other action id it was assigned to.
	for(uint32 i = 0; i < m_ActionList.size(); ++i)
	{
		if(m_ActionList[i].iID == iActionId)
			continue;

		if(m_ActionList[i].iBtn == eBtn)
		{
			m_ActionList[i].iBtn = HYKEY_Unknown;
			return m_ActionList[i].iID;
		}
		else if(m_ActionList[i].iBtnAlternative == eBtn)
		{
			m_ActionList[i].iBtnAlternative = HYKEY_Unknown;
			return m_ActionList[i].iID;
		}
	}

	return -1;
}

int32 HyInputMap::MapAlternativeBtn(int32 iActionId, HyMouseBtn eBtn)
{
	return MapAlternativeBtn(iActionId, static_cast<HyKeyboardBtn>(eBtn));
}

bool HyInputMap::MapJoystickBtn(int32 iActionId, HyGamePadBtn eBtn, uint32 uiJoystickIndex)
{
	return false;
}

bool HyInputMap::MapAxis_GP(int32 iUserId, HyGamePadBtn eAxis, float fMin /*= 0.0f*/, float fMax /*= 1.0f*/)
{
	return false;
}

bool HyInputMap::Unmap(int32 iUserId)
{
	// TODO:
	HyError("HyInputMap::Unmap not implemented");
	return false;
}

bool HyInputMap::IsMapped(int32 iUserId) const
{
	// TODO:
	HyError("HyInputMap::IsMapped not implemented");
	return false;
}

bool HyInputMap::IsActionDown(int32 iActionId) const
{
	auto iter = m_ActionIndexMap.find(iActionId);
	if(iter == m_ActionIndexMap.end())
		return false;

	return (m_ActionList[iter->second].uiFlags & ActionInfo::FLAG_Pressed) != 0;
}

bool HyInputMap::IsActionReleased(int32 iActionId) const
{
	auto iter = m_ActionIndexMap.find(iActionId);
	if(iter == m_ActionIndexMap.end())
		return false;

	return (m_ActionList[iter->second].uiFlags & ActionInfo::FLAG_IsReleased) != 0;
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
	for(uint32 i = 0; i < static_cast<uint32>(m_ActionList.size()); ++i)
	{
		if(m_ActionList[i].uiFlags & ActionInfo::FLAG_WasReleased)
		{
			m_ActionList[i].uiFlags |= ActionInfo::FLAG_IsReleased;
			m_ActionList[i].uiFlags &= ~ActionInfo::FLAG_WasReleased;
		}
		else
			m_ActionList[i].uiFlags &= ~ActionInfo::FLAG_IsReleased;
	}
}

void HyInputMap::ApplyInput(int32 iKey, HyBtnPressState ePressState)
{
	for(uint32 i = 0; i < static_cast<uint32>(m_ActionList.size()); ++i)
	{
		if(m_ActionList[i].iBtn == iKey || m_ActionList[i].iBtnAlternative == iKey)
		{
			if(ePressState == HYBTN_Press)
				m_ActionList[i].uiFlags |= ActionInfo::FLAG_Pressed;
			if(ePressState == HYBTN_Release)
			{
				m_ActionList[i].uiFlags &= ~ActionInfo::FLAG_Pressed;
				m_ActionList[i].uiFlags |= ActionInfo::FLAG_WasReleased;
			}
		}
	}
}
