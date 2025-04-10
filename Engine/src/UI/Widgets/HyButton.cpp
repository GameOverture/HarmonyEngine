/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyButton.h"
#include "UI/Components/HyButtonGroup.h"
#include "HyEngine.h"

HyButton::HyButton(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_pButtonGroup(nullptr),
	m_fpBtnClickedCallback(nullptr)
{
	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
	SetKeyboardFocusAllowed(true);
	SetMouseHoverCursor(HYMOUSECURSOR_Hand);
	UsePanelStates();
}

HyButton::HyButton(const HyUiPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, pParent),
	m_pButtonGroup(nullptr),
	m_fpBtnClickedCallback(nullptr)
{
	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
	SetKeyboardFocusAllowed(true);
	SetMouseHoverCursor(HYMOUSECURSOR_Hand);
	UsePanelStates();
}

HyButton::HyButton(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, textInit, pParent),
	m_pButtonGroup(nullptr),
	m_fpBtnClickedCallback(nullptr)
{
	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
	SetKeyboardFocusAllowed(true);
	SetMouseHoverCursor(HYMOUSECURSOR_Hand);
	UsePanelStates();
}

//HyButton::HyButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
//	HyLabel(panelInit, textNodePath, pParent),
//	m_pButtonGroup(nullptr),
//	m_fpBtnClickedCallback(nullptr)
//{
//	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
//	SetKeyboardFocusAllowed(true);
//	SetMouseHoverCursor(HYMOUSECURSOR_Hand);
//	UsePanelStates();
//}
//
//HyButton::HyButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent /*= nullptr*/) :
//	HyLabel(panelInit, textNodePath, textMargins, pParent),
//	m_pButtonGroup(nullptr),
//	m_fpBtnClickedCallback(nullptr)
//{
//	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
//	SetKeyboardFocusAllowed(true);
//	SetMouseHoverCursor(HYMOUSECURSOR_Hand);
//	UsePanelStates();
//}

/*virtual*/ HyButton::~HyButton()
{
	if(m_pButtonGroup)
		m_pButtonGroup->RemoveButton(*this);
}

/*virtual*/ bool HyButton::IsDepressed() const /*override*/
{
	return IHyWidget::IsDepressed() || (m_uiAttribs & BTNATTRIB_IsKbDownState) != 0;
}

HyButtonGroup *HyButton::GetButtonGroup() const
{
	return m_pButtonGroup;
}

bool HyButton::IsAutoExclusive() const
{
	return (m_uiAttribs & BTNATTRIB_IsAutoExclusive) != 0;
}

bool HyButton::IsChecked() const
{
	return m_uiAttribs & BTNATTRIB_IsChecked;
}

void HyButton::SetChecked(bool bChecked)
{
	if(bChecked == IsChecked())
		return;

	if(bChecked)
		m_uiAttribs |= BTNATTRIB_IsChecked;
	else
		m_uiAttribs &= ~BTNATTRIB_IsChecked;

	if(m_pButtonGroup && m_pButtonGroup->ProcessButtonChecked(*this, bChecked) == false)
	{
		// Undo the change
		if(!bChecked)
			m_uiAttribs |= BTNATTRIB_IsChecked;
		else
			m_uiAttribs &= ~BTNATTRIB_IsChecked;

		return;
	}
	
	SetAssembleNeeded();
	OnSetChecked(bChecked);
}

void HyButton::SetButtonClickedCallback(std::function<void(HyButton *)> fpCallBack, const HyNodePath &audioNodePath /*= HyNodePath()*/)
{
	m_fpBtnClickedCallback = fpCallBack;
	m_ClickedSound.Init(audioNodePath, this);
}

void HyButton::InvokeButtonClicked()
{
	OnMouseClicked();
}

/*virtual*/ void HyButton::OnUiMouseClicked() /*override*/
{
	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this);

	if(m_ClickedSound.IsLoadDataValid() && m_ClickedSound.IsLoaded())
		m_ClickedSound.PlayOneShot(true);
}

/*virtual*/ void HyButton::OnRelinquishKeyboardFocus() /*override*/
{
	if((m_uiAttribs & BTNATTRIB_IsKbDownState) != 0)
	{
		m_uiAttribs &= ~BTNATTRIB_IsKbDownState;
		ApplyPanelState();
	}
}

/*virtual*/ void HyButton::OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) /*override*/
{
	if(eBtn == HYKEY_Space || eBtn == HYKEY_Enter)
	{
		if(eBtnState == HYBTN_Press && (m_uiAttribs & BTNATTRIB_IsKbDownState) == 0)
		{
			m_uiAttribs |= BTNATTRIB_IsKbDownState;
			ApplyPanelState();
		}
		else if(eBtnState == HYBTN_Release && (m_uiAttribs & BTNATTRIB_IsKbDownState) != 0)
		{
			m_uiAttribs &= ~BTNATTRIB_IsKbDownState;
			ApplyPanelState();

			if(IsDepressed() == false)
				InvokeButtonClicked();
		}
	}
}
