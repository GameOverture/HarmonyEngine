/**************************************************************************
 *	Polygon2dHyView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "Polygon2dHyView.h"

Polygon2dHyView::Polygon2dHyView(Polygon2dModel &modelRef, HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_ModelRef(modelRef)
{
}

/*virtual*/ Polygon2dHyView::~Polygon2dHyView()
{
}
