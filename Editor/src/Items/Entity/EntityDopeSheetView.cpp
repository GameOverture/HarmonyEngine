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

			// Shadow Text
			const float fTextWidth = pPainter->fontMetrics().width(QString::number(iFrameIndex));
			QRectF textRect(fPosX - (fTextWidth * 0.5f) + 1.0f, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f + 1.0f, fTextWidth, 20.0f);
			pPainter->setPen(HyGlobal::CovertHyColor(HyColor::Black));
			pPainter->drawText(textRect, Qt::AlignHCenter, QString::number(iFrameIndex));

			// Fill Text
			textRect.translate(-1.0f, -1.0f);
			pPainter->setPen(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
			pPainter->drawText(textRect, Qt::AlignHCenter, QString::number(iFrameIndex));
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
	std::function<bool(EntityTreeItemData *, QPointF)> fpDrawEntityItem = [&](EntityTreeItemData *pEntItemData, QPointF ptDrawPos) -> bool
	{
		//const QUuid &uuidRef = pEntItemData->GetThisUuid();
		if(GetScene()->GetKeyFramesMap().contains(pEntItemData) == false)
			return false;

		// Draw Entity Item Name
		//pPainter->font().pixelSize()
		QRectF textRect(ptDrawPos.x(), ptDrawPos.y() + 20.0f, TIMELINE_LEFT_MARGIN, ITEMS_HEIGHT);
		pPainter->drawText(textRect, pEntItemData->GetCodeName());

		// Draw divider line
		pPainter->drawLine(rect.x(), ptDrawPos.y() + ITEMS_HEIGHT, rect.x() + rect.width(), ptDrawPos.y() + ITEMS_HEIGHT);

		return true;

		//const QMap<int, QJsonObject> &keyFrameMapRef = GetScene()->GetKeyFramesMap()[pEntItemData];

		//for(int i = 0; i < keysList.size(); ++i)
		//{
		//	EntityTreeItemData *pKey = keysList[i];
		//	if(pKey->GetThisUuid() != uuidRef)
		//		continue;
		//	//const QMap<int, QJsonObject> &frameMapRef = keyFrameMapRef[pKey];
		//	//QList<int> frameList = frameMapRef.keys();
		//	//for(int iFrame : frameList)
		//	//{
		//	//	if(iFrame < m_iCurrentFrame)
		//	//		continue;
		//	//	QJsonObject &frameObjRef = frameMapRef[iFrame];
		//	//	QRectF frameRect;
		//	//	frameRect.setX(fPOSX_DRAW_THRESHOLD + (iFrame * fSubLineSpacing));
		//	//	frameRect.setY(rect.y() + TIMELINE_HEIGHT);
		//	//	frameRect.setWidth(fSubLineSpacing);
		//	//	frameRect.setHeight(100.0f);
		//	//	pPainter->setPen(HyGlobal::CovertHyColor(HyColor::WidgetFrame));
		//	//	pPainter->setBrush(HyGlobal::CovertHyColor(HyColor::WidgetFill));
		//	//	pPainter->drawRect(frameRect);
		//	//	pPainter->setPen(HyGlobal::CovertHyColor(HyColor::Black));
		//	//	pPainter->drawText(frameRect, Qt::AlignHCenter | Qt::AlignVCenter, QString::number(iFrame));
		//	//}
		//}
	};

	//const QUuid &rootUuidRef = ->GetThisUuid();
	{
		int iItemIndex = 0;
		float fPosX = rect.x() + ITEMS_LEFT_MARGIN;
		float fPosY = rect.y() + TIMELINE_HEIGHT + (iItemIndex * ITEMS_HEIGHT);

		QList<EntityTreeItemData *> itemList, shapeList;
		static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().GetTreeItemData(itemList, shapeList);
		itemList += shapeList;
		itemList.prepend(static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().GetRootTreeItemData());
		for(EntityTreeItemData *pItem : itemList)
		{
			if(fpDrawEntityItem(pItem, QPointF(fPosX, fPosY)))
			{
				iItemIndex++;
				fPosY += ITEMS_HEIGHT;
			}
		}
	}
}
