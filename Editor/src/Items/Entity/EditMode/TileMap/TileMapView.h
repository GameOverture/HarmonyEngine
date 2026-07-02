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

class TileMapView : public IEditModeView
{
public:
	TileMapView(HyEntity2d *pParent = nullptr);
	virtual ~TileMapView();

	virtual void SyncWithModel(EditModeState eEditModeState) = 0;
};

#endif // TileMapView_H
