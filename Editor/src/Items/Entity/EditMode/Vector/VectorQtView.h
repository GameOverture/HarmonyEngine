/**************************************************************************
 *	VectorQtView.h
 *
 *	Harmony Engine - HyEditor
 *	Copyright (c) 2025 Jason Knobler
 *
 *	GPL/LGPL License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/main/Editor/LICENSE
 *************************************************************************/
#ifndef VectorQtView_H
#define VectorQtView_H

#include "Global.h"
#include "VectorView.h"

#include <QGraphicsItem>

class QGraphicsPolygonItem;
class QGraphicsRectItem;

class VectorQtView : public VectorView, public QGraphicsItem
{
	QGraphicsPolygonItem *				m_pGfxPolygonItem;
	QList<QGraphicsRectItem *>			m_GrabPointList;

public:
	VectorQtView(QGraphicsItem *pParent = nullptr);
	virtual ~VectorQtView();

	virtual void SyncWithModel(EditModeState eEditModeState) override;

	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) override;
};

#endif // VectorQtView_H
