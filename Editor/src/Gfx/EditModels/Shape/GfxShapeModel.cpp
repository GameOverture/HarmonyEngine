///**************************************************************************
//*	GfxShapeModel.cpp
//*
//*	Harmony Engine - Editor Tool
//*	Copyright (c) 2025 Jason Knobler
//*
//*	Harmony Editor Tool License:
//*	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
//*************************************************************************/
//#include "Global.h"
//#include "GfxShapeModel.h"
//#include "IGfxEditView.h"
//
//GfxShapeModel::GfxShapeModel(HyColor color) :
//	IGfxEditModel(EDITMODETYPE_Shape, color)
//{
//}
//
///*virtual*/ GfxShapeModel::~GfxShapeModel()
//{
//	ClearFixtures();
//}
//
//EditorShape GfxShapeModel::GetShapeType() const
//{
//	return m_eShapeType;
//}
//
//void GfxShapeModel::SetShapeType(EditorShape eNewShape, QList<float> floatList)
//{
//	if(floatList.empty())
//	{
//		
//	}
//
//	m_eShapeType = eNewShape;
//
//	ClearFixtures();
//	m_FixtureList.push_back(new HyShape2d());
//
//
//}
//
//
//
//HyShape2d *GfxShapeModel::GetShape(int iIndex)
//{
//	if(iIndex < 0 || iIndex >= m_FixtureList.size())
//		return nullptr;
//	return static_cast<HyShape2d *>(m_FixtureList[iIndex]);
//}
//
//const HyShape2d *GfxShapeModel::GetShape(int iIndex) const
//{
//	if(iIndex < 0 || iIndex >= m_FixtureList.size())
//		return nullptr;
//	return static_cast<const HyShape2d *>(m_FixtureList[iIndex]);
//}
//
//bool GfxShapeModel::IsLoopClosed() const
//{
//	return m_bLoopClosed;
//}
//
///*virtual*/ QString GfxShapeModel::GetActionText(QString sNodeCodeName) const /*override*/
//{
//
//}
//
/////*virtual*/ QJsonObject GfxShapeModel::GetActionSerialized() const /*override*/
////{
////	switch(m_eCurAction)
////	{
////	case EDITMODEACTION_Creation:
////		return Serialize();
////
////	case EDITMODEACTION_Inside:
////		if(IsAllGrabPointsSelected() == false)
////			HyGuiLog("GfxShapeModel::GetActionSerialized - EDITMODEACTION_Inside with not all grab points selected", LOGTYPE_Error);
////		[[fallthrough]];
////	case EDITMODEACTION_HoverCenter:
////		if(m_eShapeType != SHAPE_Polygon)
////		{
////			if(m_FixtureList.size() != 1)
////				HyGuiLog("GfxShapeModel::GetActionSerialized - Expected exactly one shape fixture for non-polygon shape type", LOGTYPE_Error);
////			
////			// Translate entire shape by the drag delta.
////			HyShape2d tmpShape = *GetShape(0);
////			tmpShape.TransformSelf(glm::translate(glm::mat4(1.0f), glm::vec3(m_vDragDelta, 0.0f)));
////
////			std::vector<float> serializedData = tmpShape.SerializeSelf();
////			QJsonObject serializedObj;
////			serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));
////			QJsonArray dataArray;
////			for(float f : serializedData)
////				dataArray.push_back(f);
////			serializedObj.insert("data", dataArray);
////			serializedObj.insert("outline", m_fOutline);
////			return serializedObj;
////		}
////		else // SHAPE_Polygon
////		{
////			QList<float> returnList;
////			for(const GfxGrabPointModel &grabPt : m_GrabPointList)
////			{
////				glm::vec2 ptVertex = grabPt.GetPos();
////				returnList.push_back(static_cast<float>(ptVertex.x));
////				returnList.push_back(static_cast<float>(ptVertex.y));
////			}
////
////			// Translate entire shape by the drag delta.
////			for(int i = 0; i < returnList.size(); i += 2)
////			{
////				returnList[i] += m_vDragDelta.x;
////				returnList[i + 1] += m_vDragDelta.y;
////			}
////
////			returnList.push_back(m_bLoopClosed ? 1.0f : 0.0f); // Final float indicates whether loop is closed
////
////			QJsonObject serializedObj;
////			serializedObj.insert("type", HyGlobal::ShapeName(m_eShapeType));
////			QJsonArray dataArray;
////			for(float f : returnList)
////				dataArray.push_back(f);
////			serializedObj.insert("data", dataArray);
////			serializedObj.insert("outline", m_fOutline);
////
////			return serializedObj;
////		}
////		break;
////
////	case EDITMODEACTION_AppendVertex:
////	case EDITMODEACTION_InsertVertex:
////		// Guaranteed to be SHAPE_Polygon, translate all (selected) vertices by the drag delta.
////		// AppendVertex/InsertVertex both work with EDITMODEACTION_HoverGrabPoint's SHAPE_Polygon logic, so they can share the same serialization format.
////		[[fallthrough]];
////	case EDITMODEACTION_HoverGrabPoint:
////		switch(m_eShapeType)
////		{
////		case SHAPE_Box:
////
////			break;
////		case SHAPE_Circle:
////			break;
////		case SHAPE_LineSegment:
////			break;
////		case SHAPE_Polygon:
////			break;
////		case SHAPE_Capsule:
////			break;
////
////		default:
////			HyGuiLog("GfxShapeModel::GetActionSerialized - Unknown shape type encountered", LOGTYPE_Error);
////			break;
////		}
////		break;
////
////	default:
////		HyGuiLog("GfxShapeModel::GetActionSerialized - Invalid m_eCurTransform", LOGTYPE_Error);
////		break;
////	}
////
////	return QJsonObject();
////}
//
///*virtual*/ QString GfxShapeModel::DoDeserialize(const QJsonObject &serializedObj) /*override*/
//{
//	
//}
//
///*virtual*/ EditModeAction GfxShapeModel::DoMouseMoveIdle() /*override*/
//{
//
//}
//
//void GfxShapeModel::DoTransformCreation(bool bShiftMod, glm::vec2 ptStartPos, glm::vec2 ptDragPos)
//{
//
//}
//
//
//
//
//
//
//
//
//
//
//
//
