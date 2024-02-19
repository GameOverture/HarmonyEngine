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
#include "HyEngine.h"

HyButton::HyButton(HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(pParent),
	m_pButtonGroup(nullptr),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
}

HyButton::HyButton(const HyPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, pParent),
	m_pButtonGroup(nullptr),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
	OnSetup();
}

HyButton::HyButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, textNodePath, pParent),
	m_pButtonGroup(nullptr),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
	OnSetup();
}

HyButton::HyButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent /*= nullptr*/) :
	HyLabel(panelInit, textNodePath, iTextMarginLeft, iTextMarginBottom, iTextMarginRight, iTextMarginTop, pParent),
	m_pButtonGroup(nullptr),
	m_fpBtnClickedCallback(nullptr),
	m_pBtnClickedParam(nullptr)
{
	m_uiAttribs |= WIDGETATTRIB_IsTypeButton;
	OnSetup();
}

/*virtual*/ HyButton::~HyButton()
{
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
	
	ResetTextAndPanel();
	OnSetChecked(bChecked);
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/, const HyNodePath &audioNodePath /*= HyNodePath()*/)
{
	m_fpBtnClickedCallback = fpCallBack;
	m_pBtnClickedParam = pParam;
	m_ClickedSound.Init(audioNodePath, this);
}

void HyButton::InvokeButtonClicked()
{
	OnMouseClicked();
}

/*virtual*/ void HyButton::OnUiMouseClicked() /*override*/
{
	if(m_fpBtnClickedCallback)
		m_fpBtnClickedCallback(this, m_pBtnClickedParam);

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

/*virtual*/ void HyButton::OnSetup() /*override*/
{
	SetKeyboardFocusAllowed(true);
	SetAsHighlighted(IsHighlighted());
	SetMouseHoverCursor(HYMOUSECURSOR_Hand);
	OnSetChecked(IsChecked());
}
