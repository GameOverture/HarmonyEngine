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

#define HYUICONTAINER_DefaultWidgetSpacing 8

HyUiContainer *HyUiContainer::sm_pCurModalContainer = nullptr;
std::vector<HyUiContainer *> HyUiContainer::sm_pContainerList;
HySpacerHandle HyUiContainer::sm_hSpacerHandleCounter = 1;
HyLayoutHandle HyUiContainer::sm_hLayoutHandleCounter = 1;

HyUiContainer::HyUiContainer(HyOrientation eRootLayoutDirection, const HyUiPanelInit &initRef, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	//m_Shape(this),
	m_bInputAllowed(true),
	m_iDefaultWidgetSpacing(HYUICONTAINER_DefaultWidgetSpacing),
	m_bFlexSizeX(true),
	m_bFlexSizeY(true),
	m_Panel(this),
	m_RootLayout(eRootLayoutDirection, HYUICONTAINER_DefaultWidgetSpacing, this),
	m_eContainerState(CONTAINERSTATE_Shown),
	m_fElapsedTime(0.0f),
	m_bUseVertBar(false),
	m_bUseHorzBar(false),
	m_VertBar(HYORIENT_Vertical, 20, this),
	m_HorzBar(HYORIENT_Horizontal, 20, this)
{
	m_RootBtnGrp.SetAsAutoExclusive();
	m_Panel.Setup(initRef);

	m_bFlexSizeX = m_Panel.GetWidth() == 0.0f;
	m_bFlexSizeY = m_Panel.GetHeight() == 0.0f;

	m_RootLayout.SetSizePolicy(HYSIZEPOLICY_Flexible, HYSIZEPOLICY_Flexible);
	m_RootLayout.SetLayoutDirty();

	sm_pContainerList.push_back(this);

	// Scroll bars
	SetScrollBarColor(initRef.m_PanelColor);
	EnableScrollBars(m_bUseVertBar, m_bUseHorzBar);

	SetSize(initRef.m_uiWidth, initRef.m_uiHeight);
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
	return glm::ivec2(m_Panel.GetWidth(), m_Panel.GetHeight());
}

void HyUiContainer::SetSize(int32 iNewWidth, int32 iNewHeight)
{
	//m_Shape.SetAsBox(iNewWidth, iNewHeight);
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

IHyWidget *HyUiContainer::FocusNextWidget(bool bForwardDirection)
{
	auto widgetList = AssembleWidgetList();
	IHyWidget *pOldFocusedWidget = nullptr;
	IHyWidget *pNewFocusedWidget = nullptr;

	std::function<void(IHyWidget *)> fpSortedFind = [&](IHyWidget *pWidget) -> void
	{
		if(pWidget == nullptr || (pOldFocusedWidget && pNewFocusedWidget))
			return;
		if(pOldFocusedWidget == nullptr)
		{
			if(pWidget->IsKeyboardFocus())
				pOldFocusedWidget = pWidget;
		}
		else
		{
			if(pWidget->IsKeyboardFocusAllowed())
			{
				pNewFocusedWidget = pWidget;
				return;
			}
		}
	};
	if(bForwardDirection)
		std::for_each(widgetList.begin(), widgetList.end(), fpSortedFind);
	else
		std::for_each(widgetList.rbegin(), widgetList.rend(), fpSortedFind);

	std::function<void(IHyWidget *)> fpSecondPass = [&](IHyWidget *pWidget) -> void
	{
		if(pWidget == nullptr || pNewFocusedWidget)
			return;
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
	};
	if(bForwardDirection)
		std::for_each(widgetList.begin(), widgetList.end(), fpSecondPass);
	else
		std::for_each(widgetList.rbegin(), widgetList.rend(), fpSecondPass);

	if(pOldFocusedWidget && pNewFocusedWidget)
	{
		pOldFocusedWidget->RelinquishKeyboardFocus();
		pNewFocusedWidget->TakeKeyboardFocus();
	}

	return pNewFocusedWidget;
}

glm::vec2 HyUiContainer::GetWidgetPos(IHyWidget &widgetRef)
{
	glm::vec2 vPos = widgetRef.pos.Get();

	for(auto iter = m_SubLayoutMap.begin(); iter != m_SubLayoutMap.end(); ++iter)
	{
		if(iter->second->ChildExists(widgetRef))
		{
			HyEntity2d *pCurLayout = iter->second;
			
			// Add together each "parent" layout
			// Propagate upward if this is nested in another layout
			while(pCurLayout && (pCurLayout->GetInternalFlags() & NODETYPE_IsLayout) != 0)
			{
				vPos += pCurLayout->pos.Get();
				pCurLayout = pCurLayout->ParentGet();
			}
			break;
		}
	}
	
	return vPos;
}

bool HyUiContainer::InsertWidget(IHyWidget &widgetRef, HyLayoutHandle hInsertInto /*= HY_UNUSED_HANDLE*/)
{
	bool bSuccess = false;
	if(hInsertInto == HY_UNUSED_HANDLE)
	{
		m_RootLayout.AppendItem(widgetRef);
		bSuccess = true;
	}
	else if(m_SubLayoutMap.find(hInsertInto) != m_SubLayoutMap.end())
	{
		m_SubLayoutMap[hInsertInto]->AppendItem(widgetRef);
		bSuccess = true;
	}

	if(bSuccess && widgetRef.IsButton() && static_cast<HyButton &>(widgetRef).GetButtonGroup() == nullptr)
		m_RootBtnGrp.AddButton(static_cast<HyButton &>(widgetRef));
	
	return bSuccess;
}

bool HyUiContainer::RemoveWidget(IHyWidget &widgetRef)
{
	for(uint32 i = 0; i < m_RootLayout.ChildCount(); ++i)
	{
		if(m_RootLayout.ChildGet(i) == &widgetRef)
		{
			m_RootLayout.RemoveItem(widgetRef);
			return true;
		}
	}
	
	for(auto iter = m_SubLayoutMap.begin(); iter != m_SubLayoutMap.end(); ++iter)
	{
		for(uint32 i = 0; i < iter->second->ChildCount(); ++i)
		{
			if(iter->second->ChildGet(i) == &widgetRef)
			{
				m_RootLayout.RemoveItem(widgetRef);
				return true;
			}
		}
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

bool HyUiContainer::RemoveSpacer(HySpacerHandle hSpacer)
{
	if(m_SubSpacerMap.find(hSpacer) != m_SubSpacerMap.end())
	{
		if(RemoveWidget(*m_SubSpacerMap[hSpacer]))
		{
			delete m_SubSpacerMap[hSpacer];
			m_SubSpacerMap.erase(hSpacer);
			return true;
		}
	}
	return false;
}

HyLayoutHandle HyUiContainer::InsertLayout(HyOrientation eNewLayoutType, HyLayoutHandle hInsertInto /*= HY_UNUSED_HANDLE*/)
{
	HyLayoutHandle hNewLayoutHandle = HY_UNUSED_HANDLE;

	if(hInsertInto == HY_UNUSED_HANDLE)
	{
		hNewLayoutHandle = sm_hLayoutHandleCounter++;
		m_SubLayoutMap.insert(std::pair<HyLayoutHandle, HyLayout *>(hNewLayoutHandle, HY_NEW HyLayout(eNewLayoutType, m_iDefaultWidgetSpacing)));

		m_RootLayout.AppendItem(*m_SubLayoutMap[hNewLayoutHandle]);
	}
	else if(m_SubLayoutMap.find(hInsertInto) != m_SubLayoutMap.end())
	{
		hNewLayoutHandle = sm_hLayoutHandleCounter++;
		m_SubLayoutMap.insert(std::pair<HyLayoutHandle, HyLayout *>(hNewLayoutHandle, HY_NEW HyLayout(eNewLayoutType, m_iDefaultWidgetSpacing)));

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

	return glm::ivec2(0, 0);
}

bool HyUiContainer::SetLayoutMargin(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, HyLayoutHandle hAffectedLayout /*= HY_UNUSED_HANDLE*/)
{
	if(hAffectedLayout == HY_UNUSED_HANDLE)
	{
		m_RootLayout.SetMargins(iLeft, iBottom, iRight, iTop, m_RootLayout.GetWidgetSpacing());
		return true;
	}
	else if(m_SubLayoutMap.find(hAffectedLayout) != m_SubLayoutMap.end())
	{
		m_SubLayoutMap[hAffectedLayout]->SetMargins(iLeft, iBottom, iRight, iTop, m_SubLayoutMap[hAffectedLayout]->GetWidgetSpacing());
		return true;
	}

	HyLogWarning("HyUiContainer::SetLayoutMargins could not find specified layout: " << hAffectedLayout);
	return false;
}

bool HyUiContainer::SetLayoutWidgetSpacing(int32 iWidgetSpacing, HyLayoutHandle hAffectedLayout /*= HY_UNUSED_HANDLE*/)
{
	if(hAffectedLayout == HY_UNUSED_HANDLE)
	{
		m_RootLayout.SetMargins(m_RootLayout.GetMargins().left,
								m_RootLayout.GetMargins().bottom,
								m_RootLayout.GetMargins().right,
								m_RootLayout.GetMargins().top,
								iWidgetSpacing);
		return true;
	}
	else if(m_SubLayoutMap.find(hAffectedLayout) != m_SubLayoutMap.end())
	{
		m_SubLayoutMap[hAffectedLayout]->SetMargins(m_SubLayoutMap[hAffectedLayout]->GetMargins().left,
													m_SubLayoutMap[hAffectedLayout]->GetMargins().bottom,
													m_SubLayoutMap[hAffectedLayout]->GetMargins().right,
													m_SubLayoutMap[hAffectedLayout]->GetMargins().top,
													iWidgetSpacing);
		return true;
	}

	HyLogWarning("HyUiContainer::SetLayoutWidgetSpacing could not find specified layout: " << hAffectedLayout);
	return false;
}

bool HyUiContainer::RemoveLayout(HyLayoutHandle hLayout)
{
	if(m_SubLayoutMap.find(hLayout) == m_SubLayoutMap.end())
		return false;

	HyLayout *pLayoutToRemove = m_SubLayoutMap[hLayout];

	if(m_RootLayout.RemoveItem(*pLayoutToRemove))
	{
		m_SubLayoutMap.erase(hLayout);
		delete pLayoutToRemove;
		return true;
	}

	for(auto iter = m_SubLayoutMap.begin(); iter != m_SubLayoutMap.end(); ++iter)
	{
		if(iter->second->RemoveItem(*pLayoutToRemove))
		{
			m_SubLayoutMap.erase(hLayout);
			delete pLayoutToRemove;
			return true;
		}
	}

	return false;
}

int32 HyUiContainer::GetDefaultWidgetSpacing() const
{
	return m_iDefaultWidgetSpacing;
}

void HyUiContainer::SetDefaultWidgetSpacing(int32 iSpacing, bool bSetRootLayout)
{
	m_iDefaultWidgetSpacing = iSpacing;
	if(bSetRootLayout)
	{
		m_RootLayout.SetMargins(m_RootLayout.GetMargins().left,
								m_RootLayout.GetMargins().bottom,
								m_RootLayout.GetMargins().right,
								m_RootLayout.GetMargins().top,
								m_iDefaultWidgetSpacing);
	}
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

void HyUiContainer::EnableScrollBars(bool bUseVert, bool bUseHorz)
{
	m_bUseVertBar = bUseVert;
	m_bUseHorzBar = bUseHorz;

	m_VertBar.SetOnScrollCallback(m_bUseVertBar ? OnScroll : nullptr, this);
	m_HorzBar.SetOnScrollCallback(m_bUseHorzBar ? OnScroll : nullptr, this);

	// Use alpha because *this container may want to SetVisible() and it should then propagate to the scroll bars
	m_VertBar.alpha.Set(static_cast<int32>(m_bUseVertBar) * 1.0f);
	m_HorzBar.alpha.Set(static_cast<int32>(m_bUseHorzBar) * 1.0f);

	m_RootLayout.SetLayoutDirty();
}

void HyUiContainer::SetScrollBarColor(HyColor color)
{
	m_VertBar.SetColor(color);
	m_HorzBar.SetColor(color);
}

void HyUiContainer::SetScrollBarSize(uint32 uiDiameter)
{
	m_VertBar.SetDiameter(uiDiameter);
	m_HorzBar.SetDiameter(uiDiameter);
	
	m_RootLayout.SetLayoutDirty();
}

void HyUiContainer::SetLineScrollAmt(float fLineScrollAmt)
{
	m_VertBar.SetLineScrollAmt(fLineScrollAmt);
	m_HorzBar.SetLineScrollAmt(fLineScrollAmt);
}

void HyUiContainer::ScrollTo(float fVertScrollPos, float fHorzScrollPos)
{
	m_VertBar.ScrollTo(fVertScrollPos);
	m_HorzBar.ScrollTo(fHorzScrollPos);
}

/*virtual*/ void HyUiContainer::OnUpdate() /*override final*/
{
	if(m_RootLayout.IsLayoutDirty())
	{
		OnRootLayoutUpdate();
		OnRootLayoutUpdate();
	}

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

	// Scroll bars
	if(IsShown())
	{
		glm::ivec2 vScroll = HyEngine::Input().GetMouseScroll();
		if(m_bUseVertBar)
			m_VertBar.DoLineScroll(-vScroll.y);
		if(m_bUseHorzBar && !m_bUseVertBar)
			m_HorzBar.DoLineScroll(vScroll.x);
	}

	// Derived classes
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

void HyUiContainer::OnRootLayoutUpdate()
{
	int32 iNewWidth = static_cast<int32>(m_Panel.GetWidth(m_Panel.scale.X()));
	int32 iNewHeight = static_cast<int32>(m_Panel.GetHeight(m_Panel.scale.Y()));

	if(iNewWidth == 0 || iNewHeight == 0)
		return;

	if(m_bUseVertBar || m_bUseHorzBar)
	{
		// If scrolling, then use '0' for that dimension, to indicate to the layout use the exact amount it needs.
		// NOTE: Using the layout's size hint (instead of '0') may be inaccurate if other dimension is being resized
		if(m_bUseHorzBar && m_RootLayout.GetSizeHint().x > iNewWidth)
		{
			//iNewHeight -= (m_RootLayout.GetMargins().top + m_RootLayout.GetMargins().bottom);
			iNewHeight -= m_HorzBar.GetDiameter();
			iNewWidth = 0;
		}

		if(m_bUseVertBar && m_RootLayout.GetSizeHint().y > iNewHeight)
		{
			if(iNewWidth != 0)
			{
				//iNewWidth -= (m_RootLayout.GetMargins().left + m_RootLayout.GetMargins().right);
				iNewWidth -= m_VertBar.GetDiameter();
			}
			iNewHeight = 0;
		}
	}

	bool bVertBarShown = iNewHeight == 0;
	bool bHorzBarShown = iNewWidth == 0;

	int32 iScissorMargin = 0;
	if(bVertBarShown || bHorzBarShown)
	{
		if(m_Panel.IsPrimitive())
			iScissorMargin = m_Panel.GetFrameStrokeSize();

		int32 iScissorWidth = static_cast<int32>(m_Panel.GetWidth(m_Panel.scale.X())) - (iScissorMargin * 2) - (static_cast<int32>(bVertBarShown) * m_VertBar.GetDiameter());
		int32 iScissorHeight = static_cast<int32>(m_Panel.GetHeight(m_Panel.scale.Y())) - (iScissorMargin * 2) - (static_cast<int32>(bHorzBarShown) * m_HorzBar.GetDiameter());
		if(iScissorWidth > 0 && iScissorHeight > 0)
			SetScissor(HyRect(static_cast<float>(iScissorMargin), static_cast<float>(iScissorMargin), static_cast<float>(iScissorWidth), static_cast<float>(iScissorHeight)));
		else
			ClearScissor(true);
	}
	else
		ClearScissor(true);

	m_Panel.ClearScissor(false);
	m_VertBar.ClearScissor(false);
	m_HorzBar.ClearScissor(false);

	glm::ivec2 vActualSize = m_RootLayout.Resize(iNewWidth, iNewHeight);

	if(bVertBarShown && bHorzBarShown == false)
	{
		m_VertBar.SetMetrics(GetSize().y - (iScissorMargin * 2), vActualSize.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_VertBar.GetDiameter() - iScissorMargin, iScissorMargin);
		m_VertBar.alpha.Set(1.0f);
		
		m_HorzBar.alpha.Set(0.0f);
		m_HorzBar.SetMetrics(GetSize().y - (iScissorMargin * 2), iNewWidth, iNewWidth);
	}
	else if(bVertBarShown == false && bHorzBarShown)
	{
		m_HorzBar.SetMetrics(GetSize().x - (iScissorMargin * 2), vActualSize.x, GetSize().x);
		m_HorzBar.pos.Set(iScissorMargin, iScissorMargin);
		m_HorzBar.alpha.Set(1.0f);
		
		m_VertBar.alpha.Set(0.0f);
		m_VertBar.SetMetrics(GetSize().y - (iScissorMargin * 2), iNewHeight, iNewHeight);
	}
	else if(bVertBarShown && bHorzBarShown)
	{
		m_VertBar.SetMetrics(GetSize().y - (iScissorMargin * 2) - m_HorzBar.GetDiameter(), vActualSize.y, GetSize().y);
		m_VertBar.pos.Set(GetSize().x - m_VertBar.GetDiameter() - iScissorMargin, iScissorMargin + static_cast<int32>(m_HorzBar.GetDiameter()));
		m_VertBar.alpha.Set(1.0f);

		m_HorzBar.SetMetrics(GetSize().x - (iScissorMargin * 2) - m_VertBar.GetDiameter(), vActualSize.x, GetSize().x);
		m_HorzBar.pos.Set(iScissorMargin, iScissorMargin);
		m_HorzBar.alpha.Set(1.0f);
	}
	else if(bVertBarShown == false && bHorzBarShown == false)
	{
		m_HorzBar.alpha.Set(0.0f);
		m_HorzBar.SetMetrics(GetSize().x - (iScissorMargin * 2), iNewWidth, iNewWidth);

		m_VertBar.alpha.Set(0.0f);
		m_VertBar.SetMetrics(GetSize().y - (iScissorMargin * 2), iNewHeight, iNewHeight);
	}
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

/*static*/ void HyUiContainer::DistrubuteKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods)
{
	// Check for 'TAB' and 'SHIFT+TAB' to cycle keyboard focus to valid widgets
	switch(eBtn)
	{
	case HYKEY_Tab:
		if(eBtnState != HYBTN_Release)
		{
			for(uint32 i = 0; i < static_cast<uint32>(sm_pContainerList.size()); ++i)
			{
				if(sm_pContainerList[i]->IsInputAllowed())
					sm_pContainerList[i]->FocusNextWidget((iMods & HYKBMOD_Shift) == 0);
			}
		}
		break;

	default:
		break;
	}

	for(uint32 i = 0; i < static_cast<uint32>(sm_pContainerList.size()); ++i)
	{
		IHyWidget *pFocusedWidget = sm_pContainerList[i]->GetFocusedWidget();
		if(pFocusedWidget && sm_pContainerList[i]->IsInputAllowed())
			pFocusedWidget->OnUiKeyboardInput(eBtn, eBtnState, iMods);
	}
}

/*static*/ void HyUiContainer::OnScroll(HyScrollBar *pSelf, float fNewPosition, float fTotalRange, void *pData)
{
	HyUiContainer *pThis = static_cast<HyUiContainer *>(pData);

	if(pSelf->GetOrientation() == HYORIENT_Vertical)
		pThis->m_RootLayout.pos.SetY(fNewPosition - fTotalRange);
	else
		pThis->m_RootLayout.pos.SetX(fNewPosition);
}
