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
#include "IWgtTileSetItem.h"
#include "WgtTileSetAnimation.h"
#include "TileData.h"

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

void TileSetView::ResetCamera(TileSetPage ePage)
{
	if(GetScene() == nullptr)
		return;

	QPointF ptTopLeft;
	if(ePage == TILESETPAGE_Import)
		ptTopLeft = GetScene()->GetGfxImportBorderRect().scenePos();
	else
		ptTopLeft = GetScene()->GetGfxBorderRect().scenePos();

	ptTopLeft -= QPointF(32, 32);

	QSize vViewPortSize = viewport()->size();
	fitInView(QRectF(ptTopLeft, ptTopLeft + QPointF(vViewPortSize.width(), vViewPortSize.height())), Qt::KeepAspectRatio);
	scale(1.0f, 1.0f);
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

/*virtual*/ void TileSetView::showEvent(QShowEvent *pEvent) /*override*/
{
	if(m_pAuxTileSet->GetTileSet()->GetNumTiles() == 0)
		ResetCamera(TILESETPAGE_Import);
	else
		ResetCamera(TILESETPAGE_Arrange);

	QGraphicsView::showEvent(pEvent);
}

///*virtual*/ void TileSetView::drawBackground(QPainter *pPainter, const QRectF &rect) /*override*/
//{
//}
//
///*virtual*/ void TileSetView::drawForeground(QPainter *pPainter, const QRectF &rect) /*override*/
//{
//}

/*virtual*/ void TileSetView::mousePressEvent(QMouseEvent *pEvent) /*override*/
{
	CommonGfxView::mousePressEvent(pEvent);

	if(m_bMiddleMousePanning == false)
	{
		switch (m_pAuxTileSet->GetCurrentPage())
		{
		case TILESETPAGE_Import:
		case TILESETPAGE_Arrange:
			m_eDragState = DRAGSTATE_InitialPress;
			m_ptDragStart = pEvent->pos();
			setDragMode(QGraphicsView::RubberBandDrag);
			break;

		case TILESETPAGE_Animation:
			m_eDragState = DRAGSTATE_Painting;
			GetScene()->StartPaintStroke();
			GetScene()->OnPaintingStroke(*m_pAuxTileSet, mapToScene(pEvent->pos()), pEvent->buttons());
			setDragMode(QGraphicsView::NoDrag);
			break;

		case TILESETPAGE_Autotile: {
			TileData *pTile = GetScene()->IsPointInTile(mapToScene(pEvent->pos()));
			if(pTile && pTile->GetTerrainSet().isNull() == false)
			{
				m_eDragState = DRAGSTATE_Painting;
				GetScene()->StartPaintStroke();
				GetScene()->OnPaintingStroke(*m_pAuxTileSet, mapToScene(pEvent->pos()), pEvent->buttons());
				setDragMode(QGraphicsView::NoDrag);
			}
			else
			{
				m_eDragState = DRAGSTATE_InitialPress;
				setDragMode(QGraphicsView::RubberBandDrag);
			}
			m_ptDragStart = pEvent->pos();
			break; }

		case TILESETPAGE_Collision:
			break;
		case TILESETPAGE_CustomData:
			break;

		default:
			HyGuiLog("TileSetView::mousePressEvent - Unknown TileSet page!", LOGTYPE_Error);
			break;
		}
	}

	update();
}

/*virtual*/ void TileSetView::mouseMoveEvent(QMouseEvent *pEvent) /*override*/
{
	CommonGfxView::mouseMoveEvent(pEvent);

	switch (m_eDragState)
	{
	case DRAGSTATE_None:
		if(m_pAuxTileSet->GetCurrentPage() == TILESETPAGE_Autotile)
			GetScene()->HoverAutoTilePartAt(mapToScene(pEvent->pos()));
		break;

	case DRAGSTATE_MarqueeSelect:
		break;

	case DRAGSTATE_InitialPress: {
		QPointF dragDelta = pEvent->pos() - m_ptDragStart;
		if (dragDelta.manhattanLength() >= 3)
		{
			switch (m_pAuxTileSet->GetCurrentPage())
			{
			case TILESETPAGE_Import:
			case TILESETPAGE_Animation:
			case TILESETPAGE_Autotile:
				setDragMode(QGraphicsView::RubberBandDrag);
				m_eDragState = DRAGSTATE_MarqueeSelect;
				break;

			case TILESETPAGE_Arrange:
				TileSetGfxItem *pHoveredSetupItem = GetScene()->GetGfxTile(GetScene()->IsPointInTile(mapToScene(m_ptDragStart)));
				if ((pEvent->modifiers() & Qt::ShiftModifier) == 0 && pHoveredSetupItem)
				{
					if (pHoveredSetupItem->IsSelected() == false)
					{
						GetScene()->ClearSetupSelection();
						pHoveredSetupItem->SetSelected(true);
					}

					setDragMode(QGraphicsView::NoDrag);
					GetScene()->OnArrangingTilesMousePress(*m_pAuxTileSet, mapToScene(pEvent->pos()));
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
		GetScene()->OnArrangingTilesMouseMove(*m_pAuxTileSet, mapToScene(pEvent->pos()));
		break;

	case DRAGSTATE_Painting:
		GetScene()->OnPaintingStroke(*m_pAuxTileSet, mapToScene(pEvent->pos()), pEvent->buttons());
		break;
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
			GetScene()->OnMarqueeRelease(*m_pAuxTileSet, pEvent->button(), bShiftHeld, mapToScene(m_ptDragStart), mapToScene(ptOffset));
			break; }
		case DRAGSTATE_MarqueeSelect:
			GetScene()->OnMarqueeRelease(*m_pAuxTileSet, pEvent->button(), bShiftHeld, mapToScene(m_ptDragStart), mapToScene(pEvent->pos()));
			break;
		case DRAGSTATE_ArrangingSelection:
			GetScene()->OnArrangingTilesMouseRelease(*m_pAuxTileSet, mapToScene(pEvent->pos()));
			break;

		case DRAGSTATE_Painting:
			GetScene()->OnPaintStrokeRelease(*m_pAuxTileSet, pEvent->button());
			break;
		}
	}

	m_pAuxTileSet->UpdateGfxItemSelection();
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
