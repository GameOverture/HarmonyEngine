/**************************************************************************
 *	TileGfxItem.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileSetGfxItem.h"
#include "AtlasTileSet.h"
#include "TileData.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

const int iTILE_PADDING = 0;

TileSetGfxItem::TileSetGfxItem(const QPixmap& pixmapRef, const QPolygonF& outlinePolygon) :
	QGraphicsItem(nullptr),
	m_SelectedPen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::Orange)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)),
	m_UnselectedPen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::Black)), 1.0f, Qt::DashLine, Qt::SquareCap, Qt::MiterJoin)),
	m_DraggedPen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::White)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)),
	m_ShapePen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::Orange)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin)),
	m_bSelected(true),
	m_bDragged(false),
	m_pRectItem(nullptr),
	m_pPixmapItem(nullptr),
	m_pShapeItem(nullptr),
	m_ptDraggingInitialPos(0.0f, 0.0f),
	m_ptDraggingGridPos(0, 0)
{
	QRectF rect(pixmapRef.width() * -0.5f, pixmapRef.height() * -0.5f, pixmapRef.width() + 1, pixmapRef.height() + 1);

	m_pAnimationRectItem = new QGraphicsRectItem(rect, this);
	m_pAnimationRectItem->setPen(QPen(QBrush(HyGlobal::ConvertHyColor(HyColor::Yellow)), 2.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
	m_pAnimationRectItem->setVisible(false);

	m_pRectItem = new QGraphicsRectItem(rect, this);
	m_pRectItem->setPen(m_SelectedPen);

	m_pPixmapItem = new QGraphicsPixmapItem(pixmapRef, this);
	m_pPixmapItem->setPos(pixmapRef.width() * -0.5f, pixmapRef.height() * -0.5f);

	m_pShapeItem = new QGraphicsPolygonItem(outlinePolygon, this);
	m_pShapeItem->setPen(m_ShapePen);

	for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
		m_pTerrainParts[i] = nullptr;

	setAcceptHoverEvents(true);
	//setAcceptedMouseButtons(Qt::NoButton);
}

/*virtual*/ TileSetGfxItem::~TileSetGfxItem()
{
	delete m_pRectItem;
	delete m_pPixmapItem;
	delete m_pShapeItem;

	for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
		delete m_pTerrainParts[i];
}

void TileSetGfxItem::Refresh(QSize regionSize, AtlasTileSet *pTileSet, TileSetPage ePage, TileData *pTileData)
{
	// Local origin is the center of the atlas region box
	QRectF rect(regionSize.width() * -0.5f, regionSize.height() * -0.5f, regionSize.width() + 1, regionSize.height() + 1);

	m_pRectItem->setRect(rect);
	m_pShapeItem->setPolygon(pTileSet->GetTilePolygon());
	m_pShapeItem->setPos(pTileSet->GetTileOffset());

	if(m_pTerrainParts[0])
	{
		for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
			m_pTerrainParts[i]->hide();
	}

	m_pShapeItem->setVisible(true);// m_bSelected);
	setOpacity(1.0f);

	switch(ePage)
	{
	case TILESETPAGE_Import:
	case TILESETPAGE_Arrange:
		break;

	case TILESETPAGE_Animation:
		break;

	case TILESETPAGE_Autotile:
		if(pTileSet->GetTerrainSetType(pTileData->GetTerrainSet()) == AUTOTILETYPE_Unknown ||
		   pTileSet->GetTileShape() == TILESETSHAPE_Unknown)
		{
			setOpacity(0.4f); // Darken *this (QGraphicsItem)
			break;
		}

		for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
		{
			QUuid terrainPartUuid = pTileData->GetTerrain(static_cast<TileSetAutoTilePart>(i));
			if(terrainPartUuid.isNull())
				continue;

			m_pTerrainParts[i]->setBrush(QBrush(HyGlobal::ConvertHyColor(pTileSet->GetTerrainColor(terrainPartUuid))));
			m_pTerrainParts[i]->show();
		}
		break;

	case TILESETPAGE_Collision:
	case TILESETPAGE_CustomData:
		break;

	default:
		HyGuiLog("TileSetGfxItem::Refresh: Unhandled TileSetPage enum value!", LOGTYPE_Error);
		break;
	}
}

