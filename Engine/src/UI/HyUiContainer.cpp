/**************************************************************************
*	HyUiContainer.cpp
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "UI/HyUiContainer.h"
#include "HyEngine.h"

HyUiContainer *HyUiContainer::sm_pCurModalContainer = nullptr;
std::vector<HyUiContainer *> HyUiContainer::sm_pContainerList;
HySpacerHandle HyUiContainer::sm_hSpacerHandleCounter = 1;
HyLayoutHandle HyUiContainer::sm_hLayoutHandleCounter = 1;

HyUiContainer::HyUiContainer(HyOrientation eRootLayoutDirection, const HyPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_bInputAllowed(true),
	m_Panel(initRef, this),
	m_RootLayout(eRootLayoutDirection, this),
	m_eContainerState(CONTAINERSTATE_Shown),
	m_fElapsedTime(0.0f)
{
	m_RootLayout.SetSizePolicy(HYSIZEPOLICY_Flexible, HYSIZEPOLICY_Flexible);
	m_RootLayout.SetLayoutDirty();

	sm_pContainerList.push_back(this);
}

/*virtual*/ HyUiContainer::~HyUiContainer()
{
	ClearItems();

	for(auto it = sm_pContainerList.begin(); it != sm_pContainerList.end(); ++it)
	{
		if((*it) == this)
		{
			sm_pContainerList.erase(it);
			break;
		}
	}
}

/*static*/ bool HyUiContainer::IsModalActive()
{
	return sm_pCurModalContainer != nullptr;
}

glm::ivec2 HyUiContainer::GetSize()
{
	return m_Panel.GetSize();
}

/*virtual*/ void HyUiContainer::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	m_Panel.SetSize(iNewWidth, iNewHeight);
	m_RootLayout.SetLayoutDirty();
}

bool HyUiContainer::Show(bool bInstant /*= false*/)
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

bool HyUiContainer::Hide(bool bInstant /*= false*/)
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

bool HyUiContainer::IsTransition()
{
	return m_eContainerState == CONTAINERSTATE_Showing || m_eContainerState == CONTAINERSTATE_Hiding;
}

bool HyUiContainer::IsShown()
{
	return m_eContainerState == CONTAINERSTATE_Shown || m_eContainerState == CONTAINERSTATE_Showing;
}

void HyUiContainer::SetAsModal()
{
	if(sm_pCurModalContainer == this)
		return;

	for(uint32 i = 0; i < static_cast<uint32>(sm_pContainerList.size()); ++i)
	{
		IHyWidget *pFocusedWidget = sm_pContainerList[i]->GetFocusedWidget();
		if(sm_pContainerList[i] == this || sm_pContainerList[i]->m_bInputAllowed == false || pFocusedWidget == nullptr)
			continue;
		else
			pFocusedWidget->RelinquishKeyboardFocus();
	}

	sm_pCurModalContainer = this;
}

/*static*/ void HyUiContainer::RelinquishModal()
{
	for(uint32 i = 0; i < static_cast<uint32>(sm_pContainerList.size()); ++i)
	{
		IHyWidget *pFocusedWidget = sm_pContainerList[i]->GetFocusedWidget();
		if(sm_pContainerList[i] == sm_pCurModalContainer || sm_pContainerList[i]->m_bInputAllowed == false || pFocusedWidget == nullptr)
			continue;
		else
			pFocusedWidget->TakeKeyboardFocus();
	}

	sm_pCurModalContainer = nullptr;
}

bool HyUiContainer::IsInputAllowed() const
{
	return m_bInputAllowed && (sm_pCurModalContainer == nullptr || sm_pCurModalContainer == this);
}

void HyUiContainer::SetInputAllowed(bool bEnable)
{
	if(m_bInputAllowed == bEnable)
		return;

	m_bInputAllowed = bEnable;
	IHyWidget *pFocusedWidget = GetFocusedWidget();
	if(pFocusedWidget && (sm_pCurModalContainer == nullptr || sm_pCurModalContainer == this))
	{
		if(m_bInputAllowed)
			pFocusedWidget->TakeKeyboardFocus();
		else
			pFocusedWidget->RelinquishKeyboardFocus();
	}
}

IHyWidget *HyUiContainer::GetFocusedWidget()
{
	auto widgetList = AssembleWidgetList();
	for(uint32 i = 0; i < static_cast<uint32>(widgetList.size()); ++i)
	{
		if(widgetList[i]->IsKeyboardFocus())
			return widgetList[i];
	}

	return nullptr;
}

