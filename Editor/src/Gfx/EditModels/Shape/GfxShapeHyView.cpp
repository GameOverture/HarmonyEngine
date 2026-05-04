///**************************************************************************
// *	GfxShapeHyView.cpp
// *
// *	Harmony Engine - Editor Tool
// *	Copyright (c) 2025 Jason Knobler
// *
// *	Harmony Editor Tool License:
// *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
//#include "Global.h"
//#include "GfxShapeHyView.h"
//#include "GfxShapeModel.h"
//#include "GfxGrabPointView.h"
//
//GfxShapeHyView::GfxShapeHyView(bool bIsFixture, HyEntity2d *pParent /*= nullptr*/) :
//	EditModeView(pParent),
//	m_bIsFixture(bIsFixture)
//{
//}
//
///*virtual*/ GfxShapeHyView::~GfxShapeHyView()
//{
//}
//
//void GfxShapeHyView::DoGrabPointPreview(EditModeState eEditModeState, EditModeAction eEditModeAction)
//{
//	int iGrabPointIndex = m_pModel->GetActiveGrabPointIndex();
//	glm::vec2 vDragDelta = m_pModel->GetDragDelta();
//
//	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
//	if(iGrabPointIndex < 0 || iGrabPointIndex >= grabPointModelList.size())
//	{
//		HyGuiLog("GfxShapeHyView::DoHoverGrabPoint - invalid m_iGrabPointIndex", LOGTYPE_Error);
//		return;
//	}
//
//	// Apply grab point drag logic based on shape type
//	switch(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType())
//	{
//	case SHAPE_Box: // Lock vertices together to keep box form
//		if(m_pModel->GetNumGrabPointsSelected() == 1)
//		{
//			glm::vec2 ptVertPos = grabPointModelList[iGrabPointIndex].GetPos();
//
//			// Find the opposite/locked vertex by iterating over all vertices and finding the one that is farthest from iGrabPointIndex
//			int iLockedVertIndex = -1;
//			float fMaxDistance = -1.0f;
//			for(int i = 0; i < grabPointModelList.size(); ++i)
//			{
//				float fDistance = glm::distance(ptVertPos, grabPointModelList[i].GetPos());
//				if(fDistance > fMaxDistance)
//				{
//					fMaxDistance = fDistance;
//					iLockedVertIndex = i;
//				}
//			}
//
//			// Translate the selected vertex
//			m_GrabPointViewList[iGrabPointIndex]->pos.Set(ptVertPos + vDragDelta);
//
//			// Determine the lower/upper bounds based on the selected and locked vertices
//			glm::vec2 ptSelectedVert = m_GrabPointViewList[iGrabPointIndex]->pos.Get();
//			glm::vec2 ptLockedVert = m_GrabPointViewList[iLockedVertIndex]->pos.Get();
//			glm::vec2 ptLowerBound, ptUpperBound;
//			HySetVec(ptLowerBound, ptLockedVert.x < ptSelectedVert.x ? ptLockedVert.x : ptSelectedVert.x, ptLockedVert.y < ptSelectedVert.y ? ptLockedVert.y : ptSelectedVert.y);
//			HySetVec(ptUpperBound, ptLockedVert.x >= ptSelectedVert.x ? ptLockedVert.x : ptSelectedVert.x, ptLockedVert.y >= ptSelectedVert.y ? ptLockedVert.y : ptSelectedVert.y);
//
//			// Update the other 2 vertices that aren't the selected or locked
//			for(int i = 0; i < grabPointModelList.size(); ++i)
//			{
//				if(i == iGrabPointIndex || i == iLockedVertIndex)
//					continue;
//
//				glm::vec2 ptCurrentVert = m_GrabPointViewList[i]->pos.Get();
//				if(ptCurrentVert.x == ptLockedVert.x)
//					ptCurrentVert.x = ptUpperBound.x;
//				else
//					ptCurrentVert.x = ptLowerBound.x;
//				if(ptCurrentVert.y == ptLockedVert.y)
//					ptCurrentVert.y = ptUpperBound.y;
//				else
//					ptCurrentVert.y = ptLowerBound.y;
//				m_GrabPointViewList[i]->pos.Set(ptCurrentVert);
//			}
//		}
//		else // TODO: Better control when 2 verts selected
//		{
//			
//		}
//		break;
//	case SHAPE_Circle:
//		break;
//	case SHAPE_LineSegment:
//		break;
//	case SHAPE_Polygon:
//		break;
//	case SHAPE_Capsule:
//		break;
//
//	default:
//		HyGuiLog("GfxShapeHyView::DoHoverGrabPoint - Unsupported shape type for grab point transform: " % QString::number(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType()), LOGTYPE_Error);
//		break;
//	}
//}
//
//GfxShapeModel *GfxShapeHyView::GetShapeModel()
//{
//	return static_cast<GfxShapeModel *>(m_pModel);
//}
