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

#include <QPainter>
#include <QGraphicsItem>
#include <QMouseEvent>

HarmonyRulerGfxView::HarmonyRulerGfxView(QWidget *pParent /*= nullptr*/) :
	QGraphicsView(pParent),
	m_eOrientation(HYORIENT_Horizontal)
{
}

/*virtual*/ HarmonyRulerGfxView::~HarmonyRulerGfxView()
{
}

void HarmonyRulerGfxView::Init(HyOrientation eOrientation)
{
}

/*virtual*/ void HarmonyRulerGfxView::drawBackground(QPainter *painter, const QRectF &rect) /*override*/
{
	QGraphicsView::drawBackground(painter, rect);
	////////////////////////////////////////////////////////////////////////////
	//// DRAW CURRENT FRAME INDICATOR
	////////////////////////////////////////////////////////////////////////////
	//const qreal fPOSX_DRAW_THRESHOLD = rect.x() + TIMELINE_LEFT_MARGIN;
	//int iHorzScrollAmt = horizontalScrollBar()->value();
	//qreal fPosX = fPOSX_DRAW_THRESHOLD - iHorzScrollAmt;
	//fPosX += (GetScene()->GetCurrentFrame() * TIMELINE_NOTCH_SUBLINES_WIDTH);

	//if(fPosX >= fPOSX_DRAW_THRESHOLD && fPosX < (rect.x() + rect.width()))
	//{
	//	painter->setPen(HyGlobal::ConvertHyColor(HyColor::Cyan));
	//	painter->drawLine(fPosX, rect.y(), fPosX, rect.y() + rect.height());
	//}
}

