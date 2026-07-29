/**************************************************************************
*	TileMapGfxView.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "TileMapGfxView.h"
#include "TileMapGfxScene.h"

#include <QMouseEvent>

TileMapGfxView::TileMapGfxView(QWidget *pParent /*= nullptr*/) :
	CommonGfxView(pParent),
	m_eDragState(DRAGSTATE_None),
	m_ptDragStart(0.0f, 0.0f)
{
	setDragMode(QGraphicsView::RubberBandDrag);
}

/*virtual*/ TileMapGfxView::~TileMapGfxView()
{
}

TileMapGfxScene *TileMapGfxView::GetScene() const
{
	return static_cast<TileMapGfxScene *>(scene());
}

//void TileMapGfxView::SetScene(TileMapGfxScene *pTileSetScene)
//{
//	setScene(pTileSetScene);
//}

/*virtual*/ void TileMapGfxView::contextMenuEvent(QContextMenuEvent *pEvent) /*override*/
{
	//QPointF ptScenePos = mapToScene(pEvent->pos());
	//QMenu *pNewMenu = m_pAuxTileSet->AllocContextMenu();
	//pNewMenu->exec(pEvent->globalPos());
	//delete pNewMenu;
	CommonGfxView::contextMenuEvent(pEvent);
}

/*virtual*/ void TileMapGfxView::showEvent(QShowEvent *pEvent) /*override*/
{
	QGraphicsView::showEvent(pEvent);
}

///*virtual*/ void TileMapGfxView::drawBackground(QPainter *pPainter, const QRectF &rect) /*override*/
//{
//}
//
///*virtual*/ void TileMapGfxView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
//{
//}

/*virtual*/ void TileMapGfxView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	CommonGfxView::mousePressEvent(pEvent);

	if(m_bMiddleMousePanning == false)
	{
		m_eDragState = DRAGSTATE_InitialPress;
		m_ptDragStart = pEvent->pos();
	}

	update();
}

/*virtual*/ void TileMapGfxView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	CommonGfxView::mouseMoveEvent(pEvent);

	switch(m_eDragState)
	{
	case DRAGSTATE_None:
		break;

	case DRAGSTATE_MarqueeSelect:
		break;

	case DRAGSTATE_InitialPress: {
		QPointF dragDelta = pEvent->pos() - m_ptDragStart;
		if (dragDelta.manhattanLength() >= MANHATTAN_DRAG_THRESHOLD)
			m_eDragState = DRAGSTATE_MarqueeSelect;
		break; }
	}
	
	update();
}

/*virtual*/ void TileMapGfxView::mouseReleaseEvent(QMouseEvent *pEvent) /*override*/
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
			GetScene()->OnMarqueeRelease(pEvent->button(), bShiftHeld, mapToScene(m_ptDragStart), mapToScene(ptOffset));
			break; }
		case DRAGSTATE_MarqueeSelect:
			GetScene()->OnMarqueeRelease(pEvent->button(), bShiftHeld, mapToScene(m_ptDragStart), mapToScene(pEvent->pos()));
			break;
		}
	}

	m_eDragState = DRAGSTATE_None;

	CommonGfxView::mouseReleaseEvent(pEvent);
	update();
}

void TileMapGfxView::DrawShadowText(QPainter *pPainter, QRectF textRect, const QString &sText, HyColor textColor)
{
	textRect.translate(1.0f, 1.0f);
	pPainter->setPen(HyGlobal::GetEditorQtColor(EDITORCOLOR_DopeSheetTextShadow));
	pPainter->drawText(textRect, sText);
	textRect.translate(-1.0f, -1.0f);
	pPainter->setPen(HyGlobal::ConvertHyColor(textColor));
	pPainter->drawText(textRect, sText);
}
