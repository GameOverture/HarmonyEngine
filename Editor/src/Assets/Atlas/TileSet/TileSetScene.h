/**************************************************************************
 *	TileSetScene.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2024 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILESETSCENE_H
#define TILESETSCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>

class AtlasTileSet;
class TileGfxItem;

enum TileSetMode
{
	TILESETMODE_Importing,
	TILESETMODE_TileSet
};

class TileSetGroupItem : public QGraphicsItemGroup {
public:
	TileSetGroupItem(QGraphicsItem *parent = nullptr)
		: QGraphicsItemGroup(parent) {
		setHandlesChildEvents(false);
		setAcceptedMouseButtons(Qt::AllButtons);
	}
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *pEvent) override {
		pEvent->ignore(); // Pass the event to child items
	}
};

class TileSetScene : public QGraphicsScene
{
	Q_OBJECT

	AtlasTileSet *													m_pTileSet;

	TileSetMode														m_eMode;
	TileSetGroupItem *												m_pModeImportGroup;
	TileSetGroupItem *												m_pModeTileSetGroup;

	QGraphicsRectItem												m_BoundsRect;		// A dash-line box that encompasses the entire import scene
	struct ImportTileItem
	{
		bool														m_bSelected;
		QGraphicsRectItem *											m_pRectItem;
		QGraphicsPixmapItem *										m_pPixmapItem;
		QGraphicsPolygonItem *										m_pShapeItem;		// This is the shape outline of the tile as it sits in a grid, especially helpful for isometric and hexagon

		ImportTileItem(QGraphicsRectItem *pRect, QGraphicsPixmapItem *pPixmap, QGraphicsPolygonItem *pShape) :
			m_bSelected(true),
			m_pRectItem(pRect),
			m_pPixmapItem(pPixmap),
			m_pShapeItem(pShape)
		{ }

		void SetSelected(bool bSelected)
		{
			m_bSelected = bSelected;

			HyColor selectedColor = m_bSelected ? HyColor::Orange : HyColor::Black;
			m_pRectItem->setPen(QPen(QBrush(HyGlobal::ConvertHyColor(selectedColor)), 1.0f, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
			m_pShapeItem->setVisible(m_bSelected);
		}
	};
	QMap<QPoint, ImportTileItem>									m_ImportTileMap;
	QSize															m_vImportRegionSize;

	QVector<TileGfxItem *>											m_TileSetPixmapItem;// The tile set pixmap item that is displayed in the tiles scene

public:
	TileSetScene();
	~TileSetScene();

	void Initialize(AtlasTileSet *pTileSet);

	void SetDisplayMode(TileSetMode eMode);

	int GetNumImportPixmaps() const;
	QSize GetImportRegionSize() const;
	QMap<QPoint, QPixmap> AssembleImportMap();

	void ClearImport();
	void AddImport(const QPolygonF &outlinePolygon, QPoint ptGridPos, QPixmap pixmap, bool bDefaultSelected);
	void SyncImport();

	void SyncTileSet(); // Slow, deletes/reallocates all graphics items

	void OnMarqueeRelease(Qt::MouseButton eMouseBtn, QPointF ptStartDrag, QPointF ptEndDrag);
};

#endif // TILESETSCENE_H
