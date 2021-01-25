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

HyButton::HyButton() :
	m_fpButtonClickedCallback(nullptr)
{
}

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName) :
	m_fpButtonClickedCallback(nullptr)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName);
}

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY) :
	m_fpButtonClickedCallback(nullptr)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY);
}

HyButton::HyButton(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY) :
	m_fpButtonClickedCallback(nullptr)
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);
}

/*virtual*/ HyButton::~HyButton()
{
}

/*virtual*/ void HyButton::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName) /*override*/
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, 0, 0, 0, 0);
}

/*virtual*/ void HyButton::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY) /*override*/
{
	Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, 0, 0);
}

/*virtual*/ void HyButton::Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY) /*override*/
{
	HyInfoPanel::Setup(sPanelPrefix, sPanelName, sTextPrefix, sTextName, iTextDimensionsX, iTextDimensionsY, iTextOffsetX, iTextOffsetY);

	HyAssert(m_Panel.GetNumStates() >= 4, "HyButton was constructed with a panel with not enough anim states");

	EnableMouseInput();
}

void HyButton::SetAsSelected(bool bInvokeButtonClicked)
{
	if(bInvokeButtonClicked)
		OnMouseClicked();
	else
		m_Panel.SetState(HYBUTTONSTATE_Selected);
}

void HyButton::SetAsDisabled()
{
	m_Panel.SetState(HYBUTTONSTATE_Disabled);
}

void HyButton::SetButtonClickedCallback(HyButtonClickedCallback fpCallBack)
{
	m_fpButtonClickedCallback = fpCallBack;
}

/*virtual*/ void HyButton::OnMouseDown() /*override final*/
{
	if(m_Panel.GetState() != HYBUTTONSTATE_Selected && m_Panel.GetState() != HYBUTTONSTATE_Disabled)
		m_Panel.SetState(HYBUTTONSTATE_Down);
}

/*virtual*/ void HyButton::OnMouseUp() /*override final*/
{
	if(m_Panel.GetState() != HYBUTTONSTATE_Selected && m_Panel.GetState() != HYBUTTONSTATE_Disabled)
		m_Panel.SetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void HyButton::OnMouseLeave() /*override final*/
{
	if(m_Panel.GetState() != HYBUTTONSTATE_Selected && m_Panel.GetState() != HYBUTTONSTATE_Disabled)
		m_Panel.SetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void HyButton::OnMouseClicked() /*override final*/
{
	if(m_fpButtonClickedCallback)
		m_fpButtonClickedCallback(this);
}