/*virtual*/ void HarmonyRulerGfxView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
{
	////////////////////////////////////////////////////////////////////////////
	//// DRAW CURRENT DRAGGING ITEMS PREVIEW
	////////////////////////////////////////////////////////////////////////////
	//if(m_eDragState == DRAGSTATE_Dragging)
	//{
	//	QPointF ptSceneDragStart = mapToScene(m_ptDragStart);
	//	int iFrameOffset = m_iDragFrame - GetNearestFrame(ptSceneDragStart.x());
	//	qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * m_fZoom;

	//	QList<QGraphicsItem *> itemList = items();
	//	QAbstractGraphicsShapeItem *pGfxItem = nullptr; // Just need one item to get the pen and brush
	//	QVector<QRectF> rectList;

	//	if(m_pGfxDragTweenKnobItem)
	//		pGfxItem = m_pGfxDragTweenKnobItem;
	//	else
	//	{
	//		for(QGraphicsItem *pItem : itemList)
	//		{
	//			if(pItem->isSelected())
	//			{
	//				QRectF rect = pItem->sceneBoundingRect();
	//				rect.translate(fSubLineSpacing * iFrameOffset, 0.0f);

	//				rectList.push_back(rect);
	//				pGfxItem = static_cast<QAbstractGraphicsShapeItem *>(pItem);
	//			}
	//		}
	//	}

	//	if(pGfxItem)
	//	{
	//		pPainter->setPen(pGfxItem->pen());
	//		QBrush brush = pGfxItem->brush();
	//		QColor color = brush.color();
	//		color.setAlphaF(0.5f);
	//		brush.setColor(color);
	//		pPainter->setBrush(brush);

	//		if(m_pGfxDragTweenKnobItem)
	//		{
	//			QRectF rect = m_pGfxDragTweenKnobItem->sceneBoundingRect();
	//			rect.translate(fSubLineSpacing * iFrameOffset, 0.0f);
	//			pPainter->drawEllipse(rect);
	//		}
	//		else
	//			pPainter->drawRects(rectList);
	//	}
	//}

	////////////////////////////////////////////////////////////////////////////
	//// LEFT SIDE ITEM LIST
	////////////////////////////////////////////////////////////////////////////
	//m_pMouseHoverItem = nullptr;
	//qreal fPosY = rect.y() + TIMELINE_HEIGHT + 1.0f;
	//fPosY -= verticalScrollBar()->value();

	//QList<EntityTreeItemData *> itemList = GetItems();
	//for(EntityTreeItemData *pEntItemData : itemList)
	//{
	//	HyColor textColor = HyColor::WidgetFrame;

	//	// Determine number of rows of key frames
	//	int iNumRows = 1;
	//	QList<QPair<QString, QString>> propList;
	//	if(pEntItemData->IsSelected())
	//	{
	//		textColor = HyColor::LightGray;

	//		propList = GetScene()->GetUniquePropertiesList(pEntItemData, true);
	//		iNumRows += propList.size();
	//	}

	//	// Background Rect
	//	pPainter->setPen(Qt::NoPen);
	//	pPainter->setBrush(HyGlobal::ConvertHyColor(HyColor::ContainerPanel));
	//	pPainter->drawRect(QRectF(rect.x(), fPosY, ITEMS_WIDTH, iNumRows * ITEMS_LINE_HEIGHT));

	//	// Item Name
	//	QString sCodeName;
	//	if(pEntItemData->GetEntType() == ENTTYPE_ArrayItem)
	//		sCodeName = pEntItemData->GetCodeName() % "[" % QString::number(pEntItemData->GetArrayIndex()) % "]";
	//	else
	//		sCodeName = pEntItemData->GetCodeName();

	//	QFont bold(pPainter->font());
	//	bold.setBold(pEntItemData->IsSelected());
	//	pPainter->setFont(bold);

	//	QRectF nameBoundingRect(rect.x() + ITEMS_LEFT_MARGIN, fPosY + 4.0f, pPainter->fontMetrics().horizontalAdvance(sCodeName), ITEMS_LINE_HEIGHT - 5.0f);		
	//	if(nameBoundingRect.contains(m_MouseScenePos))
	//	{
	//		m_pMouseHoverItem = pEntItemData;
	//		textColor = HyColor::White;
	//	}

	//	DrawShadowText(pPainter, nameBoundingRect, sCodeName, textColor);

	//	// Draw Item Icon
	//	QModelIndex itemIndex = static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().FindIndex<EntityTreeItemData *>(pEntItemData, 0);
	//	QVariant variantIcon = static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().data(itemIndex, Qt::DecorationRole);
	//	if(variantIcon.isValid())
	//	{
	//		QSize iconSize(16, 16);
	//		pPainter->drawPixmap(rect.x() + ITEMS_LEFT_MARGIN - iconSize.width() - 5.0f, fPosY + ((ITEMS_LINE_HEIGHT - iconSize.height()) / 2), variantIcon.value<QIcon>().pixmap(iconSize.width(), iconSize.height()));
	//	}

	//	fPosY += ITEMS_LINE_HEIGHT;

	//	bold.setBold(false);
	//	pPainter->setFont(bold);

	//	// Properties
	//	if(pEntItemData->IsSelected())
	//	{
	//		for(QPair<QString, QString> &propPair : propList)
	//		{
	//			DrawShadowText(pPainter, QRectF(rect.x() + ITEMS_LEFT_MARGIN + ITEMS_LEFT_MARGIN, fPosY + 5.0f, ITEMS_WIDTH - ITEMS_LEFT_MARGIN, ITEMS_LINE_HEIGHT), propPair.second);
	//			fPosY += ITEMS_LINE_HEIGHT;
	//		}
	//	}

	//	// Draw divider line
	//	pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::Black));
	//	pPainter->drawLine(rect.x(), fPosY, rect.x() + rect.width(), fPosY);
	//	fPosY += 1.0f;
	//}

	////////////////////////////////////////////////////////////////////////////
	//// TIMELINE
	////////////////////////////////////////////////////////////////////////////
	//pPainter->setPen(Qt::NoPen);
	//pPainter->setBrush(HyGlobal::ConvertHyColor(HyColor::ContainerPanel));
	//pPainter->drawRect(QRectF(rect.x(), rect.y(), rect.width(), TIMELINE_HEIGHT));

	//pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::WidgetFrame));
	//pPainter->drawLine(rect.x(), rect.y() + TIMELINE_HEIGHT, rect.x() + rect.width(), rect.y() + TIMELINE_HEIGHT);

	////const QMap<int, QStringList> &eventMapRef = GetScene()->GetEventMap();

	//std::function<void(DopeSheetEvent, int, float)> fpPaintEvent = [&](DopeSheetEvent dopeSheetEvent, int iFrameIndex, float fX)
	//{
	//	switch(dopeSheetEvent.m_eType)
	//	{ 
	//	case DOPEEVENT_Callback:
	//		pPainter->translate(fX, rect.y() + TIMELINE_HEIGHT - (CALLBACK_DIAMETER * 0.5f));
	//		pPainter->setPen(Qt::NoPen);// HyGlobal::ConvertHyColor(HyColor::Black));
	//		pPainter->setBrush(HyGlobal::ConvertHyColor(HyColor::Orange));

	//		pPainter->rotate(45.0);
	//		pPainter->drawRect(CALLBACK_DIAMETER * -0.5, CALLBACK_DIAMETER * -0.5, CALLBACK_DIAMETER, CALLBACK_DIAMETER);
	//		break;

	//	case DOPEEVENT_PauseTimeline: {
	//		QSize iconSize(16, 16);
	//		QIcon pauseIcon(":/icons16x16/media-pause.png");
	//		QPoint pt = QPoint(fX - (iconSize.width() / 2) + 1.0f, rect.y() + TIMELINE_HEIGHT - (CALLBACK_DIAMETER * 0.5f) - iconSize.width());
	//		pPainter->drawPixmap(pt, pauseIcon.pixmap(iconSize.width(), iconSize.height()));
	//		break; }

	//	case DOPEEVENT_GotoFrame: {
	//		QSize iconSize(16, 16);
	//		QPoint pt = QPoint(fX - (iconSize.width() / 2) + 1.0f, rect.y() + TIMELINE_HEIGHT - (CALLBACK_DIAMETER * 0.5f) - iconSize.width());
	//		int iGotoFrameIndex = dopeSheetEvent.m_sData.toInt();
	//		if(iGotoFrameIndex > iFrameIndex)
	//		{
	//			QIcon gotoIcon(":/icons16x16/media-forward.png");
	//			pPainter->drawPixmap(pt, gotoIcon.pixmap(iconSize.width(), iconSize.height()));
	//		}
	//		else
	//		{
	//			QIcon gotoIcon(":/icons16x16/media-rewind.png");
	//			pPainter->drawPixmap(pt, gotoIcon.pixmap(iconSize.width(), iconSize.height()));
	//		}
	//		break; }

	//	default:
	//		HyGuiLog("fpPaintEvent: Unknown DopeSheetEventType", LOGTYPE_Error);
	//		break;
	//	}

	//	pPainter->resetTransform();
	//};

	//int iFrameIndex = 0;
	//qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * m_fZoom;
	//int iNumSubLines = 4; // Either 0, 1, or 4

	//const qreal fPOSX_DRAW_THRESHOLD = rect.x() + TIMELINE_LEFT_MARGIN;

	//int iHorzScrollAmt = horizontalScrollBar()->value();
	//qreal fPosX = fPOSX_DRAW_THRESHOLD - iHorzScrollAmt;
	//while(fPosX < rect.x() + rect.width())
	//{
	//	// Main Notch Line
	//	if(fPosX >= fPOSX_DRAW_THRESHOLD)
	//	{
	//		HyColor eColor = HyColor::WidgetFrame;

	//		if(GetScene()->GetCurrentFrame() == iFrameIndex)
	//		{
	//			eColor = HyColor::Cyan;
	//			
	//			DrawCurrentFrameIndicator(pPainter, fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, eColor);
	//			pPainter->setPen(HyGlobal::ConvertHyColor(eColor));
	//		}
	//		else
	//			pPainter->setPen(HyGlobal::ConvertHyColor(eColor));
	//		
	//		pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);

	//		// Main Notch Keyframe Text
	//		const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(iFrameIndex));
	//		QRectF textRect(fPosX - (fTextWidth * 0.5f), rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f, fTextWidth, 20.0f);
	//		DrawShadowText(pPainter, textRect, QString::number(iFrameIndex), eColor, HyColor::Black);

	//		// Draw timeline events
	//		QList<DopeSheetEvent> dopeSheetEventList = GetScene()->GetEventList(iFrameIndex);
	//		for(DopeSheetEvent dopeEvent : dopeSheetEventList)
	//			fpPaintEvent(dopeEvent, iFrameIndex, fPosX);
	//	}

	//	// Sub Notch Lines
	//	pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::WidgetFrame));
	//	for(int i = 0; i < iNumSubLines; ++i)
	//	{
	//		fPosX += fSubLineSpacing;
	//		if(fPosX >= fPOSX_DRAW_THRESHOLD)
	//		{
	//			int iCurSubNotchFrame = (iFrameIndex + 1) + i;
	//			if(GetScene()->GetCurrentFrame() == iCurSubNotchFrame)
	//			{
	//				pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::Cyan));

	//				const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(iCurSubNotchFrame));
	//				QRectF textRect(fPosX - (fTextWidth * 0.5f), rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f, fTextWidth, 20.0f);
	//				DrawShadowText(pPainter, textRect, QString::number(iCurSubNotchFrame), HyColor::Cyan, HyColor::Black);

	//				DrawCurrentFrameIndicator(pPainter, fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, HyColor::Cyan);
	//				pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::Cyan));
	//			}
	//			else
	//				pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::WidgetFrame));

	//			pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);

	//			// Draw timeline events
	//			QList<DopeSheetEvent> dopeSheetEventList = GetScene()->GetEventList(iCurSubNotchFrame);
	//			for(DopeSheetEvent dopeEvent : dopeSheetEventList)
	//				fpPaintEvent(dopeEvent, iCurSubNotchFrame, fPosX);
	//		}
	//	}
	//	
	//	fPosX += fSubLineSpacing;
	//	iFrameIndex += (iNumSubLines + 1);
	//}
}

