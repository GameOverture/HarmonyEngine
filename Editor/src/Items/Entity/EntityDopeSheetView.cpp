/**************************************************************************
*	EntityDopeSheetView.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "EntityDopeSheetView.h"

EntityDopeSheetView::EntityDopeSheetView() :
	QGraphicsView(),
	m_iCurrentFrame(0)
{
}

/*virtual*/ EntityDopeSheetView::~EntityDopeSheetView()
{
}
