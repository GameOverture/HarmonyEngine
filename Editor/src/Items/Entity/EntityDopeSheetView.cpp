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
#include "EntityWidget.h"
#include "EntityUndoCmds.h"

#include <QPainter>
#include <QScrollBar>
#include <QGraphicsItem>
#include <QMouseEvent>

EntityDopeSheetView::EntityDopeSheetView(QWidget *pParent /*= nullptr*/) :
	QGraphicsView(pParent),
	m_pStateData(nullptr),
	m_pMouseHoverItem(nullptr),
	m_bTimeLineMouseDown(false),
	m_bLeftSideDirty(false),
	m_fZoom(1.0f),
	m_eDragState(DRAGSTATE_None),
	m_ptDragStart(0.0f, 0.0f),
	m_iDragFrame(-1),
	m_pGfxDragTweenKnobItem(nullptr),
	m_pContextClickItem(nullptr)
{
	for(int iTweenProp = 0; iTweenProp < NUM_TWEENPROPS; ++iTweenProp)
	{
		TweenProperty eTweenProp = static_cast<TweenProperty>(iTweenProp);
		m_ActionTweenList.push_back(new QAction(HyGlobal::TweenPropIcon(eTweenProp), "Tween " % HyGlobal::TweenPropName(eTweenProp)));
		connect(m_ActionTweenList[iTweenProp], &QAction::triggered, this, &EntityDopeSheetView::OnTweenAction);
	}

	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	setDragMode(QGraphicsView::RubberBandDrag);
}

/*virtual*/ EntityDopeSheetView::~EntityDopeSheetView()
{
	for(int iTweenProp = 0; iTweenProp < NUM_TWEENPROPS; ++iTweenProp)
		delete m_ActionTweenList[iTweenProp];
}

EntityDopeSheetScene *EntityDopeSheetView::GetScene() const
{
	return static_cast<EntityDopeSheetScene *>(scene());
}

void EntityDopeSheetView::SetScene(EntityStateData *pStateData)
{
	m_pStateData = pStateData;
	if(m_pStateData == nullptr)
		setScene(nullptr);
	else
		setScene(&m_pStateData->GetDopeSheetScene());
}

float EntityDopeSheetView::GetZoom() const
{
	return m_fZoom;
}

/*virtual*/ void EntityDopeSheetView::contextMenuEvent(QContextMenuEvent *pEvent) /*override*/
{
	if(m_pStateData == nullptr || pEvent->pos().y() <= TIMELINE_HEIGHT + 1)
		return;

	m_pContextClickItem = nullptr;
	QPair<QString, QString> contextProp = QPair<QString, QString>("", "");

	QPointF ptScenePos = mapToScene(pEvent->pos());

	qreal fPosY = TIMELINE_HEIGHT + 1.0f;

	QList<EntityTreeItemData *> itemList = GetItems();
	for(EntityTreeItemData *pEntItemData : itemList)
	{
		fPosY += ITEMS_LINE_HEIGHT;
		if(ptScenePos.y() < fPosY)
		{
			m_pContextClickItem = pEntItemData;
			break;
		}

		if(pEntItemData->IsSelected())
		{
			QList<QPair<QString, QString>> propList;
			propList = GetScene()->GetUniquePropertiesList(pEntItemData);

			for(QPair<QString, QString> &propPair : propList)
			{
				fPosY += ITEMS_LINE_HEIGHT;
				if(ptScenePos.y() < fPosY)
				{
					m_pContextClickItem = pEntItemData;
					contextProp = propPair;
					break;
				}
			}
		}
		if(m_pContextClickItem)
			break;
	}

	if(m_pContextClickItem)
	{
		int iFrame = GetNearestFrame(ptScenePos.x());

		int iNumSelected = scene()->selectedItems().size();

		QMenu menu;
		//// Determine the main tween action based on 'rightClickProp', then insert the tween menu containing all valid tweens for this item
		//if(contextProp.first == "Transformation")
		//{
		//	if(contextProp.second == "Position")
		//		menu.addAction(&m_ActionTweenPos);
		//	else if(contextProp.second == "Rotation")
		//		menu.addAction(&m_ActionTweenRot);
		//	else if(contextProp.second == "Scale")
		//		menu.addAction(&m_ActionTweenScale);
		//}
		//else if(contextProp.first == "Body")
		//{
		//	if(contextProp.second == "Alpha")
		//		menu.addAction(&m_ActionTweenAlpha);
		//}
		//QMenu tweenMenu("Add Tween");
		//tweenMenu.addAction(&m_ActionTweenPos);
		//tweenMenu.addAction(&m_ActionTweenRot);
		//tweenMenu.addAction(&m_ActionTweenScale);
		//tweenMenu.addAction(&m_ActionTweenAlpha);

		//menu.addMenu(&tweenMenu);
		//menu.addSeparator();
		menu.addAction(QIcon(":/icons16x16/edit-copy.png"), "Copy");// , this, &EntityDopeSheetView::OnCopy);
		menu.addAction(QIcon(":/icons16x16/edit-paste.png"), "Paste");//, this, &EntityDopeSheetView::OnPaste);
		menu.addSeparator();
		menu.addAction("Select All '" % m_pContextClickItem->GetCodeName() % "' Key Frames", this, &EntityDopeSheetView::OnSelectAllItemKeyFrames);
		if(iNumSelected >= 1)
		{
			menu.addAction("Deselect Key Frames", this, &EntityDopeSheetView::OnDeselectItemKeyFrames);
			menu.addSeparator();
			menu.addAction(QIcon(":/icons16x16/edit-delete.png"), "Delete '" % QString::number(iNumSelected) % "' Selected Key Frames", this, &EntityDopeSheetView::OnDeleteKeyFrames);
		}
		menu.exec(pEvent->globalPos());
	}
}