/*virtual*/ bool HarmonyRulerGfxView::event(QEvent *pEvent) /*override*/
{
	//if(pEvent->type() == QEvent::HoverEnter || pEvent->type() == QEvent::HoverLeave)
	//{
	//	m_MouseScenePos.setX(0.0f);
	//	m_MouseScenePos.setY(0.0f);
	//	update();
	//}

	return QGraphicsView::event(pEvent);
}

/*virtual*/ void HarmonyRulerGfxView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	//m_MouseScenePos = mapToScene(pEvent->pos());

	//if(m_bTimeLineMouseDown)
	//{
	//	GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));
	//	if(m_pAuxDopeSheet)
	//		m_pAuxDopeSheet->UpdateWidgets();

	//	pEvent->accept();
	//}
	//else
	//{
	//	if(DRAGSTATE_Dragging == m_eDragState)
	//	{
	//		m_iDragFrame = GetNearestFrame(m_MouseScenePos.x());
	//		update();
	//	}
	//	else if(pEvent->pos().x() <= TIMELINE_LEFT_MARGIN)
	//	{
	//		m_bLeftSideDirty = true;
	//		update();
	//	}
	//	else if(m_bLeftSideDirty)
	//	{
	//		m_bLeftSideDirty = false;
	//		update();
	//	}
	//	else if(DRAGSTATE_InitialPress == m_eDragState)
	//	{
	//		QPointF dragDelta = pEvent->pos() - m_ptDragStart;
	//		if(dragDelta.manhattanLength() >= 3)
	//		{
	//			m_eDragState = DRAGSTATE_Dragging;
	//			m_iDragFrame = GetNearestFrame(m_MouseScenePos.x());
	//		}
	//		update();
	//	}
	//}

	//if(rubberBandRect().isNull() == false)
	//	update();
	
	QGraphicsView::mouseMoveEvent(pEvent);
}

