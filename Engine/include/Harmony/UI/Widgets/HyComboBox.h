/**************************************************************************
*	HyComboBox.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyComboBox_h__
#define HyComboBox_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyButton.h"

class HyComboBox : public HyButton
{
protected:
	enum ComboBoxAttributes
	{
		COMBOBOXATTRIB_IsExpanded		= 1 << 14,
		COMBOBOXATTRIB_IsTransition		= 1 << 15,
		COMBOBOXATTRIB_STATEMASK = (COMBOBOXATTRIB_IsExpanded | COMBOBOXATTRIB_IsTransition),

		COMBOBOXATTRIB_IsHorzExpand		= 1 << 16,
		COMBOBOXATTRIB_IsPositiveExpand = 1 << 17,
		COMBOBOXATTRIB_IsInstantExpand	= 1 << 18,

		COMBOBOXATTRIB_FLAG_NEXT		= 1 << 19
	};
	static_assert((int)COMBOBOXATTRIB_IsExpanded == (int)BTNATTRIB_FLAG_NEXT, "HyComboBox is not matching with base classes attrib flags");

	HyPanel						m_SubBtnPanel;
	std::vector<HyButton *>		m_SubBtnList;
	float						m_fSubBtnSpacing;

	float						m_fElapsedExpandedTime;
	float						m_fExpandedTimeout;

public:
	HyComboBox(HyEntity2d *pParent = nullptr);
	HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyComboBox();

	uint32 InsertSubButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyButtonClickedCallback fpCallBack, void *pParam = nullptr, std::string sAudioPrefix = "", std::string sAudioName = "");
	uint32 InsertSubButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyButtonClickedCallback fpCallBack, void *pParam = nullptr, std::string sAudioPrefix = "", std::string sAudioName = "");
	void ClearSubButtons();

	void SetExpandType(HyOrientation eOrientation, bool bPositiveDirection, bool bAnimate);
	void SetExpandPanel(const HyPanelInit &panelInit);

	bool IsExpanded() const;
	bool IsTransition() const;
	void ToggleExpanded();

	void SetExpandedTimeout(float fTimeoutDuration);
	void ResetExpandedTimeout();

	HyButton *GetSubBtn(uint32 uiIndex);

protected:
	virtual void OnUpdate() override;
	virtual void OnSetup() override;
};

#endif /* HyComboBox_h__ */