bool TileSetGfxItem::IsSelected() const
{
	return m_bSelected;
}

void TileSetGfxItem::SetSelected(bool bSelected)
{
	m_bSelected = bSelected;

	if(m_bSelected)
		m_pRectItem->setPen(m_SelectedPen);
	else
		m_pRectItem->setPen(m_UnselectedPen);
}

void TileSetGfxItem::SetAsDragged(bool bDragged)
{
	m_bDragged = bDragged;

	if (m_bDragged)
	{
		m_pRectItem->setPen(m_DraggedPen);
		setOpacity(0.5f);
	}
	else
	{
		if (m_bSelected)
			m_pRectItem->setPen(m_SelectedPen);
		else
			m_pRectItem->setPen(m_UnselectedPen);

		setOpacity(1.0f);
	}
}

QPointF TileSetGfxItem::GetDraggingInitialPos() const
{
	return m_ptDraggingInitialPos;
}

void TileSetGfxItem::SetDraggingInitialPos(QPointF ptInitialPos)
{
	m_ptDraggingInitialPos = ptInitialPos;
}

QPoint TileSetGfxItem::GetDraggingGridPos() const
{
	return m_ptDraggingGridPos;
}

void TileSetGfxItem::SetDraggingGridPos(QPoint ptGridPos)
{
	m_ptDraggingGridPos = ptGridPos;
}

QPixmap TileSetGfxItem::GetPixmap() const
{
	return m_pPixmapItem->pixmap();
}

/*virtual*/ QRectF TileSetGfxItem::boundingRect() const /*override*/
{
	return m_pRectItem->rect();// .adjusted(-iTILE_PADDING, -iTILE_PADDING, iTILE_PADDING, iTILE_PADDING); // Adjust for selection border
}

/*virtual*/ void TileSetGfxItem::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) /*override*/
{
	//QGraphicsPixmapItem::paint(pPainter, option, widget);
	//if(IsSelected())
	//{
	//	pPainter->setPen(Qt::DashLine);
	//	pPainter->drawRect(boundingRect());
	//}
}

void TileSetGfxItem::SetAnimation(bool bShow, HyColor color)
{
	m_pAnimationRectItem->setBrush(HyGlobal::ConvertHyColor(color));
	m_pAnimationRectItem->setVisible(bShow);
}

void TileSetGfxItem::AllocateAutoTileParts(AtlasTileSet *pTileSet, AutoTileType eAutoTileType, TileSetShape eTileSetShape)
{
	for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
	{
		delete m_pTerrainParts[i];
		m_pTerrainParts[i] = new QGraphicsPolygonItem(this);
		m_pTerrainParts[i]->setPen(QPen(Qt::NoPen));
		m_pTerrainParts[i]->setPos(pTileSet->GetTileOffset());

		QPolygonF partPolygon = AssembleAutoTilePolygon(eAutoTileType, eTileSetShape, static_cast<TileSetAutoTilePart>(i));
		m_pTerrainParts[i]->setPolygon(partPolygon);
		m_pTerrainParts[i]->hide();
	}
}

QGraphicsPolygonItem *TileSetGfxItem::GetAutoTilePartAt(QPointF ptLocalPos)
{
	for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
	{
		if(m_pTerrainParts[i]->contains(ptLocalPos))
			return m_pTerrainParts[i];
	}

	return nullptr;
}

