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
#include "IPolygon2dView.h"

Polygon2dModel::Polygon2dModel(HyColor color, EditorShape eShape /*= SHAPE_None*/, const QList<float> &floatList /*= QList<float>()*/) :
	m_eType(SHAPE_None),
	m_pData(nullptr)
{
	SetData(color, eShape, floatList);
}

/*virtual*/ Polygon2dModel::~Polygon2dModel()
{
}

HyColor Polygon2dModel::GetColor() const
{
	return m_Color;
}

EditorShape Polygon2dModel::GetType() const
{
	return m_eType;
}

IHyFixture2d *Polygon2dModel::GetData() const
{
	return m_pData;
}

const QList<QPointF> &Polygon2dModel::GetVertexList() const
{
	return m_VertexList;
}

bool Polygon2dModel::IsValidShape() const
{
	if(m_eType == SHAPE_LineChain)
	{
		HyChain2d *pChain2d = static_cast<HyChain2d *>(m_pData);
		if(pChain2d->GetData().iCount < 4)
			return false;

		return true;
	}
	
	HyShape2d *pShape2d = static_cast<HyShape2d *>(m_pData);
	switch(m_eType)
	{
	case SHAPE_None:
		return true;
	case SHAPE_Box:
	case SHAPE_Circle:
	case SHAPE_LineSegment:
	case SHAPE_Polygon:
	case SHAPE_Capsule:
		return pShape2d->IsValid();
	default:
		HyGuiLog("Polygon2dModel::IsValidShape: Unknown shape type encountered", LOGTYPE_Error);
		break;
	}

	return false;
}

void Polygon2dModel::SetColor(HyColor color)
{
	m_Color = color;

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshColor();
}

void Polygon2dModel::SetData(HyColor color, EditorShape eShape, const QList<float> &floatList)
{
	m_Color = color;

	if(m_eType != eShape || m_pData == nullptr)
	{
		m_eType = eShape;

		delete m_pData;
		if(m_eType == SHAPE_LineChain)
			m_pData = new HyChain2d();
		else
			m_pData = new HyShape2d();
	}

	std::vector<float> floatVec(floatList.begin(), floatList.end());
	m_pData->DeserializeSelf(HyGlobal::ConvertShapeToFixtureType(m_eType), floatVec);

	m_VertexList;
	m_VertexSelectedList;

	for(IPolygon2dView *pView : m_ViewList)
		pView->RefreshView();
}

void Polygon2dModel::TransformSelf(glm::mat4 mtxTransform)
{
	if(m_pData == nullptr)
		return;

	m_pData->TransformSelf(mtxTransform);
}

ShapeMouseMoveResult Polygon2dModel::OnMouseMoveEvent(QPointF ptWorldMousePos)
{
	if(IsValidShape() == false && m_eType == SHAPE_LineChain)
	{
		return SHAPEMOUSEMOVE_Crosshair;
	}

	ptWorldMousePos;

	//SHAPEMOUSEMOVE_Crosshair,
	return SHAPEMOUSEMOVE_Outside;
	//SHAPEMOUSEMOVE_Inside,
	//SHAPEMOUSEMOVE_AddVertex,
	//SHAPEMOUSEMOVE_HoverVertex,
	//SHAPEMOUSEMOVE_HoverSelectedVertex
}

ShapeMousePressResult Polygon2dModel::OnMousePressEvent(QMouseEvent *pEvent)
{
	return SHAPEMOUSEPRESS_Outside;
}

int Polygon2dModel::OnMouseMarqueeReleased(QPointF ptBotLeft, QPointF ptTopRight)
{
	return 0;
}
