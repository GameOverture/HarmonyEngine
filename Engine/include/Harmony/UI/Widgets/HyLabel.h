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
#include "UI/Widgets/IHyWidget.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

class HyLabel : public IHyWidget
{
protected:
	class PrimPanel : public HyEntity2d
	{
	public:
		HyPrimitive2d		m_BG;				// Acts as thin stroke outline (the stroke's stroke) and a background color that isn't covered by m_Fill
		HyPrimitive2d		m_Fill;
		HyPrimitive2d		m_Stroke;
		HyPrimitive2d		m_Border;

		PrimPanel(int32 iWidth, int32 iHeight, int32 iStroke, HyEntity2d *pParent);
	};

	enum PanelAttributes
	{
		PANELATTRIB_IsPrimitive		= 1 << 0,	// Whether the panel is constructed via HyLabel::PrimPanel instead of a HySprite2d
		PANELATTRIB_HideDownState	= 1 << 1,	// Don't visually indicate down state (if available)
		PANELATTRIB_HideHoverState	= 1 << 2,	// Don't visually indicate hover state (if available)
		PANELATTRIB_HideDisabled	= 1 << 3,	// Don't visually indicate if disabled
		PANELATTRIB_IsDisabled		= 1 << 4,
		PANELATTRIB_IsHighlighted	= 1 << 5,
	};
	uint32					m_uiPanelAttribs;

	PrimPanel *				m_pPrimPanel;		// Optionally construct a primitive panel instead of using HySprite2d
	HySprite2d				m_SpritePanel;

	HyText2d				m_Text;
	HyRectangle<float>		m_TextMargins;

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

	float GetPanelWidth();
	float GetPanelHeight();
	glm::vec2 GetPanelDimensions();

	uint32 GetSpriteState() const;
	virtual void SetSpriteState(uint32 uiStateIndex);

	std::string GetUtf8String() const;
	void SetText(std::string sText);
	virtual void SetTextState(uint32 uiStateIndex);
	virtual void SetTextAlignment(HyAlignment eAlignment);
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

	virtual void ResetTextOnPanel();
};

#endif /* HyLabel_h__ */
