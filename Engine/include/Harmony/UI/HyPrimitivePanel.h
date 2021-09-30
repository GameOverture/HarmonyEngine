/**************************************************************************
*	HyPrimitivePanel.h
*
*	Harmony Engine
*	Copyright (c) 2021 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPrimitivePanel_h__
#define HyPrimitivePanel_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Bodies/Objects/HyEntity2d.h"

struct HyPrimitivePanelInit
{
	uint32					m_uiWidth;
	uint32					m_uiHeight;
	uint32					m_uiBorderSize;

	HyColor					m_BgColor;
	HyColor					m_BorderColor;

	HyPrimitivePanelInit(uint32 uiWidth, uint32 uiHeight, uint32 uiBorderSize = 4, HyColor backgroundColor = HyColor(0x252526), HyColor borderColor = HyColor(0x3F3F41));
};

class HyPrimitivePanel : public HyEntity2d
{
	HyPrimitive2d			m_Border;
	HyPrimitive2d			m_BG;
	HyPrimitive2d			m_Stroke;	// Acts as thin stroke around the BG always 1px in size
	
	HyPrimitivePanelInit	m_Data;		// This structure is always kept up to date with current settings

public:
	HyPrimitivePanel(const HyPrimitivePanelInit &initRef, HyEntity2d *pParent);
	virtual ~HyPrimitivePanel();

	uint32 GetWidth() const;
	uint32 GetHeight() const;

	HyColor GetBgColor() const;
	void SetBgColor(HyColor color);

	HyColor GetBorderColor() const;
	void SetBorderColor(HyColor color);

	glm::ivec2 GetSize() const;
	void SetSize(uint32 uiWidth, uint32 uiHeight);

	uint32 GetBorderSize() const;
	void SetBorderSize(uint32 uiBorderSize);
};

#endif /* HyPrimitivePanel_h__ */
