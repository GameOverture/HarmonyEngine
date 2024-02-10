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
		LABELATTRIB_IsSideBySide			= 1 << 13,
		LABELATTRIB_SideBySideTextFirst		= 1 << 14,		// When 'PANELATTRIB_IsSideBySide' enabled, show the text and then the panel, otherwise vice versa
		LABELATTRIB_SideBySideVertical		= 1 << 15,		// When 'PANELATTRIB_IsSideBySide' enabled, show the panel/text above to below, otherwise left to right
		
		LABELATTRIB_StackedTextLeftAlign	= 1 << 16,		// When panel is stacked, use left alignment (when neither left, right, or justify, it will center)
		LABELATTRIB_StackedTextRightAlign	= 1 << 17,		// When panel is stacked, use right alignment (when neither right, left, or justify, it will center)
		LABELATTRIB_StackedTextJustifyAlign	= 1 << 18,		// When panel is stacked, use justify alignment (when neither justify, left, or right, it will center)
		LABELATTRIB_StackedTextUseLine		= 1 << 19,		// When panel is stacked, use standard text line located at bot left margin, instead of scale box
		
		LABELATTRIB_NEXTFLAG				= 1 << 20
	};
	static_assert((int)LABELATTRIB_IsSideBySide == (int)WIDGETATTRIB_NEXTFLAG, "HyLabel is not matching with base classes attrib flags");

	HyText2d				m_Text;
	HyRectangle<float>		m_TextMargins;					// Margins used for Stacked text scale box; The 'iTag' is used for Side-by-side padding between text/panel

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

	virtual void SetAsStacked(HyAlignment eTextAlignment = HYALIGN_Center, bool bUseScaleBox = true);							// Default setup. Shows text positioned on top and inside the panel based on 'eTextAlignment' and 'bUseScaleBox'
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
