/**************************************************************************
*	HyButtonGroup.cpp
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Components/HyButtonGroup.h"
#include "Gui/Widgets/HyButton.h"

HyButtonGroup::HyButtonGroup() :
	m_eExclusiveState(EXCLUSIVE_Yes),
	m_iExclusiveCheckedId(HYBUTTONGROUP_AUTO_ID),
	m_bProcessing(false)
{
}

/*virtual*/ HyButtonGroup::~HyButtonGroup()
{
	for(auto iter = m_ButtonMap.begin(); iter != m_ButtonMap.end(); ++iter)
		iter->second->m_pButtonGroup = nullptr;
}

bool HyButtonGroup::IsExclusive() const
{
	return m_eExclusiveState == EXCLUSIVE_Yes;
}

bool HyButtonGroup::Contains(HyButton *button) const
{
	for(auto iter = m_ButtonMap.begin(); iter != m_ButtonMap.end(); ++iter)
	{
		if(iter->second == button)
			return true;
	}
	return false;
}

HyButton *HyButtonGroup::GetButton(int32 iId) const
{
	auto iter = m_ButtonMap.find(iId);
	if(iter != m_ButtonMap.end())
		return iter->second;
	
	return nullptr;
}

std::vector<HyButton *> HyButtonGroup::GetButtonList() const
{
	std::vector<HyButton *> buttonList;
	for(auto iter = m_ButtonMap.begin(); iter != m_ButtonMap.end(); ++iter)
		buttonList.push_back(iter->second);

	return buttonList;
}

int32 HyButtonGroup::AddButton(HyButton &buttonRef, int32 iId /*= HYBUTTONGROUP_AUTO_ID*/)
{
	if(buttonRef.m_pButtonGroup)
		buttonRef.m_pButtonGroup->RemoveButton(buttonRef);

	if(iId == HYBUTTONGROUP_AUTO_ID)
	{
		iId--; // Start at -2, then decrement until unique
		while(m_ButtonMap.find(iId) != m_ButtonMap.end())
			iId--;
	}
	else
	{
		while(iId == HYBUTTONGROUP_AUTO_ID || m_ButtonMap.find(iId) != m_ButtonMap.end())
			iId++;
	}

	m_ButtonMap[iId] = &buttonRef;
	buttonRef.m_pButtonGroup = this;

	if(m_eExclusiveState == EXCLUSIVE_Yes || (m_eExclusiveState == EXCLUSIVE_Auto && buttonRef.IsAutoExclusive()))
	{
		if(m_iExclusiveCheckedId == HYBUTTONGROUP_AUTO_ID)
		{
			m_iExclusiveCheckedId = iId;
			buttonRef.SetChecked(true);
		}
		else
			buttonRef.SetChecked(false);
	}

	return iId;
}

HyButton *HyButtonGroup::CheckedButton() const
{
	if(m_iExclusiveCheckedId == HYBUTTONGROUP_AUTO_ID)
		return nullptr;

	auto iter = m_ButtonMap.find(m_iExclusiveCheckedId);
	if(iter != m_ButtonMap.end())
		return iter->second;

	return nullptr;
}

int	HyButtonGroup::CheckedId() const
{
	return m_iExclusiveCheckedId;
}

int	HyButtonGroup::GetId(HyButton *button) const
{
	for(auto iter = m_ButtonMap.begin(); iter != m_ButtonMap.end(); ++iter)
	{
		if(iter->second == button)
			return iter->first;
	}

	return HYBUTTONGROUP_AUTO_ID;
}

bool HyButtonGroup::RemoveButton(HyButton &buttonRef)
{
	for(auto iter = m_ButtonMap.begin(); iter != m_ButtonMap.end(); ++iter)
	{
		if(iter->second == &buttonRef)
		{
			int32 iDeletedBtnId = iter->first;
			m_ButtonMap.erase(iter);
			buttonRef.m_pButtonGroup = nullptr;

			// If this deleted button was the 'exclusive checked button', Find a new button to be the exclusive checked button
			if(iDeletedBtnId == m_iExclusiveCheckedId)
			{
				m_iExclusiveCheckedId = HYBUTTONGROUP_AUTO_ID;
				for(auto iter2 = m_ButtonMap.begin(); iter2 != m_ButtonMap.end(); ++iter2)
				{
					if(m_eExclusiveState == EXCLUSIVE_Yes || (m_eExclusiveState == EXCLUSIVE_Auto && iter2->second->IsAutoExclusive()))
					{
						m_iExclusiveCheckedId = iter2->first;
						iter2->second->SetChecked(true);
						break;
					}
				}
			}
			
			return true;
		}
	}

	return false;
}

void HyButtonGroup::SetAsExclusive(bool bIsExclusive)
{
	m_eExclusiveState = bIsExclusive ? EXCLUSIVE_Yes : EXCLUSIVE_No;
}

void HyButtonGroup::SetAsAutoExclusive()
{
	m_eExclusiveState = EXCLUSIVE_Auto;
}

bool HyButtonGroup::ProcessButtonChecked(HyButton &buttonRef, bool bChecked)
{
	HyAssert(Contains(&buttonRef), "HyButtonGroup::OnButtonChecked() - buttonRef is not part of this group");

	if(m_bProcessing) // Avoid recursion
		return true;

	m_bProcessing = true;

	if(bChecked)
	{
		if(m_eExclusiveState == EXCLUSIVE_Yes)
		{
			for(auto iter = m_ButtonMap.begin(); iter != m_ButtonMap.end(); ++iter)
			{
				if(iter->second != &buttonRef)
					iter->second->SetChecked(false);
			}
			m_iExclusiveCheckedId = GetId(&buttonRef);
		}
		else if(m_eExclusiveState == EXCLUSIVE_Auto && buttonRef.IsAutoExclusive())
		{
			for(auto iter = m_ButtonMap.begin(); iter != m_ButtonMap.end(); ++iter)
			{
				if(iter->second != &buttonRef && iter->second->IsAutoExclusive())
					iter->second->SetChecked(false);
			}
			m_iExclusiveCheckedId = GetId(&buttonRef);
		}
	}
	else
	{
		if(m_iExclusiveCheckedId == GetId(&buttonRef))
		{
			m_bProcessing = false;
			return false;
		}
	}

	m_bProcessing = false;
	return true;
}
