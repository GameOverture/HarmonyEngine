/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyButton.h"

HyButton::HyButton(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyInfoPanel(szPanelPrefix, szPanelName, szTextPrefix, szTextName, iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY, pParent),
	m_fpButtonClickedCallback(nullptr),
	m_pCallbackParam(nullptr)
{
	HyAssert(m_pPanel, "HyButton was constructed with a null m_pPanel");
	HyAssert(m_pPanel->AnimGetNumStates() > HYBUTTONSTATE_Selected, "HyButton was constructed with a panel with not enough anim states");

	EnableMouseInput();
}

/*virtual*/ HyButton::~HyButton()
{
}

void HyButton::SetAsSelected(bool bInvokeButtonClicked)
{
	if(bInvokeButtonClicked)
		OnMouseClicked(nullptr);
	else
		m_pPanel->AnimSetState(HYBUTTONSTATE_Selected);
}

void HyButton::SetAsDisabled()
{
	m_pPanel->AnimSetState(HYBUTTONSTATE_Disabled);
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam /*= nullptr*/)
{
	m_fpButtonClickedCallback = fpCallBack;
	m_pCallbackParam = pParam;
}

/*virtual*/ void HyButton::OnMouseDown(void *pUserParam) /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Down);
}

/*virtual*/ void HyButton::OnMouseUp(void *pUserParam) /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void HyButton::OnMouseLeave(void *pUserParam) /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void HyButton::OnMouseClicked(void *pUserParam) /*override final*/
{
	m_pPanel->AnimSetState(HYBUTTONSTATE_Selected);

	if(m_fpButtonClickedCallback)
		m_fpButtonClickedCallback(this, m_pCallbackParam);
}
