/**************************************************************************
 *	HyInputMap.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2015 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Afx/HyInteropAfx.h"
#include "Input/HyInputMap.h"
#include "Input/HyInput.h"
#include "Input/IHyController.h"
#include "Window/HyWindow.h"

HyInputMap::HyInputMap() :
	m_pGamePad(nullptr)
{
}

HyInputMap::~HyInputMap(void)
{
}

void HyInputMap::SetActionCategory(int32 iActionId, uint8 uiCategory)
{
	auto iter = m_ActionIndexMap.find(iActionId);
	if(iter != m_ActionIndexMap.end())
	{
		ActionInfo &actionRef = m_ActionList[iter->second];

		actionRef.uiFlags &= ~ActionInfo::FLAG_CategoryBitMask;
		actionRef.uiFlags |= (uiCategory << ActionInfo::FLAG_CategoryShiftAmt);
	}
	else
	{
		m_ActionList.emplace_back(iActionId);
		m_ActionList.back().uiFlags &= ~ActionInfo::FLAG_CategoryBitMask;
		m_ActionList.back().uiFlags |= (uiCategory << ActionInfo::FLAG_CategoryShiftAmt);

		m_ActionIndexMap[iActionId] = static_cast<uint32>(m_ActionList.size() - 1);
	}
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

	// Determine if eBtn was already used for a different action in this same action category. If so, remove that button,
	// and return the other action id it was assigned to.
	ActionInfo &actionRef = (iter != m_ActionIndexMap.end()) ? m_ActionList[iter->second] : m_ActionList.back();
	for(uint32 i = 0; i < m_ActionList.size(); ++i)
	{
		if(m_ActionList[i].iID == iActionId ||
		  (m_ActionList[i].uiFlags & ActionInfo::FLAG_CategoryBitMask) != (actionRef.uiFlags & ActionInfo::FLAG_CategoryBitMask))
		{
			continue;
		}

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

	// Determine if eBtn was already used for a different action in this same action category. If so, remove that button,
	// and return the other action id it was assigned to.
	ActionInfo &actionRef = (iter != m_ActionIndexMap.end()) ? m_ActionList[iter->second] : m_ActionList.back();
	for(uint32 i = 0; i < m_ActionList.size(); ++i)
	{
		if(m_ActionList[i].iID == iActionId ||
			(m_ActionList[i].uiFlags & ActionInfo::FLAG_CategoryBitMask) != (actionRef.uiFlags & ActionInfo::FLAG_CategoryBitMask))
		{
			continue;
		}

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

int32 HyInputMap::MapPadBtn(int32 iActionId, HyGamePadBtn eBtn)
{
	auto iter = m_ActionIndexMap.find(iActionId);
	if(iter != m_ActionIndexMap.end())
	{
		ActionInfo &actionRef = m_ActionList[iter->second];
		actionRef.ePadBtn = eBtn;
	}
	else
	{
		m_ActionList.emplace_back(iActionId);
		m_ActionList.back().ePadBtn = eBtn;

		m_ActionIndexMap[iActionId] = static_cast<uint32>(m_ActionList.size() - 1);
	}

	// Determine if eBtn was already used for a different action in this same action category. If so, remove that button,
	// and return the other action id it was assigned to.
	ActionInfo &actionRef = (iter != m_ActionIndexMap.end()) ? m_ActionList[iter->second] : m_ActionList.back();
	for(uint32 i = 0; i < m_ActionList.size(); ++i)
	{
		if(m_ActionList[i].iID == iActionId ||
			(m_ActionList[i].uiFlags & ActionInfo::FLAG_CategoryBitMask) != (actionRef.uiFlags & ActionInfo::FLAG_CategoryBitMask))
		{
			continue;
		}

		if(m_ActionList[i].ePadBtn == eBtn)
		{
			m_ActionList[i].ePadBtn = HYPAD_Unassigned;
			return m_ActionList[i].iID;
		}
	}

	return -1;
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

HyGamePad *HyInputMap::GetGamePad() const
{
	return m_pGamePad;
}

void HyInputMap::AssignGamePad(HyGamePad *pGamePad)
{
	m_pGamePad = pGamePad;
}

void HyInputMap::RemoveGamePad()
{
	m_pGamePad = nullptr;
}

const std::vector<HyJoystick *> &HyInputMap::GetJoystickList() const
{
	return m_JoystickList;
}

void HyInputMap::AssignJoystick(HyJoystick *pJoystick)
{
	if(pJoystick == nullptr)
		return;
	for(uint32 i = 0; i < m_JoystickList.size(); ++i)
	{
		if(m_JoystickList[i] == pJoystick)
			return;
	}
	m_JoystickList.push_back(pJoystick);
}

void HyInputMap::RemoveJoystick(HyJoystick *pJoystick)
{
	for(uint32 i = 0; i < m_JoystickList.size(); ++i)
	{
		if(m_JoystickList[i] == pJoystick)
		{
			m_JoystickList.erase(m_JoystickList.begin() + i);
			return;
		}
	}
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

void HyInputMap::ApplyKeyBoardInput(int32 iKey, HyBtnPressState ePressState)
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

void HyInputMap::ApplyGamePadButton(HyGamePadBtn eGamePadBtn, HyBtnPressState ePressState)
{
	for(uint32 i = 0; i < static_cast<uint32>(m_ActionList.size()); ++i)
	{
		if(m_ActionList[i].ePadBtn == eGamePadBtn)
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
