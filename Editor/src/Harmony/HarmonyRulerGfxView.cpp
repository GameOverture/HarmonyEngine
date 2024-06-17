/**************************************************************************
*	HarmonyRulerGfxView.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2024 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "HarmonyRulerGfxView.h"
#include "HarmonyWidget.h"

#include <QPainter>
#include <QGraphicsLineItem>

HarmonyRulerGfxView::HarmonyRulerGfxView(QWidget *pParent /*= nullptr*/) :
	QGraphicsView(pParent),
	m_eOrientation(HYORIENT_Horizontal),
	m_iDrawStart(0),
	m_iDrawEnd(0),
	m_iDrawWidth(0),
	m_iWorldStart(0),
	m_iWorldEnd(0),
	m_iWorldWidth(0),
	m_bShowMouse(false)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

/*virtual*/ HarmonyRulerGfxView::~HarmonyRulerGfxView()
{
}

void HarmonyRulerGfxView::Init(HyOrientation eOrientation)
{
	m_eOrientation = eOrientation;
	setBackgroundBrush(HyGlobal::GetEditorQtColor(EDITORCOLOR_Rulers));
}

bool HarmonyRulerGfxView::IsShowMouse() const
{
	return m_bShowMouse;
}

void HarmonyRulerGfxView::ShowMouse(bool bShowMouse)
{
	m_bShowMouse = bShowMouse;
}

/*virtual*/ void HarmonyRulerGfxView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
{
	QGraphicsView::drawForeground(pPainter, rect);

	if(HyEngine::IsInitialized() == false)
		return;
	
	HarmonyWidget *pHarmonyWidget = static_cast<HarmonyWidget *>(parent());
	if(pHarmonyWidget == nullptr || pHarmonyWidget->GetProject() == nullptr)
		return;

	if(m_eOrientation == HYORIENT_Horizontal)
	{
		pPainter->setPen(QPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersNotch)));
		pPainter->drawLine(rect.left(), rect.top(), rect.right(), rect.top());
	}

	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);

	b2AABB worldViewBounds;
	pCamera->CalcWorldViewBounds(worldViewBounds);

	qreal fSubLineSpacing = 4 * pCamera->GetZoom();
	pPainter->setPen(QPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersText)));
	pPainter->setFont(QFont("Arial", 7));

	int iCurDrawPos = 0;
	int iCurWorldPos = 0;
	if(m_eOrientation == HYORIENT_Horizontal)
	{
		iCurDrawPos = static_cast<int>(rect.left());
		iCurDrawPos += static_cast<int>(RULER_WIDTH);
		m_iDrawStart = iCurDrawPos;
		m_iDrawEnd = static_cast<int>(rect.right());
		m_iDrawWidth = m_iDrawEnd - m_iDrawStart;

		iCurWorldPos = static_cast<int>(worldViewBounds.lowerBound.x);
		m_iWorldStart = iCurWorldPos;
		m_iWorldEnd = static_cast<int>(worldViewBounds.upperBound.x);
		m_iWorldWidth = m_iWorldEnd - m_iWorldStart;
	}
	else // HYORIENT_Vertical
	{
		iCurDrawPos = static_cast<int>(rect.top());
		m_iDrawStart = iCurDrawPos;
		m_iDrawEnd = static_cast<int>(rect.bottom());
		m_iDrawWidth = m_iDrawEnd - m_iDrawStart;

		iCurWorldPos = static_cast<int>(worldViewBounds.upperBound.y);
		m_iWorldStart = iCurWorldPos;
		m_iWorldEnd = static_cast<int>(worldViewBounds.lowerBound.y);
		m_iWorldWidth = m_iWorldStart - m_iWorldEnd;
	}

	iCurWorldPos = HyMath::RoundToNearest(iCurWorldPos, 25);
	iCurDrawPos = ConvertWorldToDraw(iCurWorldPos);

	while(iCurDrawPos < m_iDrawEnd)
	{
		if(iCurDrawPos > m_iDrawStart)
		{
			// Draw a notch line on every global position that is a multiple of 100
			if(iCurWorldPos % 100 == 0)
			{
				QString sNotch = QString::number(iCurWorldPos);
				const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(sNotch);

				if(m_eOrientation == HYORIENT_Horizontal)
				{
					// Main Notch Keyframe Text
					QRectF textRect(iCurDrawPos - (fTextWidth * 0.5f) - 1.0f, rect.top(), fTextWidth + 2.0f, RULER_TEXT_HEIGHT + 2.0f);
					textRect.translate(1.0f, 1.0f);
					pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersTextShadow));
					pPainter->drawText(textRect, sNotch);
					textRect.translate(-1.0f, -1.0f);
					pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersText));
					pPainter->drawText(textRect, sNotch);

					// Draw notch line
					pPainter->setPen(QPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersNotch)));
					pPainter->drawLine(iCurDrawPos, rect.bottom(), iCurDrawPos, rect.bottom() - RULER_MAIN_NOTCH);
				}
				else // HYORIENT_Vertical
				{
					// Main Notch Keyframe Text
					pPainter->rotate(-90.0f);
					pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersTextShadow));
					pPainter->drawText(QPointF(-1 * (iCurDrawPos + 1.0f) - (fTextWidth * 0.5f), RULER_WIDTH + 1.0f - (RULER_WIDTH - RULER_TEXT_HEIGHT)), sNotch);
					pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersText));
					pPainter->drawText(QPointF(-1 * iCurDrawPos - (fTextWidth * 0.5f), RULER_WIDTH - (RULER_WIDTH - RULER_TEXT_HEIGHT)), sNotch);
					pPainter->rotate(90.0f);

					// Draw notch line
					pPainter->setPen(QPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersNotch)));
					pPainter->drawLine(rect.right(), iCurDrawPos, rect.right() - RULER_MAIN_NOTCH, iCurDrawPos);
				}
			}
			else if(iCurWorldPos % 25 == 0)
			{
				// Draw sub-notch line
				if(m_eOrientation == HYORIENT_Horizontal)
					pPainter->drawLine(iCurDrawPos, rect.bottom(), iCurDrawPos, rect.bottom() - RULER_SUB_NOTCH);
				else
					pPainter->drawLine(rect.right(), iCurDrawPos, rect.right() - RULER_SUB_NOTCH, iCurDrawPos);
			}
		}

		if(m_eOrientation == HYORIENT_Horizontal)
			iCurWorldPos += 25;
		else
			iCurWorldPos -= 25;
		iCurDrawPos = ConvertWorldToDraw(iCurWorldPos);
	}
	
	glm::vec2 ptWorldMousePos;
	bool bWorldMousePosValid = HyEngine::Input().GetWorldMousePos(ptWorldMousePos);
	if(bWorldMousePosValid && m_bShowMouse)
	{
		pPainter->setPen(QPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_RulersMousePos)));

		if(m_eOrientation == HYORIENT_Horizontal)
			pPainter->drawLine(ConvertWorldToDraw(static_cast<int>(ptWorldMousePos.x)), rect.top(), ConvertWorldToDraw(static_cast<int>(ptWorldMousePos.x)), rect.bottom());
		else // HYORIENT_Vertical
			pPainter->drawLine(rect.left(), ConvertWorldToDraw(static_cast<int>(ptWorldMousePos.y)), rect.right(), ConvertWorldToDraw(static_cast<int>(ptWorldMousePos.y)));
	}
}

