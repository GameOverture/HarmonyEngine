/**************************************************************************
 *	TileMapGfxScene.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TILEMAPGFXSCENE_H
#define TILEMAPGFXSCENE_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>

class AuxTileMap;
class AtlasTileSet;
class TileData;
class TileGfxItem;

class TileMapGfxScene : public QGraphicsScene
{
	Q_OBJECT

	AuxTileMap *						m_pAuxTileMap;
	const AtlasTileSet *				m_pTileSet;
	QUuid								m_TerrainUuid;

	QGraphicsRectItem					m_BorderRect;
	QMap<TileData *, TileGfxItem *>		m_TileGfxItemsMap;

public:
	TileMapGfxScene(QObject *pParent = nullptr);
	virtual ~TileMapGfxScene();

	void Initialize(const AtlasTileSet *pTileSet, QUuid terrainUuid);

	void OnMarqueeRelease(Qt::MouseButton eMouseBtn, bool bShiftHeld, QPointF ptStartDrag, QPointF ptEndDrag);
};

#endif // TILEMAPGFXSCENE_H
