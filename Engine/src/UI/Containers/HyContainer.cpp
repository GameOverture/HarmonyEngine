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
#include "HyEngine.h"

HyContainer *HyContainer::sm_pFocusedContainer = nullptr;
HyLayoutHandle HyContainer::sm_hHandleCounter = 1;

HyContainer::HyContainer(HyLayoutType eRootLayout, const HyPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_Panel(initRef, this),
	m_RootLayout(eRootLayout, this),
	m_eContainerState(CONTAINERSTATE_Shown),
	m_fElapsedTime(0.0f),
	m_pFocusedWidget(nullptr)
{
	m_RootLayout.SetSizePolicy(HYSIZEPOLICY_Flexible, HYSIZEPOLICY_Flexible);
	m_RootLayout.SetLayoutDirty();
}

/*virtual*/ HyContainer::~HyContainer()
{
	ClearItems();
}

glm::ivec2 HyContainer::GetSize()
{
	return m_Panel.GetSize();
}

/*virtual*/ void HyContainer::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	m_Panel.SetSize(iNewWidth, iNewHeight);
	m_RootLayout.SetLayoutDirty();
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

void HyContainer::TakeFocus()
{
	if(sm_pFocusedContainer == this)
		return;

	if(sm_pFocusedContainer && sm_pFocusedContainer->m_pFocusedWidget)
		sm_pFocusedContainer->m_pFocusedWidget->OnRelinquishFocus();

	sm_pFocusedContainer = this;
	if(sm_pFocusedContainer->m_pFocusedWidget)
		sm_pFocusedContainer->m_pFocusedWidget->OnTakeFocus();
	else
		sm_pFocusedContainer->FocusNextItem();
}

void HyContainer::RelinquishFocus()
{
	if(sm_pFocusedContainer == this)
	{
		sm_pFocusedContainer->m_pFocusedWidget->OnRelinquishFocus();
		sm_pFocusedContainer = nullptr;
	}
}

bool HyContainer::IsTransition()
{
	return m_eContainerState == CONTAINERSTATE_Showing || m_eContainerState == CONTAINERSTATE_Hiding;
}

bool HyContainer::IsShown()
{
	return m_eContainerState == CONTAINERSTATE_Shown || m_eContainerState == CONTAINERSTATE_Showing;
}

bool HyContainer::AppendWidget(IHyEntityUi &itemRef, HyLayoutHandle hInsertInto /*= HY_UNUSED_HANDLE*/)
{
	if(hInsertInto == HY_UNUSED_HANDLE)
	{
		m_RootLayout.AppendItem(itemRef);
		return true;
	}
	else if(m_SubLayoutMap.find(hInsertInto) != m_SubLayoutMap.end())
	{
		m_SubLayoutMap[hInsertInto]->AppendItem(itemRef);
		return true;
	}

	return false;
}

HyLayoutHandle HyContainer::InsertLayout(HyLayoutType eNewLayoutType, HyLayoutHandle hInsertInto /*= HY_UNUSED_HANDLE*/)
{
	HyLayoutHandle hNewLayoutHandle = HY_UNUSED_HANDLE;

	if(hInsertInto == HY_UNUSED_HANDLE)
	{
		hNewLayoutHandle = sm_hHandleCounter++;
		m_SubLayoutMap.insert(std::pair<HyLayoutHandle, HyLayout *>(hNewLayoutHandle, HY_NEW HyLayout(eNewLayoutType)));

		m_RootLayout.AppendItem(*m_SubLayoutMap[hNewLayoutHandle]);
	}
	else if(m_SubLayoutMap.find(hInsertInto) != m_SubLayoutMap.end())
	{
		hNewLayoutHandle = sm_hHandleCounter++;
		m_SubLayoutMap.insert(std::pair<HyLayoutHandle, HyLayout *>(hNewLayoutHandle, HY_NEW HyLayout(eNewLayoutType)));

		m_SubLayoutMap[hInsertInto]->AppendItem(*m_SubLayoutMap[hNewLayoutHandle]);
	}

	return hNewLayoutHandle;
}

void HyContainer::ClearItems()
{
	m_RootLayout.DetachAllItems();

	for(auto pSubLayout : m_SubLayoutMap)
		delete pSubLayout.second;
	m_SubLayoutMap.clear();
}

bool HyContainer::SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY, HyLayoutHandle hAffectedLayout /*= HY_UNUSED_HANDLE*/)
{
	if(hAffectedLayout == HY_UNUSED_HANDLE)
	{
		m_RootLayout.SetMargins(iLeft, iBottom, iRight, iTop, uiWidgetSpacingX, uiWidgetSpacingY);
		return true;
	}
	else if(m_SubLayoutMap.find(hAffectedLayout) != m_SubLayoutMap.end())
	{
		m_SubLayoutMap[hAffectedLayout]->SetMargins(iLeft, iBottom, iRight, iTop, uiWidgetSpacingX, uiWidgetSpacingY);
		return true;
	}

	return false;
}

IHyEntityUi *HyContainer::FocusNextItem()
{
	return nullptr;
	//if(m_pFocusedItem == nullptr)
	//{
	//	m_RootLayout.ForEachChild([&](IHyNode2d *pChild)
	//		{
	//			IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pChild);
	//			if(pItem->IsKeyboardFocusAllowed())
	//				m_pFocusedItem = pItem;
	//		});
	//}
}

/*virtual*/ void HyContainer::OnUpdate() /*override final*/
{
	if(m_RootLayout.IsLayoutDirty())
		OnRootLayoutUpdate();

	if(m_fElapsedTime > 0.0f)
	{
		m_fElapsedTime -= HyEngine::DeltaTime();
		OnContainerUpdate();
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

	// TODO: if(this == sm_pFocusedContainer) check for 'TAB' and 'SHIFT+TAB' to cycle keyboard focus to valid widgets

	m_fElapsedTime = 0.0f;
	OnContainerUpdate();
}

bool HyContainer::RequestWidgetFocus(IHyWidget *pWidget)
{
	if(pWidget != nullptr && sm_pFocusedContainer != this && m_pFocusedWidget != pWidget)
		return false;

	m_pFocusedWidget = pWidget;
	m_pFocusedWidget->OnTakeFocus();
}
