/**************************************************************************
*	HyCloseButton.h
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Gui/Components/HyToolButton.h"
#include "HyEngine.h"

HyToolButton::HyToolButton(HyToolButtonType eToolBtnType, const HyUiPanelInit &panelInit, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(panelInit, pParent),
	m_X1(this),
	m_X2(this)
{
	const float fGlyphRadius = panel.GetWidth(0.25f);
	switch(eToolBtnType)
	{
	case HYTOOLBTN_Close: {
		m_X1.SetAsLineSegment(glm::vec2(-fGlyphRadius, -fGlyphRadius), glm::vec2(fGlyphRadius, fGlyphRadius));
		m_X1.SetLineThickness(static_cast<float>(panel.GetFrameStrokeSize()));
		m_X1.pos.Offset(fGlyphRadius * 2, fGlyphRadius * 2);

		m_X2.SetAsLineSegment(glm::vec2(-fGlyphRadius, fGlyphRadius), glm::vec2(fGlyphRadius, -fGlyphRadius));
		m_X2.SetLineThickness(static_cast<float>(panel.GetFrameStrokeSize()));
		m_X2.pos.Offset(fGlyphRadius * 2, fGlyphRadius * 2);
		break; }

	case HYTOOLBTN_LeftArrow: {
		glm::vec2 ptLeftArrow[3];
		HySetVec(ptLeftArrow[0], -fGlyphRadius, 0.0f);
		HySetVec(ptLeftArrow[1], fGlyphRadius, -fGlyphRadius);
		HySetVec(ptLeftArrow[2], fGlyphRadius, fGlyphRadius);
		for(int32 i = 0; i < 3; ++i)
			ptLeftArrow[i] += glm::vec2(fGlyphRadius * -0.25, 0.0f);
		m_X1.SetAsPolygon(ptLeftArrow, 3);
		m_X2.SetAsNothing();
		break; }

	case HYTOOLBTN_RightArrow: {
		glm::vec2 ptRightArrow[3];
		HySetVec(ptRightArrow[0], fGlyphRadius, 0.0f);
		HySetVec(ptRightArrow[1], -fGlyphRadius, fGlyphRadius);
		HySetVec(ptRightArrow[2], -fGlyphRadius, -fGlyphRadius);
		for(int32 i = 0; i < 3; ++i)
			ptRightArrow[i] += glm::vec2(fGlyphRadius * 0.25, 0.0f);
		m_X1.SetAsPolygon(ptRightArrow, 3);
		m_X2.SetAsNothing();
		break; }

	case HYTOOLBTN_UpArrow: {
		glm::vec2 ptUpArrow[3];
		HySetVec(ptUpArrow[0], 0.0f, fGlyphRadius);
		HySetVec(ptUpArrow[1], -fGlyphRadius, -fGlyphRadius);
		HySetVec(ptUpArrow[2], fGlyphRadius, -fGlyphRadius);
		for(int32 i = 0; i < 3; ++i)
			ptUpArrow[i] += glm::vec2(0.0f, fGlyphRadius * 0.25);
		m_X1.SetAsPolygon(ptUpArrow, 3);
		m_X2.SetAsNothing();
		break; }

	case HYTOOLBTN_DownArrow: {
		glm::vec2 ptDownArrow[3];
		HySetVec(ptDownArrow[0], 0.0f, -fGlyphRadius);
		HySetVec(ptDownArrow[1], -fGlyphRadius, fGlyphRadius);
		HySetVec(ptDownArrow[2], fGlyphRadius, fGlyphRadius);
		for(int32 i = 0; i < 3; ++i)
			ptDownArrow[i] += glm::vec2(0.0f, fGlyphRadius * -0.25);
		m_X1.SetAsPolygon(ptDownArrow, 3);
		m_X2.SetAsNothing();
		break; }

	default:
		HyLogError("HyToolButton recieved an invalid HyToolButtonType in its ctor");
		break;
	}

	m_X1.SetTint(panel.GetTertiaryColor());
	m_X2.SetTint(panel.GetTertiaryColor());
}

/*virtual*/ HyToolButton::~HyToolButton()
{
}
