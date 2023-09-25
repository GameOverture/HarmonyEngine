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
#include "EntityModel.h"

#include <QPainter>
#include <QScrollBar>

EntityDopeSheetView::EntityDopeSheetView(QWidget *pParent /*= nullptr*/) :
	QGraphicsView(pParent),
	m_pStateData(nullptr)
{
}

/*virtual*/ EntityDopeSheetView::~EntityDopeSheetView()
{
}

EntityDopeSheetScene *EntityDopeSheetView::GetScene() const
{
	return static_cast<EntityDopeSheetScene *>(scene());
}

void EntityDopeSheetView::SetScene(EntityStateData *pStateData)
{
	m_pStateData = pStateData;
	setScene(&m_pStateData->GetDopeSheetScene());
}

/*virtual*/ void EntityDopeSheetView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
{
	//////////////////////////////////////////////////////////////////////////
	// TIMELINE
	//////////////////////////////////////////////////////////////////////////
	pPainter->setPen(Qt::NoPen);
	pPainter->setBrush(HyGlobal::CovertHyColor(HyColor::ContainerPanel));
	pPainter->drawRect(QRectF(rect.x(), rect.y(), rect.width(), TIMELINE_HEIGHT));

	pPainter->setPen(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
	pPainter->drawLine(rect.x(), rect.y() + TIMELINE_HEIGHT, rect.x() + rect.width(), rect.y() + TIMELINE_HEIGHT);

	int iNotchIndex = 0;
	int iFrameIndex = 0;
	float fCurZoom = GetScene()->GetZoom();
	qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * fCurZoom;
	int iNumSubLines = 4; // Either 0, 1, or 4

	const qreal fPOSX_DRAW_THRESHOLD = rect.x() + TIMELINE_LEFT_MARGIN;

	int iHorzScrollAmt = horizontalScrollBar()->value();
	qreal fPosX = fPOSX_DRAW_THRESHOLD - iHorzScrollAmt;
	while(fPosX < rect.x() + rect.width())
	{
		if(fPosX >= fPOSX_DRAW_THRESHOLD)
		{
			// Main Notch Line
			pPainter->setPen(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
			pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);

			// Main Notch Keyframe Text
			const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(iFrameIndex));
			QRectF textRect(fPosX - (fTextWidth * 0.5f), rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f, fTextWidth, 20.0f);
			DrawShadowText(pPainter, textRect, QString::number(iFrameIndex));
		}

		// Sub Notch Lines
		for(int i = 0; i < iNumSubLines; ++i)
		{
			fPosX += fSubLineSpacing;
			if(fPosX >= fPOSX_DRAW_THRESHOLD)
				pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);
		}

		iNotchIndex++;
		iFrameIndex += (iNumSubLines + 1);
	}

	//////////////////////////////////////////////////////////////////////////
	// LEFT SIDE ITEM LIST
	//////////////////////////////////////////////////////////////////////////
	qreal fPosY = rect.y() + TIMELINE_HEIGHT + 1.0f;

	// Gather all the entity items (root, children, shapes) into one list 'itemList'
	QList<EntityTreeItemData *> itemList, shapeList;
	static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().GetTreeItemData(itemList, shapeList);
	itemList += shapeList;
	itemList.prepend(static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().GetRootTreeItemData());

	for(EntityTreeItemData *pEntItemData : itemList)
	{
		// Only draw the items that have key frames
		if(GetScene()->GetKeyFramesMap().contains(pEntItemData) == false)
			continue;

		// Determine number of rows of key frames
		int iNumRows = 1;
		QList<QPair<QString, QString>> propList = GetScene()->GetUniquePropertiesList(pEntItemData);
		iNumRows += propList.size();

		// Background Rect
		pPainter->setPen(Qt::NoPen);
		pPainter->setBrush(HyGlobal::CovertHyColor(HyColor::ContainerPanel));
		pPainter->drawRect(QRectF(rect.x(), fPosY, TIMELINE_LEFT_MARGIN, iNumRows * ITEMS_LINE_HEIGHT));

		// Item Name
		QString sCodeName;
		if(pEntItemData->GetEntType() == ENTTYPE_ArrayItem)
			sCodeName = pEntItemData->GetCodeName() % "[" % QString::number(pEntItemData->GetArrayIndex()) % "]";
		else
			sCodeName = pEntItemData->GetCodeName();
		DrawShadowText(pPainter, QRectF(rect.x() + ITEMS_LEFT_MARGIN, fPosY + 5.0f, TIMELINE_LEFT_MARGIN, ITEMS_LINE_HEIGHT), sCodeName);
		fPosY += ITEMS_LINE_HEIGHT;

		// Properties
		for(QPair<QString, QString> &propPair : propList)
		{
			DrawShadowText(pPainter, QRectF(rect.x() + ITEMS_LEFT_MARGIN + ITEMS_LEFT_MARGIN, fPosY + 5.0f, TIMELINE_LEFT_MARGIN - ITEMS_LEFT_MARGIN, ITEMS_LINE_HEIGHT), propPair.second);
			fPosY += ITEMS_LINE_HEIGHT;
		}

		// Draw divider line
		pPainter->setPen(HyGlobal::CovertHyColor(HyColor::Black));
		pPainter->drawLine(rect.x(), fPosY, rect.x() + rect.width(), fPosY);
		fPosY += 1.0f;
	}
}

void EntityDopeSheetView::DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor color /*= HyColor::WidgetFrame*/, HyColor shadowColor /*= HyColor::Black*/)
{
	textRect.translate(1.0f, 1.0f);
	pPainter->setPen(HyGlobal::CovertHyColor(shadowColor));
	pPainter->drawText(textRect, sText);
	textRect.translate(-1.0f, -1.0f);
	pPainter->setPen(HyGlobal::CovertHyColor(color));
	pPainter->drawText(textRect, sText);
}
