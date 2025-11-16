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
			switch (m_pAuxTileSet->GetCurrentPage())
			{
			case TILESETPAGE_Import:
				setDragMode(QGraphicsView::RubberBandDrag);
				m_eDragState = DRAGSTATE_MarqueeSelect;
				break;

			case TILESETPAGE_Arrange:
				TileSetGfxItem* pHoveredSetupItem = GetScene()->GetSetupTileAt(mapToScene(m_ptDragStart));
				if ((pEvent->modifiers() & Qt::ShiftModifier) == 0 && pHoveredSetupItem)
				{
					if (pHoveredSetupItem->IsSelected() == false)
					{
						GetScene()->ClearSetupSelection();
						pHoveredSetupItem->SetSelected(true);
					}

					setDragMode(QGraphicsView::NoDrag);
					GetScene()->OnArrangingTilesMousePress(mapToScene(pEvent->pos()));
					m_eDragState = DRAGSTATE_ArrangingSelection;
				}
				else
				{
					setDragMode(QGraphicsView::RubberBandDrag);
					m_eDragState = DRAGSTATE_MarqueeSelect;
				}
				break;
			}
		}
		break; }
	
	case DRAGSTATE_ArrangingSelection:
		GetScene()->OnArrangingTilesMouseMove(mapToScene(pEvent->pos()));
		break;
	}
	update();

	//if(rubberBandRect().isNull() == false)
	//	update();
}

/*virtual*/ void TileSetView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	CommonGfxView::mousePressEvent(pEvent);

	if(m_bMiddleMousePanning == false)
	{
		switch (m_pAuxTileSet->GetCurrentPage())
		{
		case TILESETPAGE_Import:
			break;
		case TILESETPAGE_Arrange:
			break;
		case TILESETPAGE_Animation:
			break;
		case TILESETPAGE_Autotile:
			break;
		case TILESETPAGE_Collision:
			break;
		case TILESETPAGE_CustomData:
			break;

		default:
			HyGuiLog("TileSetView::mousePressEvent - Unknown TileSet page!", LOGTYPE_Error);
			break;
		}

		m_eDragState = DRAGSTATE_InitialPress;
		m_ptDragStart = pEvent->pos();

		setDragMode(QGraphicsView::RubberBandDrag);
	}

	
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
			GetScene()->OnMarqueeRelease(m_pAuxTileSet->GetCurrentPage(), pEvent->button(), bShiftHeld, mapToScene(pEvent->pos()), mapToScene(ptOffset));
			break; }
		case DRAGSTATE_MarqueeSelect:
			GetScene()->OnMarqueeRelease(m_pAuxTileSet->GetCurrentPage(), pEvent->button(), bShiftHeld, mapToScene(m_ptDragStart), mapToScene(pEvent->pos()));
			break;
		case DRAGSTATE_ArrangingSelection:
			GetScene()->OnArrangingTilesMouseRelease(*m_pAuxTileSet, mapToScene(pEvent->pos()));
			break;
		}
	}

	m_pAuxTileSet->UpdateSelection();
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
