/**************************************************************************
*	HyButton.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyButton_h__
#define HyButton_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyLabel.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"

class HyButton;
class HyButtonGroup;

class HyButton : public HyLabel
{
	friend class HyButtonGroup;

protected:
	enum ButtonAttributes
	{
		BTNATTRIB_IsKbDownState		= 1 << 22,		// Indicates this button is currently receiving keyboard input that is pressing (BTNATTRIB_IsDownState) the button
		BTNATTRIB_IsAutoExclusive	= 1 << 23,		// When ON, and this button is apart of an 'auto' HyButtonGroup, it will exclusively be checked with other 'BNTATTRIB_IsAutoExclusive' buttons
		BTNATTRIB_IsChecked			= 1 << 24,

		BTNATTRIB_NEXTFLAG			= 1 << 25
	};
	static_assert((int)BTNATTRIB_IsKbDownState == (int)LABELATTRIB_NEXTFLAG, "HyButton is not matching with base classes attrib flags");

	HyButtonGroup *						m_pButtonGroup;
	std::function<void(HyButton *)>		m_fpBtnClickedCallback;
	HyAudio2d							m_ClickedSound;

public:
	HyButton(HyEntity2d *pParent = nullptr);
	HyButton(const HyUiPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyButton(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent = nullptr);
	virtual ~HyButton();

	virtual bool IsDepressed() const override;
	
	HyButtonGroup *GetButtonGroup() const;
	bool IsAutoExclusive() const;
	bool IsChecked() const;
	void SetChecked(bool bChecked);

	void SetButtonClickedCallback(std::function<void(HyButton *)> fpCallBack, const HyNodePath &audioNodePath = HyNodePath());
	void InvokeButtonClicked();

protected:
	virtual void OnUiMouseClicked() override;
	virtual void OnRelinquishKeyboardFocus() override;
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) override;

	virtual void OnSetChecked(bool bChecked) { }
};

#endif /* HyButton_h__ */
