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
		COMBOBOXATTRIB_IsExpanded		= 1 << 16,
		COMBOBOXATTRIB_IsTransition		= 1 << 17,
		COMBOBOXATTRIB_STATEMASK = (COMBOBOXATTRIB_IsExpanded | COMBOBOXATTRIB_IsTransition),

		COMBOBOXATTRIB_IsHorzExpand		= 1 << 18,
		COMBOBOXATTRIB_IsPositiveExpand = 1 << 19,
		COMBOBOXATTRIB_IsInstantExpand	= 1 << 20,

		COMBOBOXATTRIB_IsExpandMouseDwn = 1 << 21,		// While expanded, keeps track of a left mouse click (anywhere) to retract the combo box upon release
		COMBOBOXATTRIB_NeedsRetracting	= 1 << 22,		// While expanded, this flag indicates that this combo box should retract on the next update. (This allows sub btn callbacks to occur before getting disabled on the retract)

		COMBOBOXATTRIB_FLAG_NEXT		= 1 << 23
	};
	static_assert((int)COMBOBOXATTRIB_IsExpanded == (int)BTNATTRIB_FLAG_NEXT, "HyComboBox is not matching with base classes attrib flags");

	HyPanel								m_SubBtnPanel;
	std::vector<HyButton *>				m_SubBtnList;
	float								m_fSubBtnSpacing;

	float								m_fElapsedExpandedTime;
	float								m_fExpandedTimeout;

public:
	HyComboBox(HyEntity2d *pParent = nullptr);
	HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyComboBox(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyComboBox();

	uint32 InsertSubButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyButtonClickedCallback fpCallBack, void *pParam = nullptr, std::string sAudioPrefix = "", std::string sAudioName = "");
	uint32 InsertSubButton(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyButtonClickedCallback fpCallBack, void *pParam = nullptr, std::string sAudioPrefix = "", std::string sAudioName = "");
	void ClearSubButtons();

	void SetExpandType(HyOrientation eOrientation, bool bPositiveDirection, bool bAnimate);
	void SetExpandPanel(std::string sSpritePrefix, std::string sSpriteName);
	void SetExpandPanel(uint32 uiFrameSize, HyColor panelColor = HyColor(0x252526), HyColor frameColor = HyColor(0x3F3F41));

	bool IsExpanded() const;
	bool IsTransition() const;
	void ToggleExpanded();

	void SetExpandedTimeout(float fTimeoutDuration);
	void ResetExpandedTimeout();

protected:
	virtual void OnUpdate() override;
	virtual void OnSetup() override;

	static void OnComboBoxClickedCallback(HyButton *pBtn, void *pData);
};

#endif /* HyComboBox_h__ */