IHyWidget *HyUiContainer::FocusNextWidget()
{
	auto widgetList = AssembleWidgetList();
	IHyWidget *pOldFocusedWidget = nullptr;
	IHyWidget *pNewFocusedWidget = nullptr;
	for(uint32 i = 0; i < static_cast<uint32>(widgetList.size()); ++i)
	{
		if(pOldFocusedWidget == nullptr)
		{
			if(widgetList[i]->IsKeyboardFocus())
				pOldFocusedWidget = widgetList[i];
		}
		else
		{
			if(widgetList[i]->IsKeyboardFocusAllowed())
			{
				pNewFocusedWidget = widgetList[i];
				break;
			}
		}
	}

	if(pNewFocusedWidget == nullptr)
	{
		for(uint32 i = 0; i < static_cast<uint32>(widgetList.size()); ++i)
		{
			if(widgetList[i]->IsKeyboardFocusAllowed())
			{
				pNewFocusedWidget = widgetList[i];
				break;
			}
		}
	}

	if(pOldFocusedWidget)
		pOldFocusedWidget->RelinquishKeyboardFocus();
	if(pNewFocusedWidget)
		pNewFocusedWidget->TakeKeyboardFocus();

	return pNewFocusedWidget;
}

bool HyUiContainer::InsertWidget(IHyWidget &widgetRef, HyLayoutHandle hInsertInto /*= HY_UNUSED_HANDLE*/)
{
	if(hInsertInto == HY_UNUSED_HANDLE)
	{
		m_RootLayout.AppendItem(widgetRef);
		return true;
	}
	else if(m_SubLayoutMap.find(hInsertInto) != m_SubLayoutMap.end())
	{
		m_SubLayoutMap[hInsertInto]->AppendItem(widgetRef);
		return true;
	}

	return false;
}

HySpacerHandle HyUiContainer::InsertSpacer(HySizePolicy eSizePolicy /*= HYSIZEPOLICY_Expanding*/, uint32 uiSizeHint /*= 0*/, HyLayoutHandle hInsertInto /*= HY_UNUSED_HANDLE*/)
{
	HySpacerHandle hNewSpacerHandle = HY_UNUSED_HANDLE;

	if(hInsertInto == HY_UNUSED_HANDLE)
	{
		hNewSpacerHandle = sm_hSpacerHandleCounter++;
		m_SubSpacerMap.insert(std::pair<HySpacerHandle, HySpacer *>(hNewSpacerHandle, HY_NEW HySpacer(m_RootLayout.GetLayoutType())));

		m_RootLayout.AppendItem(*m_SubSpacerMap[hNewSpacerHandle]);
		m_SubSpacerMap[hNewSpacerHandle]->Setup(eSizePolicy, uiSizeHint);
	}
	else if(m_SubLayoutMap.find(hInsertInto) != m_SubLayoutMap.end())
	{
		hNewSpacerHandle = sm_hSpacerHandleCounter++;
		m_SubSpacerMap.insert(std::pair<HySpacerHandle, HySpacer *>(hNewSpacerHandle, HY_NEW HySpacer(m_SubLayoutMap[hInsertInto]->GetLayoutType())));

		m_SubLayoutMap[hInsertInto]->AppendItem(*m_SubSpacerMap[hNewSpacerHandle]);
		m_SubSpacerMap[hNewSpacerHandle]->Setup(eSizePolicy, uiSizeHint);
	}

	return hNewSpacerHandle;
}

uint32 HyUiContainer::GetSpacerSize(HySpacerHandle hSpacer)
{
	if(m_RootLayout.IsLayoutDirty())
		OnRootLayoutUpdate();

	if(m_SubSpacerMap.find(hSpacer) != m_SubSpacerMap.end())
		return static_cast<uint32>(m_SubSpacerMap.at(hSpacer)->GetActualSize());

	return 0;
}

bool HyUiContainer::SetSpacerSize(HySpacerHandle hSpacer, HySizePolicy eSizePolicy, uint32 uiSizeHint)
{
	if(m_SubSpacerMap.find(hSpacer) != m_SubSpacerMap.end())
	{
		m_SubSpacerMap[hSpacer]->Setup(eSizePolicy, uiSizeHint);
		return true;
	}
	return false;
}

HyLayoutHandle HyUiContainer::InsertLayout(HyOrientation eNewLayoutType, HyLayoutHandle hInsertInto /*= HY_UNUSED_HANDLE*/)
{
	HyLayoutHandle hNewLayoutHandle = HY_UNUSED_HANDLE;

	if(hInsertInto == HY_UNUSED_HANDLE)
	{
		hNewLayoutHandle = sm_hLayoutHandleCounter++;
		m_SubLayoutMap.insert(std::pair<HyLayoutHandle, HyLayout *>(hNewLayoutHandle, HY_NEW HyLayout(eNewLayoutType)));

		m_RootLayout.AppendItem(*m_SubLayoutMap[hNewLayoutHandle]);
	}
	else if(m_SubLayoutMap.find(hInsertInto) != m_SubLayoutMap.end())
	{
		hNewLayoutHandle = sm_hLayoutHandleCounter++;
		m_SubLayoutMap.insert(std::pair<HyLayoutHandle, HyLayout *>(hNewLayoutHandle, HY_NEW HyLayout(eNewLayoutType)));

		m_SubLayoutMap[hInsertInto]->AppendItem(*m_SubLayoutMap[hNewLayoutHandle]);
	}

	return hNewLayoutHandle;
}

