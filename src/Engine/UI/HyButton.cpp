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

HyButton::HyButton(HyEntity2d *pParent /*= nullptr*/) :
	HyInfoPanel(pParent),
	m_fpButtonClickedCallback(nullptr)
{
}

HyButton::HyButton(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent) :
	HyInfoPanel(nullptr),
	m_fpButtonClickedCallback(nullptr)
{
	Init(szPanelPrefix, szPanelName, pParent);
}

HyButton::HyButton(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyInfoPanel(nullptr),
	m_fpButtonClickedCallback(nullptr)
{
	Init(szTextPrefix, szTextName, iTextDimensionsX, iTextDimensionsY, pParent);
}

HyButton::HyButton(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyInfoPanel(nullptr),
	m_fpButtonClickedCallback(nullptr)
{
	Init(szPanelPrefix, szPanelName, szTextPrefix, szTextName, iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY, pParent);
}

/*virtual*/ HyButton::~HyButton()
{
}

/*virtual*/ void HyButton::Init(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent) /*override*/
{
	Init(szPanelPrefix, szPanelName, nullptr, nullptr, 0, 0, 0, 0, pParent);
}

/*virtual*/ void HyButton::Init(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) /*override*/
{
	Init(nullptr, nullptr, szTextPrefix, szTextName, 0, 0, iTextDimensionsX, iTextDimensionsY, pParent);
}

/*virtual*/ void HyButton::Init(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) /*override*/
{
	HyInfoPanel::Init(szPanelPrefix, szPanelName, szTextPrefix, szTextName, iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY, pParent);

	HyAssert(m_pPanel, "HyButton was constructed with a null m_pPanel");
	HyAssert(m_pPanel->AnimGetNumStates() >= 2, "HyButton was constructed with a panel with not enough anim states");

	EnableMouseInput();
}

void HyButton::SetAsSelected(bool bInvokeButtonClicked)
{
	if(bInvokeButtonClicked)
		OnMouseClicked();
	else
		m_pPanel->AnimSetState(HYBUTTONSTATE_Selected);
}

void HyButton::SetAsDisabled()
{
	m_pPanel->AnimSetState(HYBUTTONSTATE_Disabled);
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack)
{
	m_fpButtonClickedCallback = fpCallBack;
}

/*virtual*/ void HyButton::OnMouseDown() /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Down);
}

/*virtual*/ void HyButton::OnMouseUp() /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void HyButton::OnMouseLeave() /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void HyButton::OnMouseClicked() /*override final*/
{
	if(m_fpButtonClickedCallback)
		m_fpButtonClickedCallback(this);
}
