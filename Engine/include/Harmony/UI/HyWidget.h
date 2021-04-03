/**************************************************************************
*	HyWidget.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyWidget_h__
#define HyWidget_h__

#include "Afx/HyStdAfx.h"
#include "UI/Layouts/IHyLayoutItem.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

class HyWidget : public IHyLayoutItem
{
protected:
	class PrimPanel : public HyEntity2d
	{
	public:
		HyPrimitive2d		m_Fill;
		HyPrimitive2d		m_Stroke;

		PrimPanel(float fWidth, float fHeight, float fStroke, HyEntity2d *pParent);
	};
	PrimPanel *				m_pPrimPanel;			// Optionally construct a primitive panel instead of using HySprite2d
	HySprite2d				m_SpritePanel;
	HyText2d				m_Text;

	enum InfoPanelAttributes
	{
		INFOPANELATTRIB_IsPrimitive = 1 << 0,		// Whether the panel is constructed via HyPrimitive2d's instead of a HySprite2d
		INFOPANELATTRIB_HideDownState = 1 << 1,		// Don't visually indicate down state (if available)
		INFOPANELATTRIB_HideHoverState = 1 << 2,	// Don't visually indicate hover state (if available)
		INFOPANELATTRIB_HideDisabled = 1 << 3,		// Don't visually indicate if disabled
		INFOPANELATTRIB_IsDisabled = 1 << 4,
		INFOPANELATTRIB_IsHighlighted = 1 << 5,
	};
	uint32					m_uiInfoPanelAttribs;

public:
	HyWidget(HyEntity2d *pParent = nullptr);
	HyWidget(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyWidget(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	HyWidget(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyWidget(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	virtual ~HyWidget();

	void Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName);
	void Setup(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);
	void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName);
	void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);

	float GetPanelWidth();
	float GetPanelHeight();

	uint32 GetSpriteState() const;
	virtual void SetSpriteState(uint32 uiStateIndex);

	std::string GetText() const;
	void SetText(std::string sText);
	virtual void SetTextState(uint32 uiStateIndex);
	virtual void SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY);
	virtual void SetTextAlignment(HyTextAlign eAlignment);
	virtual void SetTextLayerColor(uint32 uiLayerIndex, float fR, float fG, float fB);

	bool IsPrimitivePanel() const;

	bool IsDisabled() const;
	virtual void SetAsDisabled(bool bIsDisabled);

	bool IsHighlighted() const;
	virtual void SetAsHighlighted(bool bIsHighlighted);

	bool IsHideDisabled() const;				// Whether to not visually indicate if disabled
	void SetHideDisabled(bool bIsHideDisabled);	// Whether to not visually indicate if disabled

	HyEntity2d *GetPrimitiveNode();
	HySprite2d &GetSpriteNode();
	HyText2d &GetTextNode();

protected:
	virtual void OnSetup(std::string sPanelPrefix, std::string sPanelName,
						 std::string sTextPrefix, std::string sTextName,
						 int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);

	virtual void OnResize(int32 iNewWidth, int32 iNewHeight);
};

#endif /* HyWidget_h__ */