/*virtual*/ void EntityDopeSheetView::drawBackground(QPainter *painter, const QRectF &rect) /*override*/
{
	//////////////////////////////////////////////////////////////////////////
	// DRAW CURRENT FRAME INDICATOR
	//////////////////////////////////////////////////////////////////////////
	const qreal fPOSX_DRAW_THRESHOLD = rect.x() + TIMELINE_LEFT_MARGIN;
	int iHorzScrollAmt = horizontalScrollBar()->value();
	qreal fPosX = fPOSX_DRAW_THRESHOLD - iHorzScrollAmt;
	fPosX += (GetScene()->GetCurrentFrame() * TIMELINE_NOTCH_SUBLINES_WIDTH);

	if(fPosX >= fPOSX_DRAW_THRESHOLD && fPosX < (rect.x() + rect.width()))
	{
		painter->setPen(HyGlobal::ConvertHyColor(HyColor::Cyan));
		painter->drawLine(fPosX, rect.y(), fPosX, rect.y() + rect.height());
	}
}

/*virtual*/ void EntityDopeSheetView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
{
	//////////////////////////////////////////////////////////////////////////
	// DRAW CURRENT DRAGGING ITEMS PREVIEW
	//////////////////////////////////////////////////////////////////////////
	if(m_eDragState == DRAGSTATE_Dragging)
	{
		int iFrameOffset = m_iDragFrame - GetNearestFrame(m_ptDragStart.x());
		qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * m_fZoom;

		QList<QGraphicsItem *> itemList = items();
		GraphicsKeyFrameItem *pKeyFrameItem = nullptr; // Just need one item to get the pen and brush
		QVector<QRectF> rectList;
		for(QGraphicsItem *pItem : itemList)
		{
			if(pItem->isSelected())
			{
				QRectF rect = pItem->sceneBoundingRect();
				rect.translate(fSubLineSpacing * iFrameOffset, 0.0f);

				rectList.push_back(rect);
				pKeyFrameItem = static_cast<GraphicsKeyFrameItem *>(pItem);
			}
		}

		if(pKeyFrameItem)
		{
			pPainter->setPen(pKeyFrameItem->pen());
			QBrush brush = pKeyFrameItem->brush();
			QColor color = brush.color();
			color.setAlphaF(0.5f);
			brush.setColor(color);
			pPainter->setBrush(brush);

			pPainter->drawRects(rectList);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// LEFT SIDE ITEM LIST
	//////////////////////////////////////////////////////////////////////////
	m_pMouseHoverItem = nullptr;
	qreal fPosY = rect.y() + TIMELINE_HEIGHT + 1.0f;
	fPosY -= verticalScrollBar()->value();

	QList<EntityTreeItemData *> itemList = GetItems();
	for(EntityTreeItemData *pEntItemData : itemList)
	{
		HyColor textColor = HyColor::WidgetFrame;

		// Determine number of rows of key frames
		int iNumRows = 1;
		QList<QPair<QString, QString>> propList;
		if(pEntItemData->IsSelected())
		{
			textColor = HyColor::LightGray;

			propList = GetScene()->GetUniquePropertiesList(pEntItemData);
			iNumRows += propList.size();
		}

		// Background Rect
		pPainter->setPen(Qt::NoPen);
		pPainter->setBrush(HyGlobal::ConvertHyColor(HyColor::ContainerPanel));
		pPainter->drawRect(QRectF(rect.x(), fPosY, ITEMS_WIDTH, iNumRows * ITEMS_LINE_HEIGHT));

		// Item Name
		QString sCodeName;
		if(pEntItemData->GetEntType() == ENTTYPE_ArrayItem)
			sCodeName = pEntItemData->GetCodeName() % "[" % QString::number(pEntItemData->GetArrayIndex()) % "]";
		else
			sCodeName = pEntItemData->GetCodeName();

		QFont bold(pPainter->font());
		bold.setBold(pEntItemData->IsSelected());
		pPainter->setFont(bold);

		QRectF nameBoundingRect(rect.x() + ITEMS_LEFT_MARGIN, fPosY + 4.0f, pPainter->fontMetrics().horizontalAdvance(sCodeName), ITEMS_LINE_HEIGHT - 5.0f);		
		if(nameBoundingRect.contains(m_MouseScenePos))
		{
			m_pMouseHoverItem = pEntItemData;
			textColor = HyColor::White;
		}

		DrawShadowText(pPainter, nameBoundingRect, sCodeName, textColor);

		// Draw Item Icon
		QModelIndex itemIndex = static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().FindIndex<EntityTreeItemData *>(pEntItemData, 0);
		QVariant variantIcon = static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().data(itemIndex, Qt::DecorationRole);
		if(variantIcon.isValid())
		{
			QSize iconSize(16, 16);
			pPainter->drawPixmap(rect.x() + ITEMS_LEFT_MARGIN - iconSize.width() - 5.0f, fPosY + ((ITEMS_LINE_HEIGHT - iconSize.height()) / 2), variantIcon.value<QIcon>().pixmap(iconSize.width(), iconSize.height()));
		}

		fPosY += ITEMS_LINE_HEIGHT;

		bold.setBold(false);
		pPainter->setFont(bold);

		// Properties
		if(pEntItemData->IsSelected())
		{
			for(QPair<QString, QString> &propPair : propList)
			{
				DrawShadowText(pPainter, QRectF(rect.x() + ITEMS_LEFT_MARGIN + ITEMS_LEFT_MARGIN, fPosY + 5.0f, ITEMS_WIDTH - ITEMS_LEFT_MARGIN, ITEMS_LINE_HEIGHT), propPair.second);
				fPosY += ITEMS_LINE_HEIGHT;
			}
		}

		// Draw divider line
		pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::Black));
		pPainter->drawLine(rect.x(), fPosY, rect.x() + rect.width(), fPosY);
		fPosY += 1.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	// TIMELINE
	//////////////////////////////////////////////////////////////////////////
	pPainter->setPen(Qt::NoPen);
	pPainter->setBrush(HyGlobal::ConvertHyColor(HyColor::ContainerPanel));
	pPainter->drawRect(QRectF(rect.x(), rect.y(), rect.width(), TIMELINE_HEIGHT));

	pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::WidgetFrame));
	pPainter->drawLine(rect.x(), rect.y() + TIMELINE_HEIGHT, rect.x() + rect.width(), rect.y() + TIMELINE_HEIGHT);

	int iFrameIndex = 0;
	qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * m_fZoom;
	int iNumSubLines = 4; // Either 0, 1, or 4

	const qreal fPOSX_DRAW_THRESHOLD = rect.x() + TIMELINE_LEFT_MARGIN;

	int iHorzScrollAmt = horizontalScrollBar()->value();
	qreal fPosX = fPOSX_DRAW_THRESHOLD - iHorzScrollAmt;
	while(fPosX < rect.x() + rect.width())
	{
		// Main Notch Line
		if(fPosX >= fPOSX_DRAW_THRESHOLD)
		{
			HyColor eColor = HyColor::WidgetFrame;

			if(GetScene()->GetCurrentFrame() == iFrameIndex)
			{
				eColor = HyColor::Cyan;
				
				DrawCurrentFrameIndicator(pPainter, fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, eColor);
				pPainter->setPen(HyGlobal::ConvertHyColor(eColor));
			}
			else
				pPainter->setPen(HyGlobal::ConvertHyColor(eColor));
			
			pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);

			// Main Notch Keyframe Text
			const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(iFrameIndex));
			QRectF textRect(fPosX - (fTextWidth * 0.5f), rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f, fTextWidth, 20.0f);
			DrawShadowText(pPainter, textRect, QString::number(iFrameIndex), eColor, HyColor::Black);
		}

		// Sub Notch Lines
		pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::WidgetFrame));
		for(int i = 0; i < iNumSubLines; ++i)
		{
			fPosX += fSubLineSpacing;
			if(fPosX >= fPOSX_DRAW_THRESHOLD)
			{
				int iCurSubNotchFrame = (iFrameIndex + 1) + i;
				if(GetScene()->GetCurrentFrame() == iCurSubNotchFrame)
				{
					pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::Cyan));

					const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(iCurSubNotchFrame));
					QRectF textRect(fPosX - (fTextWidth * 0.5f), rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f, fTextWidth, 20.0f);
					DrawShadowText(pPainter, textRect, QString::number(iCurSubNotchFrame), HyColor::Cyan, HyColor::Black);

					DrawCurrentFrameIndicator(pPainter, fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, HyColor::Cyan);
					pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::Cyan));
				}
				else
					pPainter->setPen(HyGlobal::ConvertHyColor(HyColor::WidgetFrame));

				pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);
			}
		}
		
		fPosX += fSubLineSpacing;
		iFrameIndex += (iNumSubLines + 1);
	}
}

