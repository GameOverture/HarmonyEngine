/**************************************************************************
*	HyRadioButton.h
*
*	Harmony Engine
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyRadioButton_h__
#define HyRadioButton_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyButton.h"

class HyRadioButton : public HyButton
{
protected:
	// To be drawn on panel when IsChecked
	HyPrimitive2d									m_CheckMarkStroke;
	HyPrimitive2d									m_CheckMarkFill;

	std::function<void(HyRadioButton *)>			m_fpOnCheckedChanged;

public:
	HyRadioButton(HyEntity2d *pParent = nullptr);
	HyRadioButton(const HyPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyRadioButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyRadioButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, const HyMargins<float> &textMargins, HyEntity2d *pParent = nullptr);
	virtual ~HyRadioButton();

	void SetCheckedChangedCallback(std::function<void(HyRadioButton *)> fpCallback);

protected:
	virtual void OnSetup() override;
	virtual void OnAssemble() override;

	virtual void OnUiMouseClicked() override;

	virtual void OnSetChecked(bool bChecked) override;
};

#endif /* HyRadioButton_h__ */
