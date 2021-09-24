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
#include "UI/HyEntityUi.h"
#include "UI/HyPrimitivePanel.h"
#include "UI/Layouts/IHyLayout.h"

class HyContainer : public HyEntityUi
{
	friend class IHyLayout;

protected:
	IHyLayout *				m_pRootLayout;
	HyPrimitivePanel *		m_pPrimPanel;

	enum ContainerState
	{
		CONTAINERSTATE_Hidden = 0,
		CONTAINERSTATE_Showing,
		CONTAINERSTATE_Shown,
		CONTAINERSTATE_Hiding
	};
	ContainerState			m_eContainerState;
	float					m_fElapsedTime;

public:
	HyContainer(HyLayoutType eRootLayout, HyEntity2d *pParent = nullptr);
	HyContainer(HyLayoutType eRootLayout, const HyPrimitivePanelInit &initRef, HyEntity2d *pParent = nullptr);
	virtual ~HyContainer();

	glm::ivec2 GetSize() const;
	void SetSize(int32 iNewWidth, int32 iNewHeight);

	bool Show(bool bInstant = false);
	bool Hide(bool bInstant = false);

	bool IsTransition();
	bool IsShown();

	void AppendItem(HyEntityUi &itemRef);
	void ClearItems();

	//IHyLayout *GetRootLayout();
	//template<typename LAYOUT>
	//LAYOUT *GetRootLayout() {
	//	return static_cast<LAYOUT *>(m_pRootLayout);
	//}

protected:
	virtual void OnUpdate() override final;
	virtual void OnContainerUpdate() { }
	virtual void OnSetLayoutItems() { }

	// Optional overrides to control show and hide animations/functionality
	virtual float OnBeginShow() { return 0.0f; }	// Returns the duration (in seconds) of the show transition
	virtual void OnShown()		{ SetVisible(true); }
	virtual float OnBeginHide() { return 0.0f; }	// Returns the duration (in seconds) of the hide transition
	virtual void OnHidden()		{ SetVisible(false); }

	void AllocRootLayout(HyLayoutType eRootLayout);

private:
	virtual glm::ivec2 GetSizeHint() override;
};

#endif /* HyContainer_h__ */