/*virtual*/ void HarmonyRulerGfxView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{	
	static_cast<HarmonyWidget *>(parent())->OnRulerMouseMoveEvent(m_eOrientation, pEvent); // Pass this event to HarmonyWidget to manage the logic for placing guides
	QGraphicsView::mouseMoveEvent(pEvent);
}

/*virtual*/ void HarmonyRulerGfxView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	static_cast<HarmonyWidget *>(parent())->OnRulerMousePressEvent(m_eOrientation, pEvent); // Pass this event to HarmonyWidget to manage the logic for placing guides
	QGraphicsView::mousePressEvent(pEvent);
}

/*virtual*/ void HarmonyRulerGfxView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	static_cast<HarmonyWidget *>(parent())->OnRulerMouseReleaseEvent(m_eOrientation, pEvent); // Pass this event to HarmonyWidget to manage the logic for placing guides
	QGraphicsView::mouseReleaseEvent(pEvent);
}

float HarmonyRulerGfxView::ConvertWorldToDraw(float fWorldPos)
{
	float fPercent = static_cast<float>(fWorldPos - m_iWorldStart) / static_cast<float>(m_iWorldWidth);
	if(m_eOrientation == HYORIENT_Horizontal)
		return m_iDrawStart + (fPercent * m_iDrawWidth);
	else
		return m_iDrawStart - (fPercent * m_iDrawWidth);
}

float HarmonyRulerGfxView::ConvertDrawToWorld(float fDrawPos)
{
	float fPercent = static_cast<float>(fDrawPos - m_iDrawStart) / static_cast<float>(m_iDrawWidth);
	return m_iWorldStart + (fPercent * m_iWorldWidth);
}
