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

QGraphicsPolygonItem *TileSetGfxItem::GetAutoTilePartAt(QPointF ptLocalPos, TileSetAutoTilePart &ePartOut)
{
	for(int i = 0; i < NUM_AUTOTILEPARTS; ++i)
	{
		if(m_pTerrainParts[i]->contains(ptLocalPos))
		{
			ePartOut = static_cast<TileSetAutoTilePart>(i);
			return m_pTerrainParts[i];
		}
	}

	ePartOut = AUTOTILEPART_Unknown;
	return nullptr;
}

QPolygonF TileSetGfxItem::AssembleAutoTilePolygon(AutoTileType eAutoTileType, TileSetShape eTileSetShape, TileSetAutoTilePart ePart)
{
	// `mainShape` has a clockwise winding order, with the first vertex at the center top (or top-left corner if shape is square or flat-top hexagon)
	QPolygonF mainShape = m_pShapeItem->polygon();
	
	QPolygonF halfShape = mainShape;
	for(QPointF &ptVert : halfShape)
		ptVert *= 0.333f;

	if(ePart == AUTOTILEPART_Center)
		return halfShape;

	// Midway point (50%)
	std::function<QPointF(const QPointF &, const QPointF &)> fpMidway =
		[](const QPointF &a, const QPointF &b) -> QPointF {
			return QPointF((a.x() + b.x()) * 0.5, (a.y() + b.y()) * 0.5);
		};
	// 1/3rd of the way from a to b
	std::function<QPointF(const QPointF &, const QPointF &)> fpOneThird =
		[](const QPointF &a, const QPointF &b) -> QPointF {
			return QPointF(a.x() + (b.x() - a.x()) / 3.0, a.y() + (b.y() - a.y()) / 3.0);
		};
	// 2/3rd of the way from a to b
	std::function<QPointF(const QPointF &, const QPointF &)> fpTwoThird =
		[](const QPointF &a, const QPointF &b) -> QPointF {
			return QPointF(a.x() + 2.0 * (b.x() - a.x()) / 3.0, a.y() + 2.0 * (b.y() - a.y()) / 3.0);
		};

	// The `halfShape` sits inside the center of the full polygon `mainShape`.
	// Make a polygon `partPolygon`, that is the area between `halfShape` and `mainShape` based on which part is being requested.
	// Assemble using the vertices of both polygons and the above lambda functions to create the new polygon.
	// `partPolygon` will not overlap with `halfShape` and also fits exactly within `mainShape`.
	QPolygonF partPolygon;
	switch(ePart)
	{
	case AUTOTILEPART_RightCorner:		// Hexagon-flat, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpMidway(mainShape[1], mainShape[2])
							<< fpMidway(halfShape[1], halfShape[2])
							<< halfShape[1]
							<< fpMidway(halfShape[0], halfShape[1]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpOneThird(mainShape[1], mainShape[2])
							<< halfShape[1];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpMidway(mainShape[2], mainShape[3])
							<< fpMidway(halfShape[2], halfShape[3])
							<< halfShape[2]
							<< fpMidway(halfShape[1], halfShape[2]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpOneThird(mainShape[2], mainShape[3])
							<< fpOneThird(halfShape[2], halfShape[3])
							<< halfShape[2]
							<< fpTwoThird(halfShape[1], halfShape[2]);
			}
		}
		break;

	case AUTOTILEPART_RightSide:		// Hexagon-pointed, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << halfShape[1]
							<< mainShape[1]
							<< mainShape[2]
							<< halfShape[2];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << halfShape[1]
							<< fpOneThird(mainShape[1], mainShape[2])
							<< fpTwoThird(mainShape[1], mainShape[2])
							<< halfShape[2];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << halfShape[1]
							<< mainShape[1]
							<< mainShape[2]
							<< halfShape[2];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(halfShape[1], halfShape[2])
							<< fpOneThird(mainShape[1], mainShape[2])
							<< fpTwoThird(mainShape[1], mainShape[2])
							<< fpTwoThird(halfShape[1], halfShape[2]);
			}
		}
		break;

	case AUTOTILEPART_BottomRightSide:		// Hexagon-flat, Hexagon-pointed, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << halfShape[1]
							<< mainShape[1]
							<< mainShape[2]
							<< halfShape[2];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << halfShape[1]
							<< fpOneThird(mainShape[1], mainShape[2])
							<< fpTwoThird(mainShape[1], mainShape[2])
							<< halfShape[2];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << halfShape[2]
							<< mainShape[2]
							<< mainShape[3]
							<< halfShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(halfShape[2], halfShape[3])
							<< fpOneThird(mainShape[2], mainShape[3])
							<< fpTwoThird(mainShape[2], mainShape[3])
							<< fpTwoThird(halfShape[2], halfShape[3]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << halfShape[2]
							<< mainShape[2]
							<< mainShape[3]
							<< halfShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(halfShape[2], halfShape[3])
							<< fpOneThird(mainShape[2], mainShape[3])
							<< fpTwoThird(mainShape[2], mainShape[3])
							<< fpTwoThird(halfShape[2], halfShape[3]);
			}
		}
		break;

	case AUTOTILEPART_BottomRightCorner:	// Hexagon-flat, Hexagon-pointed, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(halfShape[1], halfShape[2])
							<< fpMidway(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpMidway(mainShape[2], mainShape[3])
							<< fpMidway(halfShape[2], halfShape[3])
							<< halfShape[2];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << halfShape[2]
							<< fpTwoThird(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpOneThird(mainShape[2], mainShape[3]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(halfShape[2], halfShape[3])
							<< fpMidway(mainShape[2], mainShape[3])
							<< mainShape[3]
							<< fpMidway(mainShape[3], mainShape[4])
							<< fpMidway(halfShape[3], halfShape[4])
							<< halfShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(halfShape[2], halfShape[3])
							<< fpTwoThird(mainShape[2], mainShape[3])
							<< mainShape[3]
							<< fpOneThird(mainShape[3], mainShape[4])
							<< fpOneThird(halfShape[3], halfShape[4])
							<< halfShape[3];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(halfShape[1], halfShape[2])
							<< fpMidway(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpMidway(mainShape[2], mainShape[3])
							<< fpMidway(halfShape[2], halfShape[3])
							<< halfShape[2];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(halfShape[1], halfShape[2])
							<< fpTwoThird(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpOneThird(mainShape[2], mainShape[3])
							<< fpOneThird(halfShape[2], halfShape[3])
							<< halfShape[2];
			}
		}
		break;

	case AUTOTILEPART_BottomSide:			// Hexagon-flat, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << halfShape[3]
							<< halfShape[2]
							<< mainShape[2]
							<< mainShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << halfShape[3]
							<< halfShape[2]
							<< fpOneThird(mainShape[2], mainShape[3])
							<< fpTwoThird(mainShape[2], mainShape[3]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << halfShape[4]
							<< halfShape[3]
							<< mainShape[3]
							<< mainShape[4];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(halfShape[3], halfShape[4])
							<< fpOneThird(halfShape[3], halfShape[4])
							<< fpOneThird(mainShape[3], mainShape[4])
							<< fpTwoThird(mainShape[3], mainShape[4]);
			}
		}
		break;

	case AUTOTILEPART_BottomCorner:			// Hexagon-pointed, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(halfShape[2], halfShape[3])
							<< halfShape[2]
							<< fpMidway(halfShape[1], halfShape[2])
							<< fpMidway(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpMidway(mainShape[2], mainShape[3]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << halfShape[2]
							<< fpTwoThird(mainShape[1], mainShape[2])
							<< mainShape[2]
							<< fpOneThird(mainShape[2], mainShape[3]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(halfShape[3], halfShape[4])
							<< halfShape[3]
							<< fpMidway(halfShape[2], halfShape[3])
							<< fpMidway(mainShape[2], mainShape[3])
							<< mainShape[3]
							<< fpMidway(mainShape[3], mainShape[4]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(halfShape[3], halfShape[4])
							<< halfShape[3]
							<< fpTwoThird(halfShape[2], halfShape[3])
							<< fpTwoThird(mainShape[2], mainShape[3])
							<< mainShape[3]
							<< fpOneThird(mainShape[3], mainShape[4]);
			}
		}
		break;

	case AUTOTILEPART_BottomLeftSide:		// Hexagon-flat, Hexagon-pointed, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[3]
							<< halfShape[3]
							<< halfShape[2]
							<< mainShape[2];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << halfShape[3]
							<< halfShape[2]
							<< fpOneThird(mainShape[2], mainShape[3])
							<< fpTwoThird(mainShape[2], mainShape[3]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[5]
							<< halfShape[5]
							<< halfShape[4]
							<< mainShape[4];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[4], mainShape[5])
							<< fpTwoThird(halfShape[4], halfShape[5])
							<< fpOneThird(halfShape[4], halfShape[5])
							<< fpOneThird(mainShape[4], mainShape[5]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[4]
							<< halfShape[4]
							<< halfShape[3]
							<< mainShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(halfShape[3], halfShape[4])
							<< fpOneThird(halfShape[3], halfShape[4])
							<< fpOneThird(mainShape[3], mainShape[4])
							<< fpTwoThird(mainShape[3], mainShape[4]);
			}
		}
		break;

	case AUTOTILEPART_BottomLeftCorner:		// Hexagon-flat, Hexagon-pointed, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[3], mainShape[0])
							<< fpMidway(halfShape[3], halfShape[0])
							<< halfShape[3]
							<< fpMidway(halfShape[2], halfShape[3])
							<< fpMidway(mainShape[2], mainShape[3])
							<< mainShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[3], mainShape[0])
							<< halfShape[3]
							<< fpTwoThird(mainShape[2], mainShape[3])
							<< mainShape[3];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(halfShape[4], halfShape[5])
							<< halfShape[4]
							<< fpMidway(halfShape[3], halfShape[4])
							<< fpMidway(mainShape[3], mainShape[4])
							<< mainShape[4]
							<< fpMidway(mainShape[4], mainShape[5]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(halfShape[4], halfShape[5])
							<< halfShape[4]
							<< fpTwoThird(halfShape[3], halfShape[4])
							<< fpTwoThird(mainShape[3], mainShape[4])
							<< mainShape[4]
							<< fpOneThird(mainShape[4], mainShape[5]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[4], mainShape[5])
							<< fpMidway(halfShape[4], halfShape[5])
							<< halfShape[4]
							<< fpMidway(halfShape[3], halfShape[4])
							<< fpMidway(mainShape[3], mainShape[4])
							<< mainShape[4];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[4], mainShape[5])
							<< fpOneThird(halfShape[4], halfShape[5])
							<< halfShape[4]
							<< fpTwoThird(halfShape[3], halfShape[4])
							<< fpTwoThird(mainShape[3], mainShape[4])
							<< mainShape[4];
			}
		}
		break;

	case AUTOTILEPART_LeftCorner:			// Hexagon-flat, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[3], mainShape[0])
							<< fpMidway(halfShape[3], halfShape[0])
							<< halfShape[3]
							<< fpMidway(halfShape[2], halfShape[3])
							<< fpMidway(mainShape[2], mainShape[3])
							<< mainShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[3], mainShape[0])
							<< halfShape[3]
							<< fpTwoThird(mainShape[2], mainShape[3])
							<< mainShape[3];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[5], mainShape[0])
							<< fpMidway(halfShape[5], halfShape[0])
							<< halfShape[5]
							<< fpMidway(halfShape[4], halfShape[5])
							<< fpMidway(mainShape[4], mainShape[5])
							<< mainShape[5];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[5], mainShape[0])
							<< fpOneThird(halfShape[5], halfShape[0])
							<< halfShape[5]
							<< fpTwoThird(halfShape[4], halfShape[5])
							<< fpTwoThird(mainShape[4], mainShape[5])
							<< mainShape[5];
			}
		}
		break;

	case AUTOTILEPART_LeftSide:				// Hexagon-pointed, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< halfShape[0]
							<< halfShape[3]
							<< mainShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[3], mainShape[0])
							<< halfShape[0]
							<< halfShape[3]
							<< fpOneThird(mainShape[3], mainShape[0]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[5]
							<< halfShape[5]
							<< halfShape[4]
							<< mainShape[4];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[4], mainShape[5])
							<< fpTwoThird(halfShape[4], halfShape[5])
							<< fpOneThird(halfShape[4], halfShape[5])
							<< fpOneThird(mainShape[4], mainShape[5]);
			}
		}
		break;

	case AUTOTILEPART_TopLeftSide:			// Hexagon-flat, Hexagon-pointed, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< halfShape[0]
							<< halfShape[3]
							<< mainShape[3];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[3], mainShape[0])
							<< halfShape[0]
							<< halfShape[3]
							<< fpOneThird(mainShape[3], mainShape[0]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< halfShape[0]
							<< halfShape[5]
							<< mainShape[5];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[5], mainShape[0])
							<< fpTwoThird(halfShape[5], halfShape[0])
							<< fpOneThird(halfShape[5], halfShape[0])
							<< fpOneThird(mainShape[5], mainShape[0]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< halfShape[0]
							<< halfShape[5]
							<< mainShape[5];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[5], mainShape[0])
							<< fpTwoThird(halfShape[5], halfShape[0])
							<< fpOneThird(halfShape[5], halfShape[0])
							<< fpOneThird(mainShape[5], mainShape[0]);
			}
		}
		break;

	case AUTOTILEPART_TopLeftCorner:		// Hexagon-flat, Hexagon-pointed, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << mainShape[0]
							<< fpMidway(mainShape[0], mainShape[1])
							<< fpMidway(halfShape[0], halfShape[1])
							<< halfShape[0]
							<< fpMidway(halfShape[3], halfShape[0])
							<< fpMidway(mainShape[3], mainShape[0]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << mainShape[0]
							<< fpOneThird(mainShape[0], mainShape[1])
							<< halfShape[0]
							<< fpTwoThird(mainShape[3], mainShape[0]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << mainShape[0]
							<< fpMidway(mainShape[0], mainShape[1])
							<< fpMidway(halfShape[0], halfShape[1])
							<< halfShape[0]
							<< fpMidway(halfShape[5], halfShape[0])
							<< fpMidway(mainShape[5], mainShape[0]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << mainShape[0]
							<< fpOneThird(mainShape[0], mainShape[1])
							<< fpOneThird(halfShape[0], halfShape[1])
							<< halfShape[0]
							<< fpTwoThird(halfShape[5], halfShape[0])
							<< fpTwoThird(mainShape[5], mainShape[0]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << mainShape[5]
							<< fpMidway(mainShape[5], mainShape[0])
							<< fpMidway(halfShape[5], halfShape[0])
							<< halfShape[5]
							<< fpMidway(halfShape[4], halfShape[5])
							<< fpMidway(mainShape[4], mainShape[5]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << mainShape[5]
							<< fpOneThird(mainShape[5], mainShape[0])
							<< fpOneThird(halfShape[5], halfShape[0])
							<< halfShape[5]
							<< fpTwoThird(halfShape[4], halfShape[5])
							<< fpTwoThird(mainShape[4], mainShape[5]);
			}
		}
		break;

	case AUTOTILEPART_TopCorner:			// Hexagon-pointed, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << mainShape[0]
							<< fpMidway(mainShape[0], mainShape[1])
							<< fpMidway(halfShape[0], halfShape[1])
							<< halfShape[0]
							<< fpMidway(halfShape[3], halfShape[0])
							<< fpMidway(mainShape[3], mainShape[0]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << mainShape[0]
							<< fpOneThird(mainShape[0], mainShape[1])
							<< halfShape[0]
							<< fpTwoThird(mainShape[3], mainShape[0]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << mainShape[0]
							<< fpMidway(mainShape[0], mainShape[1])
							<< fpMidway(halfShape[0], halfShape[1])
							<< halfShape[0]
							<< fpMidway(halfShape[5], halfShape[0])
							<< fpMidway(mainShape[5], mainShape[0]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << mainShape[0]
							<< fpOneThird(mainShape[0], mainShape[1])
							<< fpOneThird(halfShape[0], halfShape[1])
							<< halfShape[0]
							<< fpTwoThird(halfShape[5], halfShape[0])
							<< fpTwoThird(mainShape[5], mainShape[0]);
			}
		}
		break;

	case AUTOTILEPART_TopSide:				// Hexagon-flat, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< mainShape[1]
							<< halfShape[1]
							<< halfShape[0];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[0], mainShape[1])
							<< fpTwoThird(mainShape[0], mainShape[1])
							<< halfShape[1]
							<< halfShape[0];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< mainShape[1]
							<< halfShape[1]
							<< halfShape[0];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[0], mainShape[1])
							<< fpTwoThird(mainShape[0], mainShape[1])
							<< fpTwoThird(halfShape[0], halfShape[1])
							<< fpOneThird(halfShape[0], halfShape[1]);
			}
		}
		break;

	case AUTOTILEPART_TopRightSide:			// Hexagon-flat, Hexagon-pointed, Isometric
		if(eTileSetShape == TILESETSHAPE_Isometric)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< mainShape[1]
							<< halfShape[1]
							<< halfShape[0];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[0], mainShape[1])
							<< fpTwoThird(mainShape[0], mainShape[1])
							<< halfShape[1]
							<< halfShape[0];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[1]
							<< mainShape[2]
							<< halfShape[2]
							<< halfShape[1];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[1], mainShape[2])
							<< fpTwoThird(mainShape[1], mainShape[2])
							<< fpTwoThird(halfShape[1], halfShape[2])
							<< fpOneThird(halfShape[1], halfShape[2]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchSides)
			{
				partPolygon << mainShape[0]
							<< mainShape[1]
							<< halfShape[1]
							<< halfShape[0];
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpOneThird(mainShape[0], mainShape[1])
							<< fpTwoThird(mainShape[0], mainShape[1])
							<< fpTwoThird(halfShape[0], halfShape[1])
							<< fpOneThird(halfShape[0], halfShape[1]);
			}
		}
		break;

	case AUTOTILEPART_TopRightCorner:		// Hexagon-flat, Hexagon-pointed, Square, Half-offset-square
		if(eTileSetShape == TILESETSHAPE_Square || eTileSetShape == TILESETSHAPE_HalfOffsetSquare)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpMidway(mainShape[1], mainShape[2])
							<< fpMidway(halfShape[1], halfShape[2])
							<< halfShape[1]
							<< fpMidway(halfShape[0], halfShape[1]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpOneThird(mainShape[1], mainShape[2])
							<< halfShape[1];
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonFlatTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpMidway(mainShape[1], mainShape[2])
							<< fpMidway(halfShape[1], halfShape[2])
							<< halfShape[1]
							<< fpMidway(halfShape[0], halfShape[1]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpOneThird(mainShape[1], mainShape[2])
							<< fpOneThird(halfShape[1], halfShape[2])
							<< halfShape[1]
							<< fpTwoThird(halfShape[0], halfShape[1]);
			}
		}
		else if(eTileSetShape == TILESETSHAPE_HexagonPointTop)
		{
			if(eAutoTileType == AUTOTILETYPE_MatchCorner)
			{
				partPolygon << fpMidway(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpMidway(mainShape[1], mainShape[2])
							<< fpMidway(halfShape[1], halfShape[2])
							<< halfShape[1]
							<< fpMidway(halfShape[0], halfShape[1]);
			}
			else if(eAutoTileType == AUTOTILETYPE_MatchCornerSides)
			{
				partPolygon << fpTwoThird(mainShape[0], mainShape[1])
							<< mainShape[1]
							<< fpOneThird(mainShape[1], mainShape[2])
							<< fpOneThird(halfShape[1], halfShape[2])
							<< halfShape[1]
							<< fpTwoThird(halfShape[0], halfShape[1]);
			}
		}
		break;
	}

	return partPolygon;
}

