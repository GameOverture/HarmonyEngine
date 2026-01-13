/**************************************************************************
 *	Polygon2dQtView.h
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef POLYGON2DQTVIEW_H
#define POLYGON2DQTVIEW_H

#include "Global.h"
#include "IPolygon2dView.h"

#include <QGraphicsItem>

class QGraphicsPolygonItem;
class QGraphicsRectItem;

class Polygon2dQtView : public IPolygon2dView, public QGraphicsItem
{
	QGraphicsPolygonItem *				m_pGfxPolygonItem;
	QList<QGraphicsRectItem *>			m_GrabPointList;

public:
	Polygon2dQtView(QGraphicsItem *pParent = nullptr);
	virtual ~Polygon2dQtView();

	virtual void RefreshColor() override;
	virtual void RefreshView() override;

	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;
};

#endif // POLYGON2DQTVIEW_H