/*virtual*/ bool EntityDopeSheetView::event(QEvent *pEvent) /*override*/
{
	if(pEvent->type() == QEvent::HoverEnter || pEvent->type() == QEvent::HoverLeave)
	{
		m_MouseScenePos.setX(0.0f);
		m_MouseScenePos.setY(0.0f);
		update();
	}

	return QGraphicsView::event(pEvent);
}

/*virtual*/ void EntityDopeSheetView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	m_MouseScenePos = mapToScene(pEvent->pos());

	if(m_bTimeLineMouseDown)
	{
		GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));
		pEvent->accept();
	}
	else
	{
		if(DRAGSTATE_Dragging == m_eDragState)
		{
			m_iDragFrame = GetNearestFrame(m_MouseScenePos.x());
			update();
		}
		else if(pEvent->pos().x() <= TIMELINE_LEFT_MARGIN)
		{
			m_bLeftSideDirty = true;
			update();
		}
		else if(m_bLeftSideDirty)
		{
			m_bLeftSideDirty = false;
			update();
		}
		else if(DRAGSTATE_InitialPress == m_eDragState)
		{
			QPointF dragDelta = pEvent->pos() - m_ptDragStart;
			if(dragDelta.manhattanLength() >= 3)
			{
				m_eDragState = DRAGSTATE_Dragging;
				m_iDragFrame = GetNearestFrame(m_MouseScenePos.x());
			}
			update();
		}
	}

	if(rubberBandRect().isNull() == false)
		update();
	
	QGraphicsView::mouseMoveEvent(pEvent);
}

