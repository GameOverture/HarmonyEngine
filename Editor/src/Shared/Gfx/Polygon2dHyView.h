/**************************************************************************
 *	Polygon2dHyView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef POLYGON2DHYVIEW_H
#define POLYGON2DHYVIEW_H

#include "Global.h"
#include "Polygon2dModel.h"

class Polygon2dHyView : public HyEntity2d
{
	Polygon2dModel &		m_ModelRef;

	//GrabPoint

public:
	Polygon2dHyView(Polygon2dModel &modelRef, HyEntity2d *pParent = nullptr);
	virtual ~Polygon2dHyView();
};

#endif // POLYGON2DHYVIEW_H
