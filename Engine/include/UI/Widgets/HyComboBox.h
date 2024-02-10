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
	HyShape2d			m_Shape;

protected:
	enum ComboBoxAttributes
	{
		COMBOBOXATTRIB_IsExpanded		= 1 << 21,
		COMBOBOXATTRIB_IsTransition		= 1 << 22,
		COMBOBOXATTRIB_STATEMASK = (COMBOBOXATTRIB_IsExpanded | COMBOBOXATTRIB_IsTransition),

		COMBOBOXATTRIB_IsHorzExpand		= 1 << 23,
		COMBOBOXATTRIB_IsPositiveExpand = 1 << 24,
		COMBOBOXATTRIB_IsInstantExpand	= 1 << 25,

		COMBOBOXATTRIB_IsExpandMouseDwn = 1 << 26,		// While expanded, keeps track of a left mouse click (anywhere) to retract the combo box upon release
		COMBOBOXATTRIB_NeedsRetracting	= 1 << 27,		// While expanded, this flag indicates that this combo box should retract on the next update. (This allows sub btn callbacks to occur before getting disabled on the retract)

		COMBOBOXATTRIB_NEXTFLAG			= 1 << 28
	};
	static_assert((int)COMBOBOXATTRIB_IsExpanded == (int)BTNATTRIB_NEXTFLAG, "HyComboBox is not matching with base classes attrib flags");

	HyPanel								m_SubBtnPanel;
	std::vector<HyButton *>				m_SubBtnList;
	float								m_fSubBtnSpacing;

	float								m_fElapsedExpandedTime;
	float								m_fExpandedTimeout;

	std::map<HyButton *, bool>			m_SubBtnEnabledMap;

	HyAnimVec2							m_ExpandAnimVec;

public:
	HyComboBox(HyEntity2d *pParent = nullptr);
	HyComboBox(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyComboBox(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyComboBox();

	uint32 InsertSubButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyButtonClickedCallback fpCallBack, void *pParam = nullptr, const HyNodePath &audioNodePath = HyNodePath());
	uint32 InsertSubButton(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyButtonClickedCallback fpCallBack, void *pParam = nullptr, const HyNodePath &audioNodePath = HyNodePath());
	void SetSubButtonEnabled(uint32 uiSubBtnIndex, bool bEnabled);
	void RemoveSubButton(uint32 uiSubBtnIndex);
	void ClearSubButtons();

	void SetExpandPanel(const HyPanelInit &panelInit, HyOrientation eOrientation, bool bPositiveDirection, bool bAnimate);

	bool IsExpanded() const;
	bool IsTransition() const;
	void ToggleExpanded();

	void SetExpandedTimeout(float fTimeoutDuration);
	void ResetExpandedTimeout();

protected:
	virtual void Update() override;
	virtual void OnSetup() override;

	static void OnComboBoxClickedCallback(HyButton *pBtn, void *pData);
};

#endif /* HyComboBox_h__ */
