/**************************************************************************
*	HyContainer.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/Containers/HyContainer.h"
#include "UI/Layouts/HyBoxLayout.h"
#include "HyEngine.h"

HyContainer::HyContainer(HyLayoutType eRootLayout, HyEntity2d *pParent /*= nullptr*/) :
	HyEntityUi(Ui_Container, pParent),
	m_pRootLayout(nullptr),
	m_pPrimPanel(nullptr),
	m_eContainerState(CONTAINERSTATE_Shown),
	m_fElapsedTime(0.0f)
{
	AllocRootLayout(eRootLayout);
}

HyContainer::HyContainer(HyLayoutType eRootLayout, int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent /*= nullptr*/) :
	HyEntityUi(Ui_Container, pParent),
	m_pRootLayout(nullptr),
	m_pPrimPanel(HY_NEW HyPrimitivePanel(iWidth, iHeight, iStroke, this)),
	m_eContainerState(CONTAINERSTATE_Shown),
	m_fElapsedTime(0.0f)
{
	AllocRootLayout(eRootLayout);
}

/*virtual*/ HyContainer::~HyContainer()
{
	delete m_pRootLayout;
	delete m_pPrimPanel;
}

glm::ivec2 HyContainer::GetSize() const
{
	return m_pRootLayout->GetSize();
}

void HyContainer::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	HyInternal_LayoutSetSize(*m_pRootLayout, iNewWidth, iNewHeight);

	if(m_pPrimPanel)
		m_pPrimPanel->SetSize(iNewWidth, iNewHeight);
}

bool HyContainer::Show(bool bInstant /*= false*/)
{
	if(IsShown() || IsTransition())
		return false;

	if(bInstant)
	{
		m_eContainerState = CONTAINERSTATE_Shown;
		m_fElapsedTime = 0.0f;

		OnShown();
	}
	else
	{
		m_eContainerState = CONTAINERSTATE_Showing;
		m_fElapsedTime = OnBeginShow();
	}

	return true;
}

bool HyContainer::Hide(bool bInstant /*= false*/)
{
	if(IsShown() == false || IsTransition())
		return false;

	if(bInstant)
	{
		m_eContainerState = CONTAINERSTATE_Hidden;
		m_fElapsedTime = 0.0f;

		OnHidden();
	}
	else
	{
		m_eContainerState = CONTAINERSTATE_Hiding;
		m_fElapsedTime = OnBeginHide();
	}

	return true;
}

bool HyContainer::IsTransition()
{
	return m_eContainerState == CONTAINERSTATE_Showing || m_eContainerState == CONTAINERSTATE_Hiding;
}

bool HyContainer::IsShown()
{
	return m_eContainerState == CONTAINERSTATE_Shown || m_eContainerState == CONTAINERSTATE_Showing;
}

IHyLayout *HyContainer::GetRootLayout()
{
	return m_pRootLayout;
}

/*virtual*/ void HyContainer::OnUpdate() /*override final*/
{
	if(m_fElapsedTime > 0.0f)
	{
		m_fElapsedTime -= HyEngine::DeltaTime();
		return;
	}

	switch(m_eContainerState)
	{
	case CONTAINERSTATE_Showing:
		m_eContainerState = CONTAINERSTATE_Shown;
		OnShown();
		break;

	case CONTAINERSTATE_Hiding:
		m_eContainerState = CONTAINERSTATE_Hidden;
		OnHidden();
		break;

	default:
		break;
	}

	m_fElapsedTime = 0.0f;
	OnContainerUpdate();
}

void HyContainer::AllocRootLayout(HyLayoutType eRootLayout)
{
	switch(eRootLayout)
	{
	case HYLAYOUT_Horizontal:
		m_pRootLayout = HY_NEW HyBoxLayout(HYORIEN_Horizontal, this);
		break;
	case HYLAYOUT_Vertical:
		m_pRootLayout = HY_NEW HyBoxLayout(HYORIEN_Vertical, this);
		break;

	default:
		HyError("HyContainer::HyContainer layout type not supported");
		break;
	}

	if(m_pPrimPanel)
		HyInternal_LayoutSetSize(*m_pRootLayout, m_pPrimPanel->GetSize().x, m_pPrimPanel->GetSize().y);
	//m_pRootLayout-
}

/*virtual*/ glm::ivec2 HyContainer::GetSizeHint() /*override*/
{
	return GetSize();
}
