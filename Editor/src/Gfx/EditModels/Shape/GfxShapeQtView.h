/**************************************************************************
 *	GfxShapeQtView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef GfxShapeQtView_H
#define GfxShapeQtView_H

#include "Global.h"
#include "EditModeView.h"

#include <QGraphicsItem>

class QGraphicsPolygonItem;
class QGraphicsRectItem;

class GfxShapeQtView : public EditModeView, public QGraphicsItem
{
	QGraphicsPolygonItem *				m_pGfxPolygonItem;
	QList<QGraphicsRectItem *>			m_GrabPointList;

public:
	GfxShapeQtView(QGraphicsItem *pParent = nullptr);
	virtual ~GfxShapeQtView();

	virtual void SyncWithModel(EditModeState eEditModeState, EditModeAction eEditModeAction) override;

	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;
};

#endif // GfxShapeQtView_H
