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

	// Whatever PanelType is chosen will affect UI layout positioning and metrics
	enum PanelType
	{
		PANELTYPE_BoundingVolume = 0,	// The panel isn't visibly seen, but can still have a width and height
		PANELTYPE_Sprite,				// The panel metrics are dictated by the HySprite
		PANELTYPE_Primitive				// The panel is assembled in this class using primitives
	};
	PanelType				m_ePanelType;

	// Constructs a 'BoundingVolume' panel with 0 width/height
	HyPanelInit();

	// Constructs a 'BoundingVolume' panel
	HyPanelInit(uint32 uiWidth, uint32 uiHeight);

	// Constructs a 'Sprite' panel
	HyPanelInit(std::string sSpritePrefix, std::string sSpriteName);

	// Constructs a 'Primitive' panel
	HyPanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiFrameSize, HyColor panelColor = HyColor(0x252526), HyColor frameColor = HyColor(0x3F3F41));
};

class HyPanel : public HyEntity2d
{
	HyPanelInit::PanelType	m_ePanelType;
	glm::ivec2				m_vSize;

	HySprite2d				m_SpritePanel;

	uint32					m_uiFrameSize;
	HyPrimitive2d			m_Frame1;
	HyPrimitive2d			m_Frame2;	// When thicc enough, a frame can have two tones to it
	HyPrimitive2d			m_Panel;

public:
	HyPanel(HyEntity2d *pParent = nullptr);
	HyPanel(const HyPanelInit &initRef, HyEntity2d *pParent);
	virtual ~HyPanel();

	void Setup(const HyPanelInit &initRef);
	bool IsValid();

	bool IsBoundingVolume() const;
	bool IsPrimitive() const;
	bool IsSprite() const;

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