/*virtual*/ void HarmonyRulerGfxView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(pEvent->button() != Qt::LeftButton)
	{
		QGraphicsView::mousePressEvent(pEvent);
		return;
	}

	//if(m_pMouseHoverItem)
	//{
	//	bool bShiftPressed = pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier);

	//	QItemSelectionModel::SelectionFlags flags = bShiftPressed ? QItemSelectionModel::Toggle : QItemSelectionModel::ClearAndSelect;
	//	static_cast<EntityWidget *>(m_pStateData->GetModel().GetItem().GetWidget())->RequestSelectedItemChange(m_pMouseHoverItem, flags);
	//	pEvent->accept();
	//}
	//else if(pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f && pEvent->pos().y() < TIMELINE_HEIGHT)
	//{
	//	m_MouseScenePos = mapToScene(pEvent->pos());

	//	m_bTimeLineMouseDown = true;
	//	GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));
	//	if(m_pAuxDopeSheet)
	//		m_pAuxDopeSheet->UpdateWidgets();

	//	pEvent->accept();
	//}
	//else if(pEvent->pos().y() > TIMELINE_HEIGHT)
	//{	
	//	QGraphicsItem *pItemUnderMouse = itemAt(pEvent->pos());
	//	if(pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f && pItemUnderMouse)
	//	{
	//		m_eDragState = DRAGSTATE_InitialPress;
	//		m_ptDragStart = pEvent->pos();

	//		if(pItemUnderMouse->data(GFXDATAKEY_Type).toInt() == GFXITEM_TweenKnob)
	//		{
	//			//GetScene()->clearSelection();
	//			m_pGfxDragTweenKnobItem = static_cast<GraphicsTweenKnobItem *>(pItemUnderMouse);
	//			m_pGfxDragTweenKnobItem->setSelected(true);
	//		}
	//	}

	//	// Only want default selection behavior when NOT clicking in the 'time line' or an 'item name' column AND it's not a tween knob
	//	if(pItemUnderMouse == nullptr || pItemUnderMouse->data(GFXDATAKEY_Type).toInt() != GFXITEM_TweenKnob)
	//		QGraphicsView::mousePressEvent(pEvent);
	//}

	update();
}

