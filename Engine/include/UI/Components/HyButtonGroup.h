/**************************************************************************
*	HyButtonGroup.h
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyButtonGroup_h__
#define HyButtonGroup_h__

#define HYBUTTONGROUP_AUTO_ID -1

#include "Afx/HyStdAfx.h"

class HyButton;

class HyButtonGroup
{
	friend class HyButton;

	enum ExclusiveState
	{
		EXCLUSIVE_Auto,
		EXCLUSIVE_Yes,
		EXCLUSIVE_No
	};
	ExclusiveState					m_eExclusiveState;
	int32							m_iExclusiveCheckedId;

	std::map<int32, HyButton *>		m_ButtonMap;

public:
	HyButtonGroup();
	virtual ~HyButtonGroup();

	bool IsExclusive() const;

	bool Contains(HyButton *button) const;
	HyButton *GetButton(int32 iId) const;
	std::vector<HyButton *> GetButtonList() const;
	
	int32 AddButton(HyButton &buttonRef, int32 iId = HYBUTTONGROUP_AUTO_ID);
	HyButton *CheckedButton() const;
	int	CheckedId() const;
	int	GetId(HyButton *button) const;
	bool RemoveButton(HyButton &buttonRef);
	void SetAsExclusive(bool bIsExclusive);
	void SetAsAutoExclusive();

private:
	bool ProcessButtonChecked(HyButton &buttonRef, bool bChecked);
};

#endif /* HyButtonGroup_h__ */
