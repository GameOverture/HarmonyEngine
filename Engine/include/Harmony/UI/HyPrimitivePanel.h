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

class HyPrimitivePanel : public HyEntity2d
{
	HyPrimitive2d		m_Border;
	HyPrimitive2d		m_BG;
	HyPrimitive2d		m_Stroke;	// Acts as thin stroke around the BG always 1px in size
	
	glm::ivec2			m_vSize;
	uint32				m_uiBorderSize;

public:
	HyPrimitivePanel(uint32 uiWidth, uint32 uiHeight, uint32 uiBorderSize, HyEntity2d *pParent);
	virtual ~HyPrimitivePanel();

	HyAnimVec3 &BgColorTop();
	HyAnimVec3 &BgColorBot();
	void SetBgColor(float fR, float fG, float fB);
	void SetBgColor(uint32 uiColor);

	HyAnimVec3 &BorderColorTop();
	HyAnimVec3 &BorderColorBot();
	void SetBorderColor(float fR, float fG, float fB);
	void SetBorderColor(uint32 uiColor);

	glm::ivec2 GetSize() const;
	void SetSize(uint32 uiWidth, uint32 uiHeight);

	uint32 GetBorderSize() const;
	void SetBorderSize(uint32 uiBorderSize);
};

#endif /* HyPrimitivePanel_h__ */