/*virtual*/ void EntityDopeSheetView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(pEvent->button() != Qt::LeftButton)
	{
		QGraphicsView::mousePressEvent(pEvent);
		return;
	}

	if(m_pMouseHoverItem)
	{
		bool bShiftPressed = pEvent->modifiers().testFlag(Qt::KeyboardModifier::ShiftModifier);

		QItemSelectionModel::SelectionFlags flags = bShiftPressed ? QItemSelectionModel::Toggle : QItemSelectionModel::ClearAndSelect;
		static_cast<EntityWidget *>(m_pStateData->GetModel().GetItem().GetWidget())->RequestSelectedItemChange(m_pMouseHoverItem, flags);
		pEvent->accept();
	}
	else if(pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f && pEvent->pos().y() < TIMELINE_HEIGHT)
	{
		m_MouseScenePos = mapToScene(pEvent->pos());

		m_bTimeLineMouseDown = true;
		GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));
		pEvent->accept();
	}
	else if(pEvent->pos().y() > TIMELINE_HEIGHT)
	{	
		QGraphicsItem *pItemUnderMouse = itemAt(pEvent->pos());
		if(pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f && pItemUnderMouse)
		{
			m_eDragState = DRAGSTATE_InitialPress;
			m_ptDragStart = pEvent->pos();

			if(pItemUnderMouse->data(GraphicsKeyFrameItem::DATAKEY_IsTweenKnob).toBool())
			{
				GetScene()->clearSelection();
				m_pGfxDragTweenKnobItem = static_cast<GraphicsTweenKnobItem *>(pItemUnderMouse);
			}
		}

		// Only want default selection behavior when NOT clicking in the 'time line' or an 'item name' column AND it's not a tween knob
		if(pItemUnderMouse == nullptr || pItemUnderMouse->data(GraphicsKeyFrameItem::DATAKEY_IsTweenKnob).toBool() == false)
			QGraphicsView::mousePressEvent(pEvent);
	}

	update();
}

