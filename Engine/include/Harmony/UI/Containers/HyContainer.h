/**************************************************************************
*	HyContainer.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyContainer_h__
#define HyContainer_h__

#include "Afx/HyStdAfx.h"
#include "UI/IHyEntityUi.h"
#include "UI/HyPanel.h"
#include "UI/Containers/Components/HyLayout.h"
#include "UI/Widgets/IHyWidget.h"

class HyContainer : public HyEntity2d
{
	friend class HyLayout;
	friend class HyInput;

	static HyContainer *					sm_pCurModalContainer;	// If any container is considered 'modal' then only that container may accept input
	static std::vector<HyContainer *>		sm_pContainerList;

	bool									m_bInputAllowed;

protected:
	HyPanel									m_Panel;
	HyLayout								m_RootLayout;
	std::map<HyLayoutHandle, HyLayout *>	m_SubLayoutMap;
	static HyLayoutHandle					sm_hHandleCounter;

	enum ContainerState
	{
		CONTAINERSTATE_Hidden = 0,
		CONTAINERSTATE_Showing,
		CONTAINERSTATE_Shown,
		CONTAINERSTATE_Hiding
	};
	ContainerState							m_eContainerState;
	float									m_fElapsedTime;

public:
	HyContainer(HyLayoutType eRootLayout, const HyPanelInit &initRef, HyEntity2d *pParent = nullptr);
	virtual ~HyContainer();

	static bool IsModalActive();

	glm::ivec2 GetSize();
	virtual void SetSize(int32 iNewWidth, int32 iNewHeight);

	bool Show(bool bInstant = false);
	bool Hide(bool bInstant = false);
	bool IsTransition();
	bool IsShown();

	void SetAsModal();
	static void RelinquishModal();

	bool IsInputAllowed() const;
	void SetInputAllowed(bool bEnable);
	IHyWidget *GetFocusedWidget();
	IHyWidget *FocusNextWidget();

	bool AppendWidget(IHyWidget &widgetRef, HyLayoutHandle hInsertInto = HY_UNUSED_HANDLE);
	HyLayoutHandle InsertLayout(HyLayoutType eNewLayoutType, HyLayoutHandle hInsertInto = HY_UNUSED_HANDLE);
	void ClearItems();

	bool SetMargins(int16 iLeft, int16 iBottom, int16 iRight, int16 iTop, uint16 uiWidgetSpacingX, uint16 uiWidgetSpacingY, HyLayoutHandle hAffectedLayout = HY_UNUSED_HANDLE);

protected:
	virtual void OnUpdate() override final;
	std::vector<IHyWidget *> AssembleWidgetList();

	virtual void OnContainerUpdate() { }
	virtual void OnRootLayoutUpdate() { m_RootLayout.Resize(m_Panel.GetWidth(), m_Panel.GetHeight()); }

	// Optional overrides to control show and hide animations/functionality
	virtual float OnBeginShow() { return 0.0f; }	// Returns the duration (in seconds) of the show transition
	virtual void OnShown()		{ SetVisible(true); SetInputAllowed(true); }
	virtual float OnBeginHide() { return 0.0f; }	// Returns the duration (in seconds) of the hide transition
	virtual void OnHidden()		{ SetVisible(false); SetInputAllowed(false);}

private:
	bool RequestWidgetFocus(IHyWidget *pWidget);

	static void DistrubuteTextInput(std::string sText);
	static void DistrubuteKeyboardInput(HyKeyboardBtn eBtn);
};

#endif /* HyContainer_h__ */
