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
	uint32					m_uiFrameSize;

	HyColor					m_PanelColor;
	HyColor					m_FrameColor;

	enum PanelType
	{
		PANELTYPE_Null = 0,
		PANELTYPE_Sprite,
		PANELTYPE_Primitive
	};
	PanelType				m_ePanelType;

	HyPanelInit();
	HyPanelInit(std::string sSpritePrefix, std::string sSpriteName);
	HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize = 4, HyColor panelColor = HyColor(0x252526), HyColor frameColor = HyColor(0x3F3F41));
};

class HyPanel : public HyEntity2d
{
	glm::ivec2				m_vSize;
	uint32					m_uiFrameSize;
	HyPanelInit::PanelType	m_ePanelType;

	HySprite2d				m_SpritePanel;

	HyPrimitive2d			m_Frame1;
	HyPrimitive2d			m_Frame2;	// When thicc enough, a frame can have two tones to it
	HyPrimitive2d			m_Panel;

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
	uint32 GetFrameSize() const;

	glm::vec2 GetBotLeftOffset();

	HyColor GetPanelColor() const;
	void SetPanelColor(HyColor color);
	HyColor GetFrameColor() const;
	void SetFrameColor(HyColor color);

protected:
	void ConstructPrimitives();
};

#endif /* HyPanel_h__ */
