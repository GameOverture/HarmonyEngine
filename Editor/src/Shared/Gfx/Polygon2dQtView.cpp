/**************************************************************************
 *	Polygon2dQtView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "Polygon2dQtView.h"
#include "Polygon2dModel.h"

#include <QGraphicsPolygonItem>
#include <QGraphicsRectItem>
#include <QPen>

Polygon2dQtView::Polygon2dQtView(QGraphicsItem *pParent /*= nullptr*/) :
	QGraphicsItem(pParent),
	m_pGfxPolygonItem(nullptr)
{
	m_pGfxPolygonItem = new QGraphicsPolygonItem(this);
}

/*virtual*/ Polygon2dQtView::~Polygon2dQtView()
{
	for(int i = 0; i < m_GrabPointList.size(); ++i)
		delete m_GrabPointList[i];
	delete m_pGfxPolygonItem;
}

/*virtual*/ void Polygon2dQtView::RefreshColor() /*override*/
{
	if(m_pModel == nullptr)
		return;

	m_pGfxPolygonItem->setBrush(QBrush(HyGlobal::ConvertHyColor(m_pModel->GetColor())));
	m_pGfxPolygonItem->setPen(QPen(HyGlobal::ConvertHyColor(m_pModel->GetColor().IsDark() ? HyColor::White : HyColor::Black), 1.0f));
}

/*virtual*/ void Polygon2dQtView::RefreshView(bool bTransformPreview) /*override*/
{
	if(m_pModel == nullptr)
	{
		m_pGfxPolygonItem->hide();
		return;
	}

	m_pGfxPolygonItem->setBrush(QBrush(HyGlobal::ConvertHyColor(m_pModel->GetColor())));
	m_pGfxPolygonItem->setPen(QPen(HyGlobal::ConvertHyColor(m_pModel->GetColor().IsDark() ? HyColor::White : HyColor::Black), 1.0f));

	QPolygonF polygon;
	const QList<GrabPointModel> &grabPointListRef = m_pModel->GetGrabPointList();
}

/*virtual*/ void Polygon2dQtView::OnHoverClear() /*override*/
{
}

/*virtual*/ QRectF Polygon2dQtView::boundingRect() const /*override*/
{
	return m_pGfxPolygonItem->boundingRect().adjusted(-5, -5, 5, 5); // Adjusted for grab points
}

/*virtual*/ void Polygon2dQtView::paint(QPainter* pPainter, const QStyleOptionGraphicsItem* pOption, QWidget* pWidget) /*override*/
{
	// Intentionally left blank since we are using child QGraphicsItems for rendering
}
