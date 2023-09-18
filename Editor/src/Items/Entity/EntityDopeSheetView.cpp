/**************************************************************************
*	EntityDopeSheetView.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDopeSheetView.h"
#include "EntityDopeSheetScene.h"

#include <QPainter>
#include <QScrollBar>

#define TIMELINE_HEIGHT 38.0f
#define TIMELINE_LEFT_MARGIN 250.0f
#define TIMELINE_NOTCH_WIDTH 88.0f
#define TIMELINE_NOTCH_MAINLINE_HEIGHT 15.0f
#define TIMELINE_NOTCH_SUBLINES_HEIGHT 9.0f
#define TIMELINE_NOTCH_SUBLINES_WIDTH 18.0f
#define TIMELINE_NOTCH_TEXT_YPOS 10.0f

EntityDopeSheetView::EntityDopeSheetView(QWidget *pParent /*= nullptr*/) :
	QGraphicsView(pParent),
	m_iCurrentFrame(0)
{
}

/*virtual*/ EntityDopeSheetView::~EntityDopeSheetView()
{
}

EntityDopeSheetScene *EntityDopeSheetView::GetScene() const
{
	return static_cast<EntityDopeSheetScene *>(scene());
}

/*virtual*/ void EntityDopeSheetView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
{
	pPainter->setPen(Qt::NoPen);
	pPainter->setBrush(HyGlobal::CovertHyColor(HyColor::ContainerPanel));
	pPainter->drawRect(QRectF(rect.x(), rect.y(), rect.width(), TIMELINE_HEIGHT));

	pPainter->setPen(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
	pPainter->drawLine(rect.x(), rect.y() + TIMELINE_HEIGHT, rect.x() + rect.width(), rect.y() + TIMELINE_HEIGHT);

	int iNotchIndex = 0;
	int iFrameIndex = 0;
	float fCurZoom = 1.0f;// fZoom;
	qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * fCurZoom;
	int iNumSubLines = 4; // Either 0, 1, or 4

	const qreal fPOSX_DRAW_THRESHOLD = rect.x() + TIMELINE_LEFT_MARGIN;

	int iHorzScrollAmt = horizontalScrollBar()->value();
	qreal fPosX = fPOSX_DRAW_THRESHOLD - iHorzScrollAmt;
	
	//float fRemainingWidth = rect.width() - TIMELINE_LEFT_MARGIN;
	while(fPosX < rect.x() + rect.width())// fRemainingWidth > 0.0f)
	{
		if(fPosX >= fPOSX_DRAW_THRESHOLD)
		{
			//qreal fPosX = rect.x() + TIMELINE_NOTCH_START_XPOS + ((TIMELINE_NOTCH_WIDTH * fCurZoom) * iNotchIndex);
			pPainter->setPen(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
			pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);

			//QFont font = pPainter->font();
			//font.setPixelSize(48);
			//pPainter->setFont(font);
			const float fTextWidth = pPainter->fontMetrics().width(QString::number(iFrameIndex));
			QRectF textRect(fPosX - (fTextWidth * 0.5f) + 1.0f, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f + 1.0f, fTextWidth, 20.0f);
			pPainter->setPen(HyGlobal::CovertHyColor(HyColor::Black));
			pPainter->drawText(textRect, Qt::AlignHCenter, QString::number(iFrameIndex));

			textRect.translate(-1.0f, -1.0f);
			pPainter->setPen(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
			pPainter->drawText(textRect, Qt::AlignHCenter, QString::number(iFrameIndex));

		}

		for(int i = 0; i < iNumSubLines; ++i)
		{
			fPosX += fSubLineSpacing;
			if(fPosX >= fPOSX_DRAW_THRESHOLD)
				pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);
		}

		iNotchIndex++;
		iFrameIndex += (iNumSubLines + 1);
		//fRemainingWidth -= TIMELINE_NOTCH_WIDTH * fCurZoom;
	}
}
