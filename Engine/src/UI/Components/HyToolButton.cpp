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
#include "UI/Components/HyToolButton.h"
#include "HyEngine.h"

HyToolButton::HyToolButton(HyToolButtonType eToolBtnType, uint32 uiDiameter, HyColor panelColor, HyColor frameColor, HyColor glyphColor, HyEntity2d *pParent /*= nullptr*/) :
	HyButton(pParent),
	m_uiDiameter(uiDiameter),
	m_CircleStroke(this),
	m_Circle(this),
	m_X1(this),
	m_X2(this)
{
	m_PanelColor = panelColor;
	m_FrameColor = frameColor;
	m_GlyphColor = glyphColor;

	m_CircleStroke.SetAsCircle((m_uiDiameter + 4) * 0.5f);
	m_Circle.SetAsCircle(m_uiDiameter * 0.5f);

	m_Circle.SetNumCircleSegments(20);
	m_CircleStroke.SetNumCircleSegments(20);

	float fGlyphRadius = static_cast<float>(m_uiDiameter / 4);
	switch(eToolBtnType)
	{
	case HYTOOLBTN_Close: {
		m_X1.SetAsLineSegment(glm::vec2(-fGlyphRadius, -fGlyphRadius), glm::vec2(fGlyphRadius, fGlyphRadius));
		m_X1.SetLineThickness(3);

		m_X2.SetAsLineSegment(glm::vec2(-fGlyphRadius, fGlyphRadius), glm::vec2(fGlyphRadius, -fGlyphRadius));
		m_X2.SetLineThickness(3);
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
	}

	SetColor(m_PanelColor, m_FrameColor, m_GlyphColor);
}

/*virtual*/ HyToolButton::~HyToolButton()
{
}

void HyToolButton::SetColor(HyColor panelColor, HyColor frameColor, HyColor glyphColor)
{
	m_CircleStroke.SetTint(frameColor);
	m_Circle.SetTint(panelColor);

	m_X1.SetTint(glyphColor);
	m_X2.SetTint(glyphColor);
}

/*virtual*/ void HyToolButton::OnSetSizeHint() /*override*/
{
	HySetVec(m_vSizeHint, m_uiDiameter, m_uiDiameter);
}

/*virtual*/ glm::vec2 HyToolButton::GetPosOffset() /*override*/
{
	return glm::vec2(m_uiDiameter * 0.5f, m_uiDiameter * 0.5f);
}

/*virtual*/ glm::ivec2 HyToolButton::OnResize(uint32 uiNewWidth, uint32 uiNewHeight) /*override*/
{
	OnSetSizeHint();
	return m_vSizeHint;
}
