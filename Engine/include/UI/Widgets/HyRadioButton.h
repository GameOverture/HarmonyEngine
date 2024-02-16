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
	enum RadioButtonAttributes
	{
		RADIOBTNATTRIB_IsChecked = 1 << 25,

		RADIOBTNATTRIB_NEXTFLAG = 1 << 26
	};
	static_assert((int)RADIOBTNATTRIB_IsChecked == (int)BTNATTRIB_NEXTFLAG, "HyRadioButton is not matching with base classes attrib flags");

	// To be drawn on panel when IsChecked
	HyPrimitive2d									m_CheckMarkStroke;
	HyPrimitive2d									m_CheckMarkFill;

	std::function<void(HyRadioButton *, void *)>	m_fpOnCheckedChanged;
	void *											m_pCheckedChangedParam;

public:
	HyRadioButton(HyEntity2d *pParent = nullptr);
	HyRadioButton(const HyPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyRadioButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyRadioButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	virtual ~HyRadioButton();

	bool IsChecked() const;
	void SetChecked(bool bChecked);

	void SetCheckedChangedCallback(std::function<void(HyRadioButton *, void *)> fpCallback, void *pParam = nullptr);

protected:
	virtual void OnSetup() override;
	virtual void ResetTextAndPanel() override;
	virtual void OnUiMouseClicked() override;

	void AssembleCheckmark();
};

#endif /* HyRadioButton_h__ */
