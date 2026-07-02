/**************************************************************************
 *	TileMapView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "TileMapView.h"

TileMapView::TileMapView(HyEntity2d *pParent /*= nullptr*/) :
	IEditModeView(pParent)
{
}

/*virtual*/ TileMapView::~TileMapView()
{
}
