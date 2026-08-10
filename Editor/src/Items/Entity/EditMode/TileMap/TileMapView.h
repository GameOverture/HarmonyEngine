/**************************************************************************
 *	TileMapView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef TileMapView_H
#define TileMapView_H

#include "Global.h"
#include "IEditModeView.h"
#include "TileMapGrid.h"

#include "vendor/libtiled/map.h"

class TileData;

//class BrushPreview : public HyEntity2d
//{
//	QList<HyTexturedQuad2d *>	m_TileList;
//
//public:
//	BrushPreview(HyEntity2d *pParent = nullptr);
//	virtual ~BrushPreview();
//
//	void Clear();
//	void Sync(const QMap<QPoint, TileData *> &brushMap, glm::vec2 vGridSize, HyTileMapLayout eLayout);
//};

class TileMapView : public IEditModeView
{
	TileMapGrid			m_MouseHoverGrid;

	HyTileMapLayer		m_TileMapLayer;

public:
	TileMapView(HyEntity2d *pParent = nullptr);
	virtual ~TileMapView();

	HyTileMapLayer &GetTileMapLayer();

	void SyncMouseHoverGrid();
	virtual void SyncWithModel(EditModeState eEditModeState);
};

#endif // TileMapView_H
