/**************************************************************************
*	HyInfoPanel.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyInfoPanel_h__
#define HyInfoPanel_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HyText2d.h"

class HyInfoPanel : public HyEntity2d
{
protected:
	class ProcPanel : public HyEntity2d
	{
	public:
		HyPrimitive2d		m_Fill;
		HyPrimitive2d		m_Stroke;

		ProcPanel(float fWidth, float fHeight, float fStroke, HyEntity2d *pParent);
	};
	ProcPanel *				m_pProcPanel;	// Used when sprite is not specified

	HySprite2d				m_Panel;
	HyText2d				m_Text;

	bool					m_bIsDisabled;

public:
	HyInfoPanel(HyEntity2d *pParent = nullptr);
	HyInfoPanel(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyInfoPanel(float fWidth, float fHeight, float fStroke, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, HyEntity2d *pParent = nullptr);
	HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY, HyEntity2d *pParent = nullptr);
	virtual ~HyInfoPanel();

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

	bool IsDisabled() const;
	virtual void SetAsDisabled(bool bIsDisabled);

	HySprite2d &GetSpriteNode();
	HyText2d &GetTextNode();

protected:
	virtual void DoSetup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);
};

#endif /* HyInfoPanel_h__ */
