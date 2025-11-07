/**************************************************************************
*	TileSetView.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TileSetView.h"
#include "TileSetScene.h"
#include "TileSetGfxItem.h"
#include "AuxTileSet.h"

#include <QPainter>
#include <QScrollBar>
#include <QGraphicsItem>
#include <QMouseEvent>

TileSetView::TileSetView(QWidget *pParent /*= nullptr*/) :
	CommonGfxView(pParent),
	m_pAuxTileSet(nullptr),
	m_eDragState(DRAGSTATE_None),
	m_ptDragStart(0.0f, 0.0f)
{
	setDragMode(QGraphicsView::RubberBandDrag);
}

/*virtual*/ TileSetView::~TileSetView()
{
}

TileSetScene *TileSetView::GetScene() const
{
	return static_cast<TileSetScene *>(scene());
}

void TileSetView::SetScene(AuxTileSet *pAuxTileSet, TileSetScene *pTileSetScene)
{
	m_pAuxTileSet = pAuxTileSet;
	setScene(pTileSetScene);
}

/*virtual*/ void TileSetView::contextMenuEvent(QContextMenuEvent *pEvent) /*override*/
{
	if(m_pAuxTileSet == nullptr)
	{
		CommonGfxView::contextMenuEvent(pEvent);
		return;
	}

	//QPointF ptScenePos = mapToScene(pEvent->pos());
	//QMenu *pNewMenu = m_pAuxTileSet->AllocContextMenu();
	//pNewMenu->exec(pEvent->globalPos());
	//delete pNewMenu;
	CommonGfxView::contextMenuEvent(pEvent);
}

///*virtual*/ void TileSetView::drawBackground(QPainter *pPainter, const QRectF &rect) /*override*/
//{
//}
//
///*virtual*/ void TileSetView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
//{
//}

/*virtual*/ void TileSetView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	CommonGfxView::mouseMoveEvent(pEvent);

	switch (m_eDragState)
	{
	case DRAGSTATE_None:
	case DRAGSTATE_MarqueeSelect:
		break;

	case DRAGSTATE_InitialPress: {
		QPointF dragDelta = pEvent->pos() - m_ptDragStart;
		if (dragDelta.manhattanLength() >= 3)
		{
			if (GetScene()->GetDisplayMode() == TILESETMODE_Setup)
			{
				TileSetGfxItem* pHoveredSetupItem = GetScene()->GetSetupTileAt(mapToScene(m_ptDragStart));
				if ((pEvent->modifiers() & Qt::ShiftModifier) == 0 && pHoveredSetupItem)
				{
					if (pHoveredSetupItem->IsSelected() == false)
					{
						GetScene()->ClearSetupSelection();
						pHoveredSetupItem->SetSelected(true);
					}

					setDragMode(QGraphicsView::NoDrag);
					GetScene()->OnDraggingTilesMousePress(mapToScene(pEvent->pos()));
					m_eDragState = DRAGSTATE_DraggingSelection;
				}
				else
				{
					setDragMode(QGraphicsView::RubberBandDrag);
					m_eDragState = DRAGSTATE_MarqueeSelect;
				}
			}
			else
			{
				setDragMode(QGraphicsView::RubberBandDrag);
				m_eDragState = DRAGSTATE_MarqueeSelect;
			}
		}
		break; }
	
	case DRAGSTATE_DraggingSelection:
		GetScene()->OnDraggingTilesMouseMove(mapToScene(pEvent->pos()));
		break;
	}
	update();

	//if(rubberBandRect().isNull() == false)
	//	update();
}

/*virtual*/ void TileSetView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_bMiddleMousePanning == false)
	{
		m_eDragState = DRAGSTATE_InitialPress;
		m_ptDragStart = pEvent->pos();

		setDragMode(QGraphicsView::RubberBandDrag);
	}

	CommonGfxView::mousePressEvent(pEvent);
	update();
}

/*virtual*/ void TileSetView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	if(m_bMiddleMousePanning == false)
	{
		bool bShiftHeld = (pEvent->modifiers() & Qt::ShiftModifier);
		switch (m_eDragState)
		{
		case DRAGSTATE_None:
			break;
		case DRAGSTATE_InitialPress: {
			QPoint ptOffset = m_ptDragStart;
			ptOffset.setX(ptOffset.x() + 1);
			ptOffset.setY(ptOffset.y() + 1);
			GetScene()->OnMarqueeRelease(pEvent->button(), bShiftHeld, mapToScene(pEvent->pos()), mapToScene(ptOffset));
			break; }
		case DRAGSTATE_MarqueeSelect:
			GetScene()->OnMarqueeRelease(pEvent->button(), bShiftHeld, mapToScene(m_ptDragStart), mapToScene(pEvent->pos()));
			break;
		case DRAGSTATE_DraggingSelection:
			GetScene()->OnDraggingTilesMouseRelease(mapToScene(pEvent->pos()));
			break;
		}
	}

	m_pAuxTileSet->UpdateImportSelection();
	m_eDragState = DRAGSTATE_None;

	CommonGfxView::mouseReleaseEvent(pEvent);
	update();
}

void TileSetView::DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor textColor)
{
	textRect.translate(1.0f, 1.0f);
	pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetTextShadow));
	pPainter->drawText(textRect, sText);
	textRect.translate(-1.0f, -1.0f);
	pPainter->setPen(HyGlobal::ConvertHyColor(textColor));
	pPainter->drawText(textRect, sText);
}
