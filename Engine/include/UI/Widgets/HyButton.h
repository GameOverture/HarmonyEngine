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
typedef std::function<void(HyButton *, void *)> HyButtonClickedCallback;

class HyButton : public HyLabel
{
protected:
	enum ButtonAttributes
	{
		BTNATTRIB_IsKbDownState		= 1 << 18,		// Indicates this button is currently receiving keyboard input that is pressing (BTNATTRIB_IsDownState) the button

		BTNATTRIB_FLAG_NEXT			= 1 << 19
	};
	static_assert((int)BTNATTRIB_IsKbDownState == (int)LABELATTRIB_FLAG_NEXT, "HyButton is not matching with base classes attrib flags");

	HyButtonClickedCallback			m_fpBtnClickedCallback;
	void *							m_pBtnClickedParam;
	HyAudio2d						m_ClickedSound;

public:
	HyButton(HyEntity2d *pParent = nullptr);
	HyButton(const HyPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyButton();

	virtual bool IsDown() const;

	void SetButtonClickedCallback(HyButtonClickedCallback fpCallBack, void *pParam = nullptr, const HyNodePath &audioNodePath = HyNodePath());
	void InvokeButtonClicked();

protected:
	virtual void OnUiMouseClicked() override;
	virtual void OnRelinquishKeyboardFocus() override;
	virtual void OnUiKeyboardInput(HyKeyboardBtn eBtn, HyBtnPressState eBtnState, HyKeyboardModifer iMods) override;

	virtual void OnSetup() override;
};

#endif /* HyButton_h__ */
