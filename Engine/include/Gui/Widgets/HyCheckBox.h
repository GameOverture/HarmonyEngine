/**************************************************************************
*	HyCheckBox.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyCheckBox_h__
#define HyCheckBox_h__

#include "Afx/HyStdAfx.h"
#include "Gui/Widgets/HyButton.h"

class HyCheckBox : public HyButton
{
protected:
	// To be drawn on panel when IsChecked
	HyPrimitive2d								m_CheckMarkStroke;
	HyPrimitive2d								m_CheckMarkFill;

	std::function<void(HyCheckBox *)>			m_fpOnCheckedChanged;

public:
	HyCheckBox(HyEntity2d *pParent = nullptr);
	HyCheckBox(const HyUiPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyCheckBox(const HyUiPanelInit &panelInit, const HyUiTextInit &textInit, HyEntity2d *pParent = nullptr);
	virtual ~HyCheckBox();

	void SetCheckedChangedCallback(std::function<void(HyCheckBox *)> fpCallback);

protected:
	virtual void OnSetup() override;
	virtual void OnAssemble() override;

	virtual void OnUiMouseClicked() override;

	virtual void OnSetChecked(bool bChecked) override;
};

#endif /* HyCheckBox_h__ */
