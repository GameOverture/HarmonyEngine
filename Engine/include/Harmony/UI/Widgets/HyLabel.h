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
#include "UI/HyPrimitivePanel.h"
#include "UI/Widgets/IHyWidget.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

class HyLabel : public IHyWidget
{
protected:
	enum PanelAttributes
	{
		PANELATTRIB_IsPrimitive				= 1 << 0,		// Whether the panel is constructed via HyPrimitivePanel instead of a HySprite2d
		PANELATTRIB_HideDownState			= 1 << 1,		// Don't visually indicate down state (when available)
		PANELATTRIB_HideHoverState			= 1 << 2,		// Don't visually indicate hover state (when available)
		PANELATTRIB_HideDisabled			= 1 << 3,		// Don't visually indicate if disabled
		PANELATTRIB_IsDisabled				= 1 << 4,
		PANELATTRIB_IsHighlighted			= 1 << 5,

		PANELATTRIB_IsSideBySide			= 1 << 6,
		PANELATTRIB_SideBySideTextFirst		= 1 << 7,		// When 'PANELATTRIB_IsSideBySide' enabled, show the text and then the panel, otherwise vice versa
		PANELATTRIB_SideBySideVertical		= 1 << 8,		// When 'PANELATTRIB_IsSideBySide' enabled, show the panel/text above to below, otherwise left to right

		PANELATTRIB_StackedTextLeftAlign	= 1 << 9,		// When panel is stacked, use left alignment (when neither left, right, or justify, it will center)
		PANELATTRIB_StackedTextRightAlign	= 1 << 10,		// When panel is stacked, use right alignment (when neither right, left, or justify, it will center)
		PANELATTRIB_StackedTextJustifyAlign	= 1 << 11,		// When panel is stacked, use justify alignment (when neither justify, left, or right, it will center)
	};
	uint32					m_uiPanelAttribs;

	HyPrimitivePanel *		m_pPrimPanel;					// Optionally construct a primitive panel instead of using HySprite2d
	HySprite2d				m_SpritePanel;

	HyText2d				m_Text;
	HyRectangle<float>		m_TextMargins;					// Margins used for Stacked text scale box; The 'iTag' is used for Side-by-side padding between text/panel

public:
	HyLabel(HyEntity2d *pParent = nullptr);
	HyLabel(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyLabel(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyLabel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop, HyEntity2d *pParent = nullptr);
	virtual ~HyLabel();

	void Setup(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName);
	void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName);
	void Setup(int32 iWidth, int32 iHeight, int32 iStroke, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop);
	void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextMarginLeft, int32 iTextMarginBottom, int32 iTextMarginRight, int32 iTextMarginTop);

	virtual void SetAsStacked(HyAlignment eTextAlignment = HYALIGN_HCenter);		// Default setup. Shows text positioned on top and inside the panel based on 'eTextAlignment'
	void SetAsSideBySide(bool bPanelBeforeText = true, int32 iPadding = 5, HyOrientation eOrientation = HYORIEN_Horizontal);	// Show the panel and text side by side specified accordingly to the arguments passed

	float GetPanelWidth();
	float GetPanelHeight();
	glm::vec2 GetPanelDimensions();

	uint32 GetSpriteState() const;
	virtual void SetSpriteState(uint32 uiStateIndex);

	std::string GetUtf8String() const;
	void SetText(std::string sText);
	virtual void SetTextState(uint32 uiStateIndex);
	virtual void SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);

	bool IsPrimitivePanel() const;

	bool IsEnabled() const;
	virtual void SetAsEnabled(bool bEnabled);

	bool IsHighlighted() const;
	virtual void SetAsHighlighted(bool bIsHighlighted);

	bool IsHideDisabled() const;				// Whether to not visually indicate if disabled
	void SetHideDisabled(bool bIsHideDisabled);	// Whether to not visually indicate if disabled

	HyEntity2d *GetPrimitiveNode();
	HySprite2d &GetSpriteNode();
	HyText2d &GetTextNode();

protected:
	void CommonSetup();
	virtual void OnSetup() { }					// Optional override for derived classes

	virtual glm::ivec2 GetSizeHint() override;
	virtual glm::vec2 GetPosOffset() override;
	virtual void OnResize(int32 iNewWidth, int32 iNewHeight) override;

	virtual void ResetTextAndPanel();
};

#endif /* HyLabel_h__ */
