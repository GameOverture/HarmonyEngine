/**************************************************************************
*	IHyButton.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/IHyButton.h"

IHyButton::IHyButton(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent) :
	HyInfoPanel(szPanelPrefix, szPanelName, szTextPrefix, szTextName, iTextOffsetX, iTextOffsetY, iTextDimensionsX, iTextDimensionsY, pParent)
{
	HyAssert(m_pPanel, "IHyButton was constructed with a null m_pPanel");
}

/*virtual*/ IHyButton::~IHyButton()
{
}

void IHyButton::SetAsSelected(bool bInvokeButtonClicked)
{
	if(bInvokeButtonClicked)
		OnButtonClicked(m_pParent);
	m_pPanel->AnimSetState(HYBUTTONSTATE_Selected);
}

void IHyButton::SetAsDisabled()
{
	m_pPanel->AnimSetState(HYBUTTONSTATE_Disabled);
}


/*virtual*/ void IHyButton::OnMouseDown(void *pUserParam) /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Down);
}

/*virtual*/ void IHyButton::OnMouseUp(void *pUserParam) /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void IHyButton::OnMouseLeave(void *pUserParam) /*override final*/
{
	if(m_pPanel->AnimGetState() != HYBUTTONSTATE_Selected && m_pPanel->AnimGetState() != HYBUTTONSTATE_Disabled)
		m_pPanel->AnimSetState(HYBUTTONSTATE_Pressable);
}

/*virtual*/ void IHyButton::OnMouseClicked(void *pUserParam) /*override final*/
{
	OnButtonClicked(m_pParent);
}
