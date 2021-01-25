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
#include "Scene/Nodes/Loadables/Drawables/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HySprite2d.h"
#include "Scene/Nodes/Loadables/Drawables/Instances/Objects/HyText2d.h"

class HyInfoPanel : public HyEntity2d
{
protected:
	HySprite2d				m_Panel;
	HyText2d				m_Text;

	glm::ivec2				m_vTextOffset;

public:
	HyInfoPanel();
	HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName);
	HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY);
	HyInfoPanel(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);
	virtual ~HyInfoPanel();

	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName);
	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY);
	virtual void Setup(std::string sPanelPrefix, std::string sPanelName, std::string sTextPrefix, std::string sTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, int32 iTextOffsetX, int32 iTextOffsetY);

	HySprite2d &GetPanel();
	HyText2d &GetText();

	virtual void SetPanelState(uint32 uiAnimIndex, bool bResetAnim = false);
	float GetPanelWidth();
	float GetPanelHeight();

	virtual std::string GetStr();
	virtual void SetStr(std::string sText);
	virtual void SetTextLocation(int32 iWidth, int32 iHeight, int32 iOffsetX, int32 iOffsetY);
	virtual void SetTextAlignment(HyTextAlign eAlignment);
	virtual glm::vec2 GetTextScaleBox();
};

#endif /* HyInfoPanel_h__ */
