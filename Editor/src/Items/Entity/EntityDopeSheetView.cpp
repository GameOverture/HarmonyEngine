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
#include "AuxDopeSheet.h"

#include <QPainter>
#include <QScrollBar>
#include <QGraphicsItem>
#include <QMouseEvent>

EntityDopeSheetView::EntityDopeSheetView(QWidget *pParent /*= nullptr*/) :
	QGraphicsView(pParent),
	m_pAuxDopeSheet(nullptr),
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
	setAlignment(Qt::AlignLeft | Qt::AlignTop);
	setDragMode(QGraphicsView::RubberBandDrag);
}

/*virtual*/ EntityDopeSheetView::~EntityDopeSheetView()
{
}

EntityDopeSheetScene *EntityDopeSheetView::GetScene() const
{
	return static_cast<EntityDopeSheetScene *>(scene());
}

void EntityDopeSheetView::SetScene(AuxDopeSheet *pAuxDopeSheet, EntityStateData *pStateData)
{
	m_pAuxDopeSheet = pAuxDopeSheet;

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

EntityTreeItemData *EntityDopeSheetView::GetContextClickItem()
{
	return m_pContextClickItem;
}

/*virtual*/ void EntityDopeSheetView::contextMenuEvent(QContextMenuEvent *pEvent) /*override*/
{
	if(m_pStateData == nullptr)
		return;

	m_pContextClickItem = nullptr;
	QPointF ptScenePos = mapToScene(pEvent->pos());

	qreal fPosY = TIMELINE_HEIGHT + 1.0f;

	if(pEvent->pos().y() > fPosY)
	{
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
				propList = GetScene()->GetUniquePropertiesList(pEntItemData, true);

				for(QPair<QString, QString> &propPair : propList)
				{
					fPosY += ITEMS_LINE_HEIGHT;
					if(ptScenePos.y() < fPosY)
					{
						m_pContextClickItem = pEntItemData;
						break;
					}
				}
			}
			if(m_pContextClickItem)
				break;
		}
	}

	bool bOnTimeline = pEvent->pos().y() <= (TIMELINE_HEIGHT + 1.0f);
	QMenu *pNewMenu = m_pAuxDopeSheet->AllocContextMenu(bOnTimeline, m_pContextClickItem, GetNearestFrame(ptScenePos.x()));
	pNewMenu->exec(pEvent->globalPos());
	delete pNewMenu;
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
		painter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetCurFrameIndicator));
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
		QPointF ptSceneDragStart = mapToScene(m_ptDragStart);
		int iFrameOffset = m_iDragFrame - GetNearestFrame(ptSceneDragStart.x());
		qreal fSubLineSpacing = TIMELINE_NOTCH_SUBLINES_WIDTH * m_fZoom;

		QList<QGraphicsItem *> itemList = items();
		QAbstractGraphicsShapeItem *pGfxItem = nullptr; // Just need one item to get the pen and brush
		QVector<QRectF> rectList;

		if(m_pGfxDragTweenKnobItem)
			pGfxItem = m_pGfxDragTweenKnobItem;
		else
		{
			for(QGraphicsItem *pItem : itemList)
			{
				if(pItem->isSelected())
				{
					QRectF rect = pItem->sceneBoundingRect();
					rect.translate(fSubLineSpacing * iFrameOffset, 0.0f);

					rectList.push_back(rect);
					pGfxItem = static_cast<QAbstractGraphicsShapeItem *>(pItem);
				}
			}
		}

		if(pGfxItem)
		{
			pPainter->setPen(pGfxItem->pen());
			QBrush brush = pGfxItem->brush();
			QColor color = brush.color();
			color.setAlphaF(0.5f);
			brush.setColor(color);
			pPainter->setBrush(brush);

			if(m_pGfxDragTweenKnobItem)
			{
				QRectF rect = m_pGfxDragTweenKnobItem->sceneBoundingRect();
				rect.translate(fSubLineSpacing * iFrameOffset, 0.0f);
				pPainter->drawEllipse(rect);
			}
			else
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
		HyColor textColor = HyGlobal::GetEditorColor(EDITORCOLOR_DopeSheetText);

		// Determine number of rows of key frames
		int iNumRows = 1;
		QList<QPair<QString, QString>> propList;
		if(pEntItemData->IsSelected())
		{
			textColor = HyGlobal::GetEditorColor(EDITORCOLOR_DopeSheetTextSelected);

			propList = GetScene()->GetUniquePropertiesList(pEntItemData, true);
			iNumRows += propList.size();
		}

		// Background Rect
		pPainter->setPen(Qt::NoPen);
		pPainter->setBrush(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetItemsColumn));
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
			textColor = HyGlobal::GetEditorColor(EDITORCOLOR_DopeSheetTextHover);
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
	pPainter->setBrush(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetTimeline));
	pPainter->drawRect(QRectF(rect.x(), rect.y(), rect.width(), TIMELINE_HEIGHT));

	pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetNotch));
	pPainter->drawLine(rect.x(), rect.y() + TIMELINE_HEIGHT, rect.x() + rect.width(), rect.y() + TIMELINE_HEIGHT);

	std::function<void(int, float)> fpPaintEvent = [&](int iFrameIndex, float fX)
	{
		if(GetScene()->GetCallbackList(iFrameIndex).empty() == false)
		{
			pPainter->translate(fX, rect.y() + TIMELINE_HEIGHT - (CALLBACK_DIAMETER * 0.5f));
			pPainter->setPen(Qt::NoPen);
			pPainter->setBrush(HyGlobal::ConvertHyColor(HyColor::Orange));

			pPainter->rotate(45.0);
			pPainter->drawRect(CALLBACK_DIAMETER * -0.5, CALLBACK_DIAMETER * -0.5, CALLBACK_DIAMETER, CALLBACK_DIAMETER);
			pPainter->resetTransform();
		}

		EntityTreeItemData *pRootTreeItemData = static_cast<EntityModel &>(m_pStateData->GetModel()).GetTreeModel().GetRootTreeItemData();
		QJsonValue pauseValue = GetScene()->GetKeyFrameProperty(pRootTreeItemData, iFrameIndex, "Timeline", "Pause");
		if(pauseValue.isBool() && pauseValue.toBool())
		{
			QSize iconSize(16, 16);
			QIcon pauseIcon(":/icons16x16/media-pause.png");
			QPoint pt = QPoint(fX - (iconSize.width() / 2) + 1.0f, rect.y() + TIMELINE_HEIGHT - (CALLBACK_DIAMETER * 0.5f) - iconSize.width());
			pPainter->drawPixmap(pt, pauseIcon.pixmap(iconSize.width(), iconSize.height()));
		}

		QJsonValue frameValue = GetScene()->GetKeyFrameProperty(pRootTreeItemData, iFrameIndex, "Timeline", "Frame");
		if(frameValue.isUndefined() == false && frameValue.isNull() == false)
		{
			QSize iconSize(16, 16);
			QPoint pt = QPoint(fX - (iconSize.width() / 2) + 1.0f, rect.y() + TIMELINE_HEIGHT - (CALLBACK_DIAMETER * 0.5f) - iconSize.width());
			int iGotoFrameIndex = frameValue.toInt();
			if(iGotoFrameIndex > iFrameIndex)
			{
				QIcon gotoIcon(":/icons16x16/media-forward.png");
				pPainter->drawPixmap(pt, gotoIcon.pixmap(iconSize.width(), iconSize.height()));
			}
			else
			{
				QIcon gotoIcon(":/icons16x16/media-rewind.png");
				pPainter->drawPixmap(pt, gotoIcon.pixmap(iconSize.width(), iconSize.height()));
			}
		}

		QJsonValue stateValue = GetScene()->GetKeyFrameProperty(pRootTreeItemData, iFrameIndex, "Timeline", "State");
		if(stateValue.isUndefined() == false && stateValue.isNull() == false)
		{
			QSize iconSize(16, 16);
			QIcon gotoIcon(":/icons16x16/items/Entity.png");
			QPoint pt = QPoint(fX - (iconSize.width() / 2) + 1.0f, rect.y() + TIMELINE_HEIGHT - (CALLBACK_DIAMETER * 0.5f) - iconSize.width());
			pPainter->drawPixmap(pt, gotoIcon.pixmap(iconSize.width(), iconSize.height()));
		}
	};

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
			HyColor color = HyGlobal::GetEditorColor(EDITORCOLOR_DopeSheetNotch);

			if(GetScene()->GetCurrentFrame() == iFrameIndex)
			{
				color = HyGlobal::GetEditorColor(EDITORCOLOR_DopeSheetCurFrameIndicator);
				DrawCurrentFrameIndicator(pPainter, fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT);
				pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetCurFrameIndicator));
			}
			else
				pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetNotch));
			
			pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);

			// Main Notch Keyframe Text
			const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(iFrameIndex));
			QRectF textRect(fPosX - (fTextWidth * 0.5f), rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f, fTextWidth, 20.0f);
			DrawShadowText(pPainter, textRect, QString::number(iFrameIndex), color);

			// Draw timeline events
			fpPaintEvent(iFrameIndex, fPosX);
		}

		// Sub Notch Lines
		pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetNotch));
		for(int i = 0; i < iNumSubLines; ++i)
		{
			fPosX += fSubLineSpacing;
			if(fPosX >= fPOSX_DRAW_THRESHOLD)
			{
				int iCurSubNotchFrame = (iFrameIndex + 1) + i;
				if(GetScene()->GetCurrentFrame() == iCurSubNotchFrame)
				{
					pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetCurFrameIndicator));

					const float fTextWidth = pPainter->fontMetrics().horizontalAdvance(QString::number(iCurSubNotchFrame));
					QRectF textRect(fPosX - (fTextWidth * 0.5f), rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_MAINLINE_HEIGHT - 20.0f, fTextWidth, 20.0f);
					DrawShadowText(pPainter, textRect, QString::number(iCurSubNotchFrame), HyGlobal::GetEditorColor(EDITORCOLOR_DopeSheetCurFrameIndicator));

					DrawCurrentFrameIndicator(pPainter, fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT);
					pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetCurFrameIndicator));
				}
				else
					pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetNotch));

				pPainter->drawLine(fPosX, rect.y() + TIMELINE_HEIGHT - TIMELINE_NOTCH_SUBLINES_HEIGHT, fPosX, rect.y() + TIMELINE_HEIGHT);

				// Draw timeline events
				fpPaintEvent(iCurSubNotchFrame, fPosX);
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

		if(pEvent->type() == QEvent::HoverEnter)
			setFocus();
		else if(pEvent->type() == QEvent::HoverLeave)
			clearFocus();
	}

	return QGraphicsView::event(pEvent);
}

