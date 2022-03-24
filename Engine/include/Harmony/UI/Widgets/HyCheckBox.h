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
#include "UI/Widgets/HyButton.h"

class HyCheckBox : public HyButton
{
protected:
	enum CheckBoxAttributes
	{
		CHECKBOXATTRIB_IsChecked = 1 << 16,

		CHECKBOXATTRIB_FLAG_NEXT = 1 << 17
	};
	static_assert((int)CHECKBOXATTRIB_IsChecked == (int)BTNATTRIB_FLAG_NEXT, "HyCheckBox is not matching with base classes attrib flags");

	// To be drawn on panel when IsChecked
	HyPrimitive2d								m_CheckMarkStroke;
	HyPrimitive2d								m_CheckMarkFill;

	std::function<void(HyCheckBox *, void *)>	m_fpOnCheckedChanged;
	void *										m_pCheckedChangedParam;

public:
	HyCheckBox(HyEntity2d *pParent = nullptr);
	HyCheckBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyCheckBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	virtual ~HyCheckBox();

	bool IsChecked() const;
	void SetChecked(bool bChecked);

	void SetCheckedChangedCallback(std::function<void(HyCheckBox *, void *)> fpCallback, void *pParam = nullptr);

protected:
	virtual void OnSetup() override;
	virtual void ResetTextAndPanel() override;
	virtual void OnUiMouseClicked() override;

	void AssembleCheckmark();
};

#endif /* HyCheckBox_h__ */
