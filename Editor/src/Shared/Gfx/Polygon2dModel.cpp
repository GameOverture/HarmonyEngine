/**************************************************************************
*	Polygon2dModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "Polygon2dModel.h"

Polygon2dModel::Polygon2dModel()
{
}

/*virtual*/ Polygon2dModel::~Polygon2dModel()
{
}

QVector<CollisionPolygon> Polygon2dModel::GetPolygonList() const
{
	return m_PolygonList;
}
