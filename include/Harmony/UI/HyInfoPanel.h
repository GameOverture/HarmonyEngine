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
	friend class LgSlotGame;

protected:
	HySprite2d *			m_pPanel;
	HyText2d *				m_pText;

	glm::ivec2				m_vTextOffset;

public:
	HyInfoPanel(HyEntity2d *pParent = nullptr);
	HyInfoPanel(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent);
	HyInfoPanel(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);
	HyInfoPanel(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);
	virtual ~HyInfoPanel();

	virtual void Init(const char *szPanelPrefix, const char *szPanelName, HyEntity2d *pParent);
	virtual void Init(const char *szTextPrefix, const char *szTextName, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);
	virtual void Init(const char *szPanelPrefix, const char *szPanelName, const char *szTextPrefix, const char *szTextName, int32 iTextOffsetX, int32 iTextOffsetY, int32 iTextDimensionsX, int32 iTextDimensionsY, HyEntity2d *pParent);

	HySprite2d *GetPanelPtr();
	HyText2d *GetTextPtr();

	virtual void SetPanelState(uint32 uiAnimIndex, bool bResetAnim = false);
	float GetPanelWidth();
	float GetPanelHeight();

	virtual std::string GetStr();
	virtual void SetStr(std::string sText);
	virtual void SetTextLocation(int32 iOffsetX, int32 iOffsetY, int32 iWidth, int32 iHeight);
	virtual void SetTextAlignment(HyTextAlign eAlignment);
	virtual glm::vec2 GetTextScaleBox();
};

#endif /* HyInfoPanel_h__ */