/*virtual*/ void EntityDopeSheetView::keyPressEvent(QKeyEvent *pEvent) /*override*/
{
	// WASD to pan the timeline
	if(pEvent->key() == Qt::Key_A)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 10);
	}
	else if(pEvent->key() == Qt::Key_D)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 10);
	}
	else if(pEvent->key() == Qt::Key_W)
	{
		verticalScrollBar()->setValue(verticalScrollBar()->value() - 10);
	}
	else if(pEvent->key() == Qt::Key_S)
	{
		verticalScrollBar()->setValue(verticalScrollBar()->value() + 10);
	}
	//else if(pEvent->key() == Qt::Key_Space)
	//{
	//	GetScene()->TogglePlay();
	//}
	else if(pEvent->key() == Qt::Key_Left)
	{
		GetScene()->SetCurrentFrame(GetScene()->GetCurrentFrame() - 1);
	}
	else if(pEvent->key() == Qt::Key_Right)
	{
		GetScene()->SetCurrentFrame(GetScene()->GetCurrentFrame() + 1);
	}
	//else if(pEvent->key() == Qt::Key_Delete)
	//{
	//	GetScene()->DeleteSelectedEvents();
	//}
	//else if(pEvent->key() == Qt::Key_Escape)
	//{
	//	GetScene()->ClearSelectedEvents();
	//}
}

