/**************************************************************************
*	GfxPrimLayerModel.cpp
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

GfxPrimLayerModel::GfxPrimLayerModel() :
	IGfxEditModel(EDITMODETYPE_Shape, HyColor::White),
	m_bIsShape(true),
	m_ShapeModel(HyColor::White),
	m_ChainModel(HyColor::White)
{
}

/*virtual*/ GfxPrimLayerModel::~GfxPrimLayerModel()
{
}

/*virtual*/ bool GfxPrimLayerModel::IsValidModel() const /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.IsValidModel();
	else
		return m_ChainModel.IsValidModel();
}

bool GfxPrimLayerModel::IsShapeModel() const
{
	return m_bIsShape;
}

QString GfxPrimLayerModel::GetPrimType() const
{
	if(m_bIsShape)
		return HyGlobal::ShapeName(m_ShapeModel.GetShapeType());
	else
		return "Line Chain";
}

//void GfxPrimLayerModel::SetPrimType(QString sNewType, QList<float> floatList)
//{
//	if(sNewType == "Line Chain")
//	{
//		if(floatList.empty())
//		{
//			if(m_bIsShape)
//				floatList = m_ShapeModel.ConvertedPolygonOrLineChainData();
//			else
//				floatList = m_ChainModel.Serialize();
//		}
//		m_bIsShape = false;
//		m_ChainModel.Deserialize(floatList);
//	}
//	else
//	{
//		m_bIsShape = true;
//		EditorShape eNewShape = HyGlobal::GetShapeFromString(sNewType);
//		m_ShapeModel.SetShapeType(eNewShape, floatList);
//	}
//
//	SyncViews(EDITMODE_Idle, EDITMODEACTION_None);
//}

/*virtual*/ QJsonObject GfxPrimLayerModel::Serialize() const /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.Serialize();
	else
		return m_ChainModel.Serialize();
}

/*virtual*/ QString GfxPrimLayerModel::GetActionText(QString sNodeCodeName) const /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.GetActionText(sNodeCodeName);
	else
		return m_ChainModel.GetActionText(sNodeCodeName);
}

/*virtual*/ QJsonObject GfxPrimLayerModel::GetActionSerialized() const /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.GetActionSerialized();
	else
		return m_ChainModel.GetActionSerialized();
}

/*virtual*/ QString GfxPrimLayerModel::DoDeserialize(const QJsonObject &floatList) /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.DoDeserialize(floatList);
	else
		return m_ChainModel.DoDeserialize(floatList);
}

/*virtual*/ EditModeAction GfxPrimLayerModel::DoMouseMoveIdle(glm::vec2 ptWorldMousePos) /*override*/
{
	if(m_bIsShape)
		return m_ShapeModel.DoMouseMoveIdle(ptWorldMousePos);
	else
		return m_ChainModel.DoMouseMoveIdle(ptWorldMousePos);
}

void GfxPrimLayerModel::DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
{
	if(m_bIsShape)
		return m_ShapeModel.DoTransformCreation(bShiftMod, ptStartPos, ptDragPos);
	else
		return m_ChainModel.DoTransformCreation(bShiftMod, ptStartPos,ptDragPos);
}
