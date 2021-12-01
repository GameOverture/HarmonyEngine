/**************************************************************************
*	HyLabel.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyLabel_h__
#define HyLabel_h__

#include "Afx/HyStdAfx.h"
#include "UI/HyPanel.h"
#include "UI/IHyEntityUi.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

class HyLabel : public IHyEntityUi
{
protected:
	enum LabelAttributes
	{
		LABELATTRIB_HideDownState			= 1 << 0,		// Don't visually indicate down state (when available)
		LABELATTRIB_HideHoverState			= 1 << 1,		// Don't visually indicate hover state (when available)
		LABELATTRIB_HideDisabled			= 1 << 2,		// Don't visually indicate if disabled
		LABELATTRIB_IsDisabled				= 1 << 3,
		LABELATTRIB_IsHighlighted			= 1 << 4,
		LABELATTRIB_ShowHandCursor			= 1 << 5,		// When mouse cursor hovers over panel, change to a hand icon

		LABELATTRIB_IsSideBySide			= 1 << 6,
		LABELATTRIB_SideBySideTextFirst		= 1 << 7,		// When 'PANELATTRIB_IsSideBySide' enabled, show the text and then the panel, otherwise vice versa
		LABELATTRIB_SideBySideVertical		= 1 << 8,		// When 'PANELATTRIB_IsSideBySide' enabled, show the panel/text above to below, otherwise left to right
		
		LABELATTRIB_StackedTextLeftAlign	= 1 << 9,		// When panel is stacked, use left alignment (when neither left, right, or justify, it will center)
		LABELATTRIB_StackedTextRightAlign	= 1 << 10,		// When panel is stacked, use right alignment (when neither right, left, or justify, it will center)
		LABELATTRIB_StackedTextJustifyAlign	= 1 << 11,		// When panel is stacked, use justify alignment (when neither justify, left, or right, it will center)
	};
	uint32					m_uiLabelAttribs;

	HyPanel					m_Panel;

	HyText2d				m_Text;
	HyRectangle<float>		m_TextMargins;					// Margins used for Stacked text scale box; The 'iTag' is used for Side-by-side padding between text/panel

public:
	HyLabel(HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyLabel(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyLabel();

	void Setup(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName);
	void Setup(const HyPanelInit &initRef, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop);

	virtual void SetAsStacked(HyAlignment eTextAlignment = HYALIGN_HCenter);		// Default setup. Shows text positioned on top and inside the panel based on 'eTextAlignment'
	void SetAsSideBySide(bool bPanelBeforeText = true, int32 iPadding = 5, HyOrientation eOrientation = HYORIEN_Horizontal);	// Show the panel and text side by side specified accordingly to the arguments passed

	float GetPanelWidth();
	float GetPanelHeight();
	glm::vec2 GetPanelSize();

	void  SetPanelVisible(bool bVisible);

	uint32 GetSpriteState() const;
	virtual void SetSpriteState(uint32 uiStateIndex);

	std::string GetUtf8String() const;
	void SetText(const std::stringstream &ssUtf8Text);
	void SetText(const std::string &sUtf8Text);
	virtual void SetTextState(uint32 uiStateIndex);
	virtual void SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);

	bool IsPrimitivePanel() const;

	bool IsEnabled() const;
	virtual void SetAsEnabled(bool bEnabled);

	bool IsHighlighted() const;
	virtual void SetAsHighlighted(bool bIsHighlighted);

	bool IsShowHandCursor() const;
	void SetShowHandCursor(bool bShowHandCursor);

	bool IsHideDisabled() const;				// Whether to not visually indicate if disabled
	void SetHideDisabled(bool bIsHideDisabled);	// Whether to not visually indicate if disabled

	HySprite2d &GetSpriteNode();
	HyText2d &GetTextNode();

protected:
	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;

	virtual glm::vec2 GetPosOffset() override;
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	virtual void OnSetup() { }					// Optional override for derived classes

	virtual void ResetTextAndPanel();
};

#endif /* HyLabel_h__ */