/*virtual*/ void HarmonyRulerGfxView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	//m_bTimeLineMouseDown = false;

	//if(DRAGSTATE_Dragging == m_eDragState)
	//{
	//	if(m_pGfxDragTweenKnobItem)
	//	{
	//		EntityTreeItemData *pTreeItemData = m_pGfxDragTweenKnobItem->parentItem()->data(GFXDATAKEY_TreeItemData).value<EntityTreeItemData *>();
	//		TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(m_pGfxDragTweenKnobItem->parentItem()->data(GFXDATAKEY_CategoryPropString).toString().split('/')[1]);
	//		int iTweenStartKeyFrame = m_pGfxDragTweenKnobItem->parentItem()->data(GFXDATAKEY_FrameIndex).toInt();
	//		int iTweenEndKeyFrame = HyMath::Max(iTweenStartKeyFrame, m_iDragFrame);

	//		double dNewDuration = (iTweenEndKeyFrame - iTweenStartKeyFrame) * (1.0 / static_cast<EntityModel &>(m_pStateData->GetModel()).GetFramesPerSecond());
	//		EntityUndoCmd_NudgeTweenDuration *pCmd = new EntityUndoCmd_NudgeTweenDuration(m_pStateData->GetDopeSheetScene(), pTreeItemData, iTweenStartKeyFrame, eTweenProp, dNewDuration);
	//		m_pStateData->GetModel().GetItem().GetUndoStack()->push(pCmd);
	//	}
	//	else
	//	{
	//		QPointF ptSceneDragStart = mapToScene(m_ptDragStart);
	//		EntityUndoCmd_NudgeSelectedKeyFrames *pCmd = new EntityUndoCmd_NudgeSelectedKeyFrames(m_pStateData->GetDopeSheetScene(), GetNearestFrame(m_MouseScenePos.x()) - GetNearestFrame(ptSceneDragStart.x()));
	//		m_pStateData->GetModel().GetItem().GetUndoStack()->push(pCmd);
	//	}
	//}
	//else if(rubberBandRect().isNull() && pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f)
	//	GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));

	//m_pGfxDragTweenKnobItem = nullptr;
	//m_eDragState = DRAGSTATE_None;

	update();
	QGraphicsView::mouseReleaseEvent(pEvent);
}
