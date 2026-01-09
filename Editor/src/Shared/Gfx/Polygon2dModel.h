/**************************************************************************
*	Polygon2dModel.h
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2025 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef POLYGON2DMODEL_H
#define POLYGON2DMODEL_H

#include "Global.h"

class IPolygon2dView;

enum ShapeMouseMoveResult
{
	SHAPEMOUSEMOVE_Crosshair,
	SHAPEMOUSEMOVE_Outside,
	SHAPEMOUSEMOVE_Inside,
	SHAPEMOUSEMOVE_AddVertex,
	SHAPEMOUSEMOVE_HoverVertex,
	SHAPEMOUSEMOVE_HoverSelectedVertex
};

enum ShapeMousePressResult
{
	SHAPEMOUSEPRESS_Outside,
	SHAPEMOUSEPRESS_Inside,
	SHAPEMOUSEPRESS_VertexAdded,
	SHAPEMOUSEPRESS_VertexPressed
};

class Polygon2dModel
{
	HyColor								m_Color;
	EditorShape							m_eType;		// "Shape", "Type" - when serialized in property (string)
	IHyFixture2d *						m_pData;		// "Shape", "Data" - when serialized in property (QJsonArray of floats)
	
	QList<QPointF>						m_VertexList;
	QList<bool>							m_VertexSelectedList;

	// Track Views manually since we don't inherit from QObject
	QList<IPolygon2dView *>				m_ViewList;

public:
	Polygon2dModel(HyColor color, EditorShape eShape = SHAPE_None, const QList<float> &floatList = QList<float>());
	virtual ~Polygon2dModel();

	HyColor GetColor() const;
	EditorShape GetType() const;
	IHyFixture2d *GetData() const;
	const QList<QPointF> &GetVertexList() const;

	bool IsValidShape() const;

	void SetColor(HyColor color);
	void SetData(HyColor color, EditorShape eShape, const QList<float> &floatList);

	void TransformSelf(glm::mat4 mtxTransform); // NOTE: Does not update m_Outline, requires a DeserializeOutline()

	ShapeMouseMoveResult OnMouseMoveEvent(QPointF ptWorldMousePos);
	ShapeMousePressResult OnMousePressEvent(QMouseEvent *pEvent);
	int OnMouseMarqueeReleased(QPointF ptBotLeft, QPointF ptTopRight);
};

#endif // POLYGON2DMODEL_H