/*virtual*/ void EntityDopeSheetView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	m_bTimeLineMouseDown = false;

	if(DRAGSTATE_Dragging == m_eDragState)
	{
		EntityUndoCmd_NudgeSelectedKeyFrames *pCmd = new EntityUndoCmd_NudgeSelectedKeyFrames(m_pStateData->GetDopeSheetScene(), GetNearestFrame(m_MouseScenePos.x()) - GetNearestFrame(m_ptDragStart.x()));
		m_pStateData->GetModel().GetItem().GetUndoStack()->push(pCmd);
	}
	else if(rubberBandRect().isNull() && pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f)
		GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));

	m_eDragState = DRAGSTATE_None;

	update();
	QGraphicsView::mouseReleaseEvent(pEvent);
}

void EntityDopeSheetView::DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor color /*= HyColor::WidgetFrame*/, HyColor shadowColor /*= HyColor::Black*/)
{
	textRect.translate(1.0f, 1.0f);
	pPainter->setPen(HyGlobal::ConvertHyColor(shadowColor));
	pPainter->drawText(textRect, sText);
	textRect.translate(-1.0f, -1.0f);
	pPainter->setPen(HyGlobal::ConvertHyColor(color));
	pPainter->drawText(textRect, sText);
}

void EntityDopeSheetView::DrawCurrentFrameIndicator(QPainter *pPainter, qreal fPosX, qreal fPosY, HyColor color)
{	
	pPainter->setPen(HyGlobal::ConvertHyColor(color.Darken()));
	pPainter->setBrush(HyGlobal::ConvertHyColor(color));

	// Draw triangle pointing downward over the notch line using the below variables
	QPointF points[3];
	points[0] = QPointF(fPosX, fPosY);
	points[1] = QPointF(fPosX - (TIMELINE_CURRENTFRAME_TRIANGLE_WIDTH * 0.5f), fPosY - TIMELINE_CURRENTFRAME_TRIANGLE_HEIGHT);
	points[2] = QPointF(fPosX + (TIMELINE_CURRENTFRAME_TRIANGLE_WIDTH * 0.5f), fPosY - TIMELINE_CURRENTFRAME_TRIANGLE_HEIGHT);
	pPainter->drawPolygon(points, 3);
}

QList<EntityTreeItemData *> EntityDopeSheetView::GetItems() const
{
	// Gather all the entity items (root, children, shapes) into one list 'itemList'
	QList<EntityTreeItemData *> itemList, shapeList;
	static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().GetTreeItemData(itemList, shapeList);
	itemList += shapeList;
	itemList.prepend(static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().GetRootTreeItemData());

	return itemList;
}

int EntityDopeSheetView::GetNearestFrame(qreal fScenePosX) const
{
	qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * m_fZoom;
	int iNumSubLines = 4; // Either 0, 1, or 4

	return ((fScenePosX - TIMELINE_LEFT_MARGIN) + (fSubLineSpacing * 0.5f)) / fSubLineSpacing;
}

void EntityDopeSheetView::OnTweenAction()
{
	int i = 0;
	i++;
}

void EntityDopeSheetView::OnSelectAllItemKeyFrames()
{
	if(scene() == nullptr)
		return;

	static_cast<EntityDopeSheetScene *>(scene())->SelectAllItemKeyFrames(m_pContextClickItem);
}

void EntityDopeSheetView::OnDeselectItemKeyFrames()
{
	scene()->clearSelection();
}

void EntityDopeSheetView::OnDeleteKeyFrames()
{
}
