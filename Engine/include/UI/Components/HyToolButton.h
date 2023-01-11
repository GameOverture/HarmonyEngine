/**************************************************************************
*	HyToolButton.h
*
*	Harmony Engine
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyToolButton_h__
#define HyToolButton_h__

#include "Afx/HyStdAfx.h"
#include "UI/Widgets/HyButton.h"

enum HyToolButtonType
{
	HYTOOLBTN_Close = 0,
	HYTOOLBTN_LeftArrow,
	HYTOOLBTN_RightArrow,
	HYTOOLBTN_UpArrow,
	HYTOOLBTN_DownArrow
};

class HyToolButton : public HyButton
{
	uint32					m_uiDiameter;

	HyColor					m_PanelColor;
	HyColor					m_FrameColor;
	HyColor					m_GlyphColor;

	HyPrimitive2d			m_CircleStroke;
	HyPrimitive2d			m_Circle;

	// Glyph Primitives
	HyPrimitive2d			m_X1;
	HyPrimitive2d			m_X2;

public:
	HyToolButton(HyToolButtonType eToolBtnType, uint32 uiDiameter, HyColor panelColor, HyColor frameColor, HyColor glyphColor, HyEntity2d *pParent = nullptr);
	virtual ~HyToolButton();

	void SetColor(HyColor panelColor, HyColor frameColor, HyColor XColor);

protected:
	virtual void OnSetSizeHint() override;
	virtual glm::vec2 GetPosOffset() override;
	virtual glm::ivec2 OnResize(uint32 uiNewWidth, uint32 uiNewHeight) override;

	virtual void OnBtnStateChange(HyButtonState eNewState) override;

};

#endif /* HyToolButton_h__ */
