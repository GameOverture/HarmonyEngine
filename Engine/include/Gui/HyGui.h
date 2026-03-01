/**************************************************************************
*	HyGui.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyGui_h__
#define HyGui_h__

#include "Afx/HyStdAfx.h"
#include "Gui/IHyGuiBase.h"
#include "Gui/Components/HyPanel.h"
#include "Gui/Components/HyLayout.h"
#include "Gui/Components/HyButtonGroup.h"
#include "Gui/Components/HySpacer.h"
#include "Gui/Components/HyToolButton.h"
#include "Gui/Components/HyScrollBar.h"

class HyGui : public IHyGuiBase
{
	friend class HyLayout;
	friend class HyInput;

	// Global UI Container members
	static HyGui *					sm_pCurModalContainer;	// If any container is considered 'modal' then only that container may accept input
	static std::vector<HyGui *>		sm_pContainerList;

protected:
	bool									m_bInputAllowed;
	int32									m_iDefaultWidgetSpacing;

	HyLayout								m_RootLayout;
	HyButtonGroup							m_RootBtnGrp;

	std::map<HySpacerHandle, HySpacer *>	m_SubSpacerMap;
	static HySpacerHandle					sm_hSpacerHandleCounter;

	std::map<HyLayoutHandle, HyLayout *>	m_SubLayoutMap;
	static HyLayoutHandle					sm_hLayoutHandleCounter;

	enum ContainerState
	{
		CONTAINERSTATE_Hidden = 0,
		CONTAINERSTATE_Showing,
		CONTAINERSTATE_Shown,
		CONTAINERSTATE_Hiding
	};
	ContainerState							m_eContainerState;
	float									m_fElapsedTime;

	// Scrolling functionality
	bool									m_bUseVertBar;
	bool									m_bUseHorzBar;
	HyScrollBar								m_VertBar;
	HyScrollBar								m_HorzBar;

public:
	HyPanel									panel;

public:
	HyGui(HyOrientation eRootLayoutDirection, const HyUiPanelInit &initRef, HyEntity2d *pParent = nullptr);
	virtual ~HyGui();

	static bool IsModalActive();

	bool Show(bool bInstant = false);
	bool Hide(bool bInstant = false);
	bool IsTransition();
	bool IsShown();

	void SetAsModal();
	static void RelinquishModal();

	bool IsInputAllowed() const;
	void SetInputAllowed(bool bEnable);
	IHyWidget *GetFocusedWidget();
	IHyWidget *FocusNextWidget(bool bForwardDirection);
	glm::vec2 GetWidgetPos(IHyWidget &widgetRef);

	bool InsertWidget(IHyWidget &widgetRef, HyLayoutHandle hInsertInto = HY_UNUSED_HANDLE);
	bool RemoveWidget(IHyWidget &widgetRef);

	HySpacerHandle InsertSpacer(HySizePolicy eSizePolicy = HYSIZEPOLICY_Expanding, uint32 uiSizeHint = 0, HyLayoutHandle hInsertInto = HY_UNUSED_HANDLE);
	uint32 GetSpacerSize(HySpacerHandle hSpacer);
	bool SetSpacerSize(HySpacerHandle hSpacer, HySizePolicy eSizePolicy, uint32 uiSizeHint);
	bool RemoveSpacer(HySpacerHandle hSpacer);

	HyLayoutHandle InsertLayout(HyOrientation eNewLayoutType, HyLayoutHandle hInsertInto = HY_UNUSED_HANDLE);
	glm::ivec2 GetLayoutSize(HyLayoutHandle hLayout) const;
	bool SetLayoutMargin(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, HyLayoutHandle hAffectedLayout = HY_UNUSED_HANDLE);
	bool SetLayoutWidgetSpacing(int32 iWidgetSpacing, HyLayoutHandle hAffectedLayout = HY_UNUSED_HANDLE);
	bool RemoveLayout(HyLayoutHandle hLayout);

	int32 GetDefaultWidgetSpacing() const;
	void SetDefaultWidgetSpacing(int32 iSpacing, bool bSetRootLayout);
	
	void ClearItems();

	void EnableScrollBars(bool bUseVert, bool bUseHorz);
	//void SetScrollBarColor(HyColor color);
	//void SetScrollBarSize(uint32 uiDiameter);
	void SetLineScrollAmt(float fLineScrollAmt);
	void ScrollTo(float fVertScrollPos, float fHorzScrollPos);

protected:
	virtual void OnUpdate() override final;
	virtual glm::ivec2 OnCalcPreferredSize() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	std::vector<IHyWidget *> AssembleWidgetList();

	virtual void OnContainerUpdate() { }

	// Optional overrides to control show and hide animations/functionality
	virtual float OnBeginShow() { return 0.0f; }	// Returns the duration (in seconds) of the show transition
	virtual void OnShown()		{ SetVisible(true); SetInputAllowed(true); }
	virtual float OnBeginHide() { return 0.0f; }	// Returns the duration (in seconds) of the hide transition
	virtual void OnHidden()		{ SetVisible(false); SetInputAllowed(false);}

private:
	bool RequestWidgetFocus(IHyWidget *pWidget);

	static void DistrubuteTextInput(std::string sText);
	static void DistrubuteKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods);

	static void OnScroll(HyScrollBar *pSelf, float fNewPosition, float fTotalRange, void *pData);
};

#endif /* HyGui_h__ */