glm::ivec2 HyUiContainer::GetLayoutSize(HyLayoutHandle hLayout)
{
	if(m_RootLayout.IsLayoutDirty())
		OnRootLayoutUpdate();

	if(m_SubLayoutMap.find(hLayout) != m_SubLayoutMap.end())
		return m_SubLayoutMap.at(hLayout)->GetActualSize();

	return glm::ivec2(0.0f);
}

bool HyUiContainer::SetLayoutMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, int32 iWidgetSpacing, HyLayoutHandle hAffectedLayout /*= HY_UNUSED_HANDLE*/)
{
	if(hAffectedLayout == HY_UNUSED_HANDLE)
	{
		m_RootLayout.SetMargins(iLeft, iBottom, iRight, iTop, iWidgetSpacing);
		return true;
	}
	else if(m_SubLayoutMap.find(hAffectedLayout) != m_SubLayoutMap.end())
	{
		m_SubLayoutMap[hAffectedLayout]->SetMargins(iLeft, iBottom, iRight, iTop, iWidgetSpacing);
		return true;
	}

	return false;
}

void HyUiContainer::ClearItems()
{
	IHyWidget *pFocusedWidget = GetFocusedWidget();
	if(pFocusedWidget)
		pFocusedWidget->RelinquishKeyboardFocus();

	m_RootLayout.DetachAllItems();

	for(auto pSubSpacer : m_SubSpacerMap)
		delete pSubSpacer.second;
	m_SubSpacerMap.clear();

	for(auto pSubLayout : m_SubLayoutMap)
		delete pSubLayout.second;
	m_SubLayoutMap.clear();
}

/*virtual*/ void HyUiContainer::OnUpdate() /*override final*/
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

	m_fElapsedTime = 0.0f;
	OnContainerUpdate();
}

std::vector<IHyWidget *> HyUiContainer::AssembleWidgetList()
{
	std::vector<IHyWidget *> widgetList;

	std::function<void(HyLayout *)> fpRecursive = [&](HyLayout *pLayout) {
		for(uint32 i = 0; i < pLayout->ChildCount(); ++i)
		{
			IHyEntityUi *pItem = static_cast<IHyEntityUi *>(pLayout->ChildGet(i));
			if((pItem->GetInternalFlags() & NODETYPE_IsLayout) == 0)
				widgetList.push_back(static_cast<IHyWidget *>(pItem));
			else
				fpRecursive(static_cast<HyLayout *>(pItem));
		}
	};
	fpRecursive(&m_RootLayout);

	return widgetList;
}

bool HyUiContainer::RequestWidgetFocus(IHyWidget *pWidget)
{
	IHyWidget *pFocusedWidget = GetFocusedWidget();
	if(pWidget == nullptr || IsInputAllowed() == false || pFocusedWidget == pWidget)
		return false;

	if(pFocusedWidget)
		pFocusedWidget->RelinquishKeyboardFocus();

	pWidget->TakeKeyboardFocus();
	return true;
}

/*static*/ void HyUiContainer::DistrubuteTextInput(std::string sText)
{
	for(uint32 i = 0; i < static_cast<uint32>(sm_pContainerList.size()); ++i)
	{
		IHyWidget *pFocusedWidget = sm_pContainerList[i]->GetFocusedWidget();
		if(pFocusedWidget && sm_pContainerList[i]->IsInputAllowed())
			pFocusedWidget->OnUiTextInput(sText);
	}
}

/*static*/ void HyUiContainer::DistrubuteKeyboardInput(HyKeyboardBtn eBtn)
{
	// Check for 'TAB' and 'SHIFT+TAB' to cycle keyboard focus to valid widgets
	switch(eBtn)
	{
	case HYKEY_Tab:
		for(uint32 i = 0; i < static_cast<uint32>(sm_pContainerList.size()); ++i)
		{
			if(sm_pContainerList[i]->IsInputAllowed())
				sm_pContainerList[i]->FocusNextWidget();
		}
		break;

	default:
		break;
	}

	for(uint32 i = 0; i < static_cast<uint32>(sm_pContainerList.size()); ++i)
	{
		IHyWidget *pFocusedWidget = sm_pContainerList[i]->GetFocusedWidget();
		if(pFocusedWidget && sm_pContainerList[i]->IsInputAllowed())
			pFocusedWidget->OnUiKeyboardInput(eBtn);
	}
}
