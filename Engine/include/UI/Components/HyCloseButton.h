/**************************************************************************
*	HyCloseButton.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyCloseButton_h__
#define HyCloseButton_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyButton.h"

class HyCloseButton : public HyButton
{
	uint32					m_uiDiameter;

	HyColor					m_PanelColor;
	HyColor					m_FrameColor;
	HyColor					m_XColor;

	HyPrimitive2d			m_CircleStroke;
	HyPrimitive2d			m_Circle;
	HyPrimitive2d			m_X1;
	HyPrimitive2d			m_X2;

public:
	HyCloseButton(uint32 uiDiameter, HyColor panelColor, HyColor frameColor, HyEntity2d *pParent);

	void SetColor(HyColor panelColor, HyColor frameColor, HyColor XColor);

protected:
	virtual void OnBtnStateChange(HyButtonState eNewState) override;

	virtual glm::vec2 GetPosOffset() override;
	virtual void OnSetSizeHint() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;
};

#endif /* HyCloseButton_h__ */
