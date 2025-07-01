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
#include "AuxTileSet.h"

#include <QPainter>
#include <QScrollBar>
#include <QGraphicsItem>
#include <QMouseEvent>

TileSetView::TileSetView(QWidget *pParent /*= nullptr*/) :
	CommonGfxView(pParent),
	m_pAuxTileSet(nullptr),
	m_eDragState(DRAGSTATE_None),
	m_ptDragStart(0.0f, 0.0f),
	m_iDragFrame(-1)
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

	if(DRAGSTATE_Dragging == m_eDragState)
	{
		update();
	}
	else if(DRAGSTATE_InitialPress == m_eDragState)
	{
		QPointF dragDelta = pEvent->pos() - m_ptDragStart;
		if(dragDelta.manhattanLength() >= 3)
		{
			m_eDragState = DRAGSTATE_Dragging;
		}
		update();
	}

	if(rubberBandRect().isNull() == false)
		update();
}

/*virtual*/ void TileSetView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	//if(pEvent->button() != Qt::LeftButton)
	//{
	//	QGraphicsView::mousePressEvent(pEvent);
	//	return;
	//}

	QGraphicsItem *pItemUnderMouse = itemAt(pEvent->pos());
	if(pItemUnderMouse)
	{
		m_eDragState = DRAGSTATE_InitialPress;
		m_ptDragStart = pEvent->pos();
		
		pItemUnderMouse->setSelected(true);
	}

	// TODO: Swap control and shift modifiers when QGraphicsView takes the wheel
	QGraphicsView::mousePressEvent(pEvent);
	update();
}

/*virtual*/ void TileSetView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
{
	if(DRAGSTATE_Dragging == m_eDragState)
	{
		GetScene()->OnMarqueeRelease(pEvent->button(), mapToScene(m_ptDragStart), mapToScene(pEvent->pos()));
		m_pAuxTileSet->UpdateSelection();
	}
	else if(DRAGSTATE_InitialPress == m_eDragState)
	{
		QPoint ptOffset = pEvent->pos();
		ptOffset.setX(ptOffset.x() + 1);
		ptOffset.setY(ptOffset.y() + 1);
		GetScene()->OnMarqueeRelease(pEvent->button(), mapToScene(pEvent->pos()), mapToScene(ptOffset));
		m_pAuxTileSet->UpdateSelection();
	}

	m_eDragState = DRAGSTATE_None;

	QGraphicsView::mouseReleaseEvent(pEvent);
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
