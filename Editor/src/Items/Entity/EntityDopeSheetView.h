/**************************************************************************
*	EntityDopeSheetView.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef ENTITYDOPESHEETVIEW_H
#define ENTITYDOPESHEETVIEW_H

#include <QGraphicsView>

class EntityDopeSheetView : public QGraphicsView
{
	int								m_iCurrentFrame;

public:
	EntityDopeSheetView();
	virtual ~EntityDopeSheetView();
};

#endif // ENTITYDOPESHEETVIEW_H
