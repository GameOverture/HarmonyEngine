/**************************************************************************
*	HyPanel.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPanel_h__
#define HyPanel_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"
#include "Scene/Nodes/Loadables/Bodies/Drawables/Objects/HySprite2d.h"

struct HyPanelInit
{
	std::string				m_sSpritePrefix;
	std::string				m_sSpriteName;

	uint32					m_uiWidth;
	uint32					m_uiHeight;
	uint32					m_uiBorderSize;

	HyColor					m_BgColor;
	HyColor					m_BorderColor;

	HyPanelInit();
	HyPanelInit(std::string sSpritePrefix, std::string sSpriteName);
	HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiBorderSize = 4, HyColor backgroundColor = HyColor(0x252526), HyColor borderColor = HyColor(0x3F3F41));
};

class HyPanel : public HyEntity2d
{
	HySprite2d				m_SpritePanel;

	// Primitive Panel
	uint32					m_uiWidth;
	uint32					m_uiHeight;
	uint32					m_uiBorderSize;
	HyPrimitive2d			m_Border;
	HyPrimitive2d			m_BG;
	HyPrimitive2d			m_Stroke;	// Acts as thin stroke around the BG always 1px in size

public:
	HyPanel(HyEntity2d *pParent = nullptr);
	HyPanel(const HyPanelInit &initRef, HyEntity2d *pParent);
	virtual ~HyPanel();

	void Setup(const HyPanelInit &initRef);
	void SetAsNull();

	bool IsValid();
	bool IsPrimitive() const;
	bool IsSprite();

	HySprite2d &GetSprite();
	uint32 GetSpriteState() const;
	void SetSpriteState(uint32 uiStateIndex);

	glm::ivec2 GetSizeHint();
	uint32 GetWidth();
	uint32 GetHeight();
	glm::ivec2 GetSize();
	void SetSize(uint32 uiWidth, uint32 uiHeight);

	glm::vec2 GetBotLeftOffset();

	HyColor GetBgColor() const;
	void SetBgColor(HyColor color);
	HyColor GetBorderColor() const;
	void SetBorderColor(HyColor color);
	uint32 GetBorderSize() const;
};

#endif /* HyPanel_h__ */
