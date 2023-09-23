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

class EntityStateData;
class EntityDopeSheetScene;

class EntityDopeSheetView : public QGraphicsView
{
	EntityStateData *			m_pStateData;

public:
	EntityDopeSheetView(QWidget *pParent = nullptr);
	virtual ~EntityDopeSheetView();

	EntityDopeSheetScene *GetScene() const;
	void SetScene(EntityStateData *pStateData);

protected:
	virtual void drawForeground(QPainter *pPainter, const QRectF &rect) override;
};

#endif // ENTITYDOPESHEETVIEW_H