/*virtual*/ void EntityDopeSheetView::keyReleaseEvent(QKeyEvent *pEvent) /*override*/
{
}

/*virtual*/ void EntityDopeSheetView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	m_MouseScenePos = mapToScene(pEvent->pos());

	if(m_bTimeLineMouseDown)
	{
		GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));
		if(m_pAuxDopeSheet)
			m_pAuxDopeSheet->UpdateWidgets();

		// If 'm_MouseScenePos' is out of view, then scroll the view
		ensureVisible(m_MouseScenePos.x(), m_MouseScenePos.y(), 1, 1, 0, 0);


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
		if(m_pAuxDopeSheet)
			m_pAuxDopeSheet->UpdateWidgets();

		pEvent->accept();
	}
	else if(pEvent->pos().y() > TIMELINE_HEIGHT)
	{	
		QGraphicsItem *pItemUnderMouse = itemAt(pEvent->pos());
		if(pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f && pItemUnderMouse)
		{
			m_eDragState = DRAGSTATE_InitialPress;
			m_ptDragStart = pEvent->pos();

			if(pItemUnderMouse->data(GFXDATAKEY_Type).toInt() == GFXITEM_TweenKnob)
			{
				//GetScene()->clearSelection();
				m_pGfxDragTweenKnobItem = static_cast<GraphicsTweenKnobItem *>(pItemUnderMouse);
				m_pGfxDragTweenKnobItem->setSelected(true);
			}
		}

		// Only want default selection behavior when NOT clicking in the 'time line' or an 'item name' column AND it's not a tween knob
		//if(pItemUnderMouse == nullptr || pItemUnderMouse->data(GFXDATAKEY_Type).toInt() != GFXITEM_TweenKnob)
		{
			GetScene()->ClearSelectionPivot();
			// TODO: Swap control and shift modifiers when QGraphicsView takes the wheel
			QGraphicsView::mousePressEvent(pEvent);
		}
	}

	update();
}

