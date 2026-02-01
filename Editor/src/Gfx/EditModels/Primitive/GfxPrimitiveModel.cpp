/**************************************************************************
*	GfxPrimitiveModel.cpp
*
*	Harmony Engine - Editor Tool
*	Copyright (c) 2026 Jason Knobler
*
*	Harmony Editor Tool License:
*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Global.h"
#include "GfxPrimitiveModel.h"
#include "IGfxEditView.h"

GfxPrimitiveModel::GfxPrimitiveModel() :
	IGfxEditModel(EDITMODEL_Shape, HyColor::White),
	m_bIsShape(true),
	m_ShapeModel(HyColor::White),
	m_ChainModel(HyColor::White)
{
}

/*virtual*/ GfxPrimitiveModel::~GfxPrimitiveModel()
{
}

/*virtual*/ bool GfxPrimitiveModel::IsValidModel() const /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.IsValidModel();
	else
		return m_ChainModel.IsValidModel();
}

bool GfxPrimitiveModel::IsShapeModel() const
{
	return m_bIsShape;
}

QString GfxPrimitiveModel::GetPrimType() const
{
	if(m_bIsShape)
		return HyGlobal::ShapeName(m_ShapeModel.GetShapeType());
	else
		return "Line Chain";
}

void GfxPrimitiveModel::SetPrimType(QString sNewType)
{
	if(sNewType == "Line Chain")
	{
		m_bIsShape = false;
		m_ChainModel.SetData(m_ShapeModel.ConvertedPolygonOrLineChainData());
	}
	else
	{
		m_bIsShape = true;
		EditorShape eNewShape = HyGlobal::GetShapeFromString(sNewType);
		m_ShapeModel.SetShapeType(eNewShape);
	}

	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshView(SHAPEMOUSEMOVE_None, false);
}

/*virtual*/ QList<float> GfxPrimitiveModel::GetData() const /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.GetData();
	else
		return m_ChainModel.GetData();
}

/*virtual*/ void GfxPrimitiveModel::SetData(const QList<float> &floatList) /*override*/
{
	if(m_bIsShape)
		m_ShapeModel.SetData(floatList);
	else
		m_ChainModel.SetData(floatList);

	for(IGfxEditView *pView : m_ViewList)
		pView->RefreshView(SHAPEMOUSEMOVE_None, false);
}

/*virtual*/ QString GfxPrimitiveModel::MouseTransformReleased(QString sShapeCodeName, QPointF ptWorldMousePos) /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.MouseTransformReleased(sShapeCodeName, ptWorldMousePos);
	else
		return m_ChainModel.MouseTransformReleased(sShapeCodeName, ptWorldMousePos);
}

/*virtual*/ ShapeMouseMoveResult GfxPrimitiveModel::DoMouseMoveIdle(glm::vec2 ptWorldMousePos) /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.DoMouseMoveIdle(ptWorldMousePos);
	else
		return m_ChainModel.DoMouseMoveIdle(ptWorldMousePos);
}

void GfxPrimitiveModel::DoTransformCreation(glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	if(m_bIsShape)
		return m_ShapeModel.DoTransformCreation(ptStartPos, ptDragPos);
	else
		return m_ChainModel.DoTransformCreation(ptStartPos,ptDragPos);
}
