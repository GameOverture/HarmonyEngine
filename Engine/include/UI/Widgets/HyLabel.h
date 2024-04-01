/**************************************************************************
*	HyLabel.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLabel_h__
#define HyLabel_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/IHyWidget.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

class HyLabel : public IHyWidget
{
protected:
	enum LabelAttributes
	{
		LABELATTRIB_IsSideBySide			= 1 << 16,
		LABELATTRIB_SideBySideTextFirst		= 1 << 17,		// When 'PANELATTRIB_IsSideBySide' enabled, show the text and then the panel, otherwise vice versa
		LABELATTRIB_SideBySideVertical		= 1 << 18,		// When 'PANELATTRIB_IsSideBySide' enabled, show the panel/text above to below, otherwise left to right
		
		LABELATTRIB_StackedTextTypeMask		= 0x380000,		// When Panel is 'Stacked' (default), this mask holds HyTextType enum value.
		LABELATTRIB_StackedTextTypeOffset	= 19,			// Bit shift offset to get/set 'HyTextType'
		//									= 1 << 19
		//									= 1 << 20		// 3 bits, bit's 19-21
		//									= 1 << 21

		LABELATTRIB_NEXTFLAG				= 1 << 22
	};
	static_assert((int)LABELATTRIB_IsSideBySide == (int)WIDGETATTRIB_NEXTFLAG, "HyLabel is not matching with base classes attrib flags");

	HyText2d				m_Text;
	HyRectangle<float>		m_TextMargins;					// Margins used for Stacked text scale box
	int32					m_iSideBySidePadding;			// When set as 'Side-by-side', the pixel padding between text/panel

public:
	HyLabel(HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &panelInit, HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyLabel();

	virtual float GetWidth(float fPercent = 1.0f) override;
	virtual float GetHeight(float fPercent = 1.0f) override;

	virtual bool IsLoadDataValid() override;

	void Setup(const HyPanelInit &panelInit);
	void Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath);
	void Setup(const HyPanelInit &panelInit, const HyNodePath &textNodePath, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop);

	virtual void SetAsStacked(HyAlignment eTextAlignment = HYALIGN_Center, HyTextType eTextType = HYTEXT_ScaleBox);				// Default setup. Shows text positioned on top and inside the panel based on 'eTextAlignment' and 'eTextType'
	void SetAsSideBySide(bool bPanelBeforeText = true, int32 iPadding = 5, HyOrientation eOrientation = HYORIEN_Horizontal);	// Show the panel and text side by side specified accordingly to the arguments passed

	bool IsTextVisible() const;
	void SetTextVisible(bool bVisible);
	std::string GetUtf8String() const;
	void SetText(const std::stringstream &ssUtf8Text);
	virtual void SetText(const std::string &sUtf8Text);
	uint32 GetTextState() const;
	virtual void SetTextState(uint32 uiStateIndex);
	std::string GetTextPrefix() const;
	std::string GetTextName() const;
	virtual void SetTextLayerColor(uint32 uiStateIndex, uint32 uiLayerIndex, HyColor topColor, HyColor botColor);
	bool IsTextMonospacedDigits() const;
	virtual void SetTextMonospacedDigits(bool bSet);
	bool IsGlyphAvailable(std::string sUtf8Character);
	float GetTextWidth(float fPercent = 1.0f);
	float GetTextHeight(float fPercent = 1.0f);

	virtual glm::vec2 GetPosOffset() override;

protected:
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	virtual void OnPanelUpdated() override;

	virtual void OnSetup() { }					// Optional override for derived classes

	virtual void ResetTextAndPanel();
};

#endif /* HyLabel_h__ */