/*virtual*/ void EntityDopeSheetView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	m_bTimeLineMouseDown = false;

	if(DRAGSTATE_Dragging == m_eDragState)
	{
		if(m_pGfxDragTweenKnobItem)
		{
			EntityTreeItemData *pTreeItemData = m_pGfxDragTweenKnobItem->parentItem()->data(GFXDATAKEY_TreeItemData).value<EntityTreeItemData *>();
			TweenProperty eTweenProp = HyGlobal::GetTweenPropFromString(m_pGfxDragTweenKnobItem->parentItem()->data(GFXDATAKEY_CategoryPropString).toString().split('/')[1]);
			int iTweenStartKeyFrame = m_pGfxDragTweenKnobItem->parentItem()->data(GFXDATAKEY_FrameIndex).toInt();
			int iTweenEndKeyFrame = HyMath::Max(iTweenStartKeyFrame, m_iDragFrame);

			double dNewDuration = (iTweenEndKeyFrame - iTweenStartKeyFrame) * (1.0 / static_cast<EntityModel &>(m_pStateData->GetModel()).GetFramesPerSecond());
			EntityUndoCmd_NudgeTweenDuration *pCmd = new EntityUndoCmd_NudgeTweenDuration(m_pStateData->GetDopeSheetScene(), pTreeItemData, iTweenStartKeyFrame, eTweenProp, dNewDuration);
			m_pStateData->GetModel().GetItem().GetUndoStack()->push(pCmd);
		}
		else
		{
			QPointF ptSceneDragStart = mapToScene(m_ptDragStart);
			EntityUndoCmd_NudgeSelectedKeyFrames *pCmd = new EntityUndoCmd_NudgeSelectedKeyFrames(m_pStateData->GetDopeSheetScene(), GetNearestFrame(m_MouseScenePos.x()) - GetNearestFrame(ptSceneDragStart.x()));
			m_pStateData->GetModel().GetItem().GetUndoStack()->push(pCmd);
		}
	}
	else if(rubberBandRect().isNull() && pEvent->pos().x() > TIMELINE_LEFT_MARGIN - 5.0f)
		GetScene()->SetCurrentFrame(GetNearestFrame(m_MouseScenePos.x()));

	m_pGfxDragTweenKnobItem = nullptr;
	m_eDragState = DRAGSTATE_None;

	update();
	QGraphicsView::mouseReleaseEvent(pEvent);

	if(m_pAuxDopeSheet)
		m_pAuxDopeSheet->UpdateWidgets();
}

void EntityDopeSheetView::DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor textColor)
{
	textRect.translate(1.0f, 1.0f);
	pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetTextShadow));
	pPainter->drawText(textRect, sText);
	textRect.translate(-1.0f, -1.0f);
	pPainter->setPen(HyGlobal::ConvertHyColor(textColor));
	pPainter->drawText(textRect, sText);
}

void EntityDopeSheetView::DrawCurrentFrameIndicator(QPainter *pPainter, qreal fPosX, qreal fPosY)
{
	HyColor color = HyGlobal::GetEditorColor(EDITORCOLOR_DopeSheetCurFrameIndicator);
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