QPolygonF TileSetGfxItem::AssembleAutoTilePolygon(AutoTileType eAutoTileType, TileSetShape eTileSetShape, TileSetAutoTilePart ePart)
{
	bool bHasCorners = false;
	bool bHasSides = false;
	switch(eAutoTileType)
	{
	case AUTOTILETYPE_MatchCornerSides:
		bHasCorners = bHasSides = true;
		break;
	case AUTOTILETYPE_MatchCorner:
		bHasCorners = true;
		break;
	case AUTOTILETYPE_MatchSides:
		bHasSides = true;
		break;
	default:
		HyGuiLog("TileSetGfxItem::AssembleAutoTilePolygon: Unhandled AutoTileType enum value!", LOGTYPE_Error);
		return QPolygonF();
	}

	QPolygonF mainShape = m_pShapeItem->polygon();
	// Half the size of `mainShape`, which is centered around (0,0)

	QPointF vHalfSize(mainShape.boundingRect().width() * 0.5f, mainShape.boundingRect().height() * 0.5f);
	QPolygonF partPolygon;
	switch(eTileSetShape)
	{
	case TILESETSHAPE_Square:
		switch(ePart)
		{
		case AUTOTILEPART_Center:
			partPolygon = QPolygonF() << QPointF(-vHalfSize.x() * 0.5f, -vHalfSize.y() * 0.5f)
				<< QPointF(vHalfSize.x() * 0.5f, -vHalfSize.y() * 0.5f)
				<< QPointF(vHalfSize.x() * 0.5f, vHalfSize.y() * 0.5f)
				<< QPointF(-vHalfSize.x() * 0.5f, vHalfSize.y() * 0.5f);
			break;
		case AUTOTILEPART_RightSide:
			if(!bHasSides)
				break;
			partPolygon = QPolygonF() << QPointF(0.0f, -vHalfSize.y())
				<< QPointF(vHalfSize.x(), -vHalfSize.y())
				<< QPointF(vHalfSize.x(), vHalfSize.y())
				<< QPointF(0.0f, vHalfSize.y());
			break;
		case AUTOTILEPART_BottomRightCorner:
			if(!bHasCorners)
				break;
			partPolygon = QPolygonF() << QPointF(0.0f, 0.0f)
				<< QPointF(vHalfSize.x(), 0.0f)
				<< QPointF(vHalfSize.x(), vHalfSize.y())
				<< QPointF(0.0f, vHalfSize.y());
			break;
		case AUTOTILEPART_BottomSide:
			if(!bHasSides)
				break;
			partPolygon = QPolygonF() << QPointF(-vHalfSize.x(), 0.0f)
				<< QPointF(vHalfSize.x(), 0.0f)
				<< QPointF(vHalfSize.x(), vHalfSize.y())
				<< QPointF(-vHalfSize.x(), vHalfSize.y());
			break;

		case AUTOTILEPART_BottomLeftCorner:
			if(!bHasCorners)
				break;
			partPolygon = QPolygonF() << QPointF(-vHalfSize.x(), 0.0f)
				<< QPointF(0.0f, 0.0f)
				<< QPointF(0.0f, vHalfSize.y())
				<< QPointF(-vHalfSize.x(), vHalfSize.y());
			break;
		case AUTOTILEPART_LeftSide:
			if(!bHasSides)
				break;
			partPolygon = QPolygonF() << QPointF(-vHalfSize.x(), -vHalfSize.y())
				<< QPointF(0.0f, -vHalfSize.y())
				<< QPointF(0.0f, vHalfSize.y())
				<< QPointF(-vHalfSize.x(), vHalfSize.y());
			break;
		case AUTOTILEPART_TopLeftCorner:
			if(!bHasCorners)
				break;
			partPolygon = QPolygonF() << QPointF(-vHalfSize.x(), -vHalfSize.y())
				<< QPointF(0.0f, -vHalfSize.y())
				<< QPointF(0.0f, 0.0f)
				<< QPointF(-vHalfSize.x(), 0.0f);
			break;
		case AUTOTILEPART_TopSide:
			if(!bHasSides)
				break;
			partPolygon = QPolygonF() << QPointF(-vHalfSize.x(), -vHalfSize.y())
				<< QPointF(vHalfSize.x(), -vHalfSize.y())
				<< QPointF(vHalfSize.x(), 0.0f)
				<< QPointF(-vHalfSize.x(), 0.0f);
			break;
		case AUTOTILEPART_TopRightCorner:
			if(!bHasCorners)
				break;
			partPolygon = QPolygonF() << QPointF(0.0f, -vHalfSize.y())
				<< QPointF(vHalfSize.x(), -vHalfSize.y())
				<< QPointF(vHalfSize.x(), 0.0f)
				<< QPointF(0.0f, 0.0f);
			break;
		default:
			return QPolygonF();
			break;
		}
		break;

	default:
		HyGuiLog("TileSetGfxItem::AssembleAutoTilePolygon: Unhandled TileSetShape enum value!", LOGTYPE_Error);
		break;
	}

	return partPolygon;
}

