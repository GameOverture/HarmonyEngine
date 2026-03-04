/**************************************************************************
 *	GfxShapeHyView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2025 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GfxShapeHyView.h"
#include "GfxShapeModel.h"
#include "GfxGrabPointView.h"

GfxShapeHyView::GfxShapeHyView(bool bIsFixture, HyEntity2d *pParent /*= nullptr*/) :
	IGfxEditView(pParent),
	m_bIsFixture(bIsFixture)
{
	// NOTE: m_Prim does not have a parent because it is projected to window coordinates
	m_Prim.UseWindowCoordinates();
	m_Prim.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);
}

/*virtual*/ GfxShapeHyView::~GfxShapeHyView()
{
}

/*virtual*/ void GfxShapeHyView::SyncColor() /*override*/
{
	if(m_pModel == nullptr)
		return;

	m_Prim.SetTint(m_pModel->GetColor());
}

/*virtual*/ void GfxShapeHyView::ClearPreview() /*override*/
{
	m_Prim.RemoveLayer(LAYER_Preview);
}

/*virtual*/ void GfxShapeHyView::OnSyncModel(EditModeState eEditModeState, EditModeAction eResult) /*override*/
{
	if(m_pModel == nullptr || static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_None)
	{
		m_Prim.RemoveAllLayers();
		return;
	}
	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sync Primitives with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	
	float fOutlineThickness = 1.0f;
	if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() != SHAPE_Polygon)
	{
		m_Prim.SetAsShape(LAYER_Fill, *static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0));
		if(m_Prim.GetLayerType(LAYER_Fill) != HYFIXTURE_Nothing)
		{
			if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Box)
			{
				b2Vec2 *pVerts = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsPolygon().vertices;
				std::vector<glm::vec2> projectedVertList;
				for(int i = 0; i < 4; ++i)
				{
					glm::vec2 ptScreenPos;
					pCamera->ProjectToCamera(glm::vec2(pVerts[i].x, pVerts[i].y), ptScreenPos);
					projectedVertList.push_back(ptScreenPos);
				}

				m_Prim.SetAsPolygon(LAYER_Outline, projectedVertList, fOutlineThickness);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Circle)
			{
				b2Circle circle = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsCircle();

				glm::vec2 ptCenter(circle.center.x, circle.center.y);
				pCamera->ProjectToCamera(ptCenter, ptCenter);
				float fRadius = circle.radius * pCamera->GetZoom();

				m_Prim.SetAsCircle(LAYER_Outline, ptCenter, fRadius, fOutlineThickness);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_LineSegment)
			{
				b2Segment seg = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsSegment();
				glm::vec2 ptOne(seg.point1.x, seg.point1.y);
				pCamera->ProjectToCamera(ptOne, ptOne);
				glm::vec2 ptTwo(seg.point2.x, seg.point2.y);
				pCamera->ProjectToCamera(ptTwo, ptTwo);

				m_Prim.SetAsLineSegment(LAYER_Outline, ptOne, ptTwo, fOutlineThickness);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Capsule)
			{
				b2Capsule capsule = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsCapsule();
				glm::vec2 ptOne(capsule.center1.x, capsule.center1.y);
				pCamera->ProjectToCamera(ptOne, ptOne);
				glm::vec2 ptTwo(capsule.center2.x, capsule.center2.y);
				pCamera->ProjectToCamera(ptTwo, ptTwo);
				float fRadius = capsule.radius * pCamera->GetZoom();

				m_Prim.SetAsCapsule(LAYER_Outline, ptOne, ptTwo, fRadius, SHAPE_Capsule);
			}
			else
				HyGuiLog("GfxShapeHyView::RefreshView - Unsupported shape type for primitive sync", LOGTYPE_Error);
		}
		else
			m_Prim.SetAsNothing(LAYER_Outline);
	}
	else // SHAPE_Polygon
	{
		int iNumFixtures = static_cast<GfxShapeModel *>(m_pModel)->GetNumShapeFixtures();
		for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
			m_Prim.SetAsShape(LAYER_Fill + iIndex, *static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(iIndex));

		// Set `m_PrimOutline`
		const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
		std::vector<glm::vec2> projectedVertList;
		for(int i = 0; i < grabPointModelList.size(); ++i)
		{
			glm::vec2 ptScreenPos;
			pCamera->ProjectToCamera(grabPointModelList[i].GetPos(), ptScreenPos);
			projectedVertList.push_back(ptScreenPos);
		}
		m_Prim.SetAsLineChain(LAYER_Outline, projectedVertList, static_cast<GfxShapeModel *>(m_pModel)->IsLoopClosed(), fOutlineThickness);
	}
}

/*virtual*/ void GfxShapeHyView::OnSyncPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta) /*override*/
{
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);

	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
	switch(eEditModeAction)
	{
	case EDITMODEACTION_Creation:
		break;
	case EDITMODEACTION_Outside:
		break;
	case EDITMODEACTION_Inside:
		break;

	case EDITMODEACTION_AppendVertex: {
		if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() != SHAPE_Polygon)
		{
			HyGuiLog("GfxShapeHyView::RefreshView - EDITMODEACTION_AppendVertex - called with non-polygon/linechain shape type", LOGTYPE_Error);
			break;
		}
		if(static_cast<GfxShapeModel *>(m_pModel)->IsLoopClosed() || grabPointModelList.empty())
		{
			HyGuiLog("GfxShapeHyView::RefreshView called with closed loop (or grab points empty)", LOGTYPE_Error);
			break;
		}
		//if(m_PrimPreviewList.empty())
		//{
		//	m_PrimPreviewList.append(new HyPrimitive2d(this));
		//	m_PrimPreviewList[0]->UseWindowCoordinates();
		//	m_PrimPreviewList[0]->SetTint(HyGlobal::GetEditorColor(EDITORCOLOR_EditMode));
		//	m_PrimPreviewList[0]->SetDisplayOrder(DISPLAYORDER_TransformCtrl - 2);
		//}
		//if(m_PrimPreviewList.size() != 1)
		//	HyGuiLog("GfxShapeHyView::RefreshView - EDITMODEACTION_AppendVertex - m_PrimPreviewList should have exactly 1 primitive", LOGTYPE_Error);
		
		glm::vec2 ptNewVertex = grabPointModelList[iGrabPointIndex].GetPos();
		ptNewVertex += vDragDelta;
		pCamera->ProjectToCamera(ptNewVertex, ptNewVertex);
		m_GrabPointViewList[iGrabPointIndex]->pos.Set(ptNewVertex);

		glm::vec2 ptEndPoint;
		if(grabPointModelList.front().IsSelected())
			ptEndPoint = grabPointModelList.front().GetPos();
		else
			ptEndPoint = grabPointModelList.back().GetPos();
		pCamera->ProjectToCamera(ptEndPoint, ptEndPoint);
		
		//m_PrimPreviewList[0]->SetAsLineSegment(ptNewVertex, ptEndPoint);
		break; }

	case EDITMODEACTION_InsertVertex: {
		if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() != SHAPE_Polygon)
		{
			HyGuiLog("GfxShapeHyView::RefreshView - EDITMODEACTION_InsertVertex - called with non-polygon/linechain shape type", LOGTYPE_Error);
			break;
		}
		if(grabPointModelList.size() < 2)
		{
			HyGuiLog("GfxShapeHyView::RefreshView called with less than 2 grab points", LOGTYPE_Error);
			break;
		}

		glm::vec2 ptInsertVertex = grabPointModelList[iGrabPointIndex].GetPos();
		ptInsertVertex += vDragDelta;
		pCamera->ProjectToCamera(ptInsertVertex, ptInsertVertex);
		m_GrabPointViewList[iGrabPointIndex]->pos.Set(ptInsertVertex);

		glm::vec2 ptConnectPoint1 = grabPointModelList[(iGrabPointIndex + 1) % grabPointModelList.size()].GetPos();
		pCamera->ProjectToCamera(ptConnectPoint1, ptConnectPoint1);

		glm::vec2 ptConnectPoint2;
		if(iGrabPointIndex == 0)
			ptConnectPoint2 = grabPointModelList[grabPointModelList.size() - 1].GetPos();
		else
			ptConnectPoint2 = grabPointModelList[iGrabPointIndex - 1].GetPos();
		pCamera->ProjectToCamera(ptConnectPoint2, ptConnectPoint2);
		
		//m_PrimPreviewList[0]->SetAsLineSegment(ptInsertVertex, ptConnectPoint1);
		//m_PrimPreviewList[1]->SetAsLineSegment(ptInsertVertex, ptConnectPoint2);
		break; }

	case EDITMODEACTION_HoverGrabPoint:
		if(eEditModeState == EDITMODE_MouseDragTransform)
			DoGrabPointPreview(eEditModeState, eEditModeAction, iGrabPointIndex, vDragDelta);
		break;

	case EDITMODEACTION_HoverCenter:
		if(eEditModeState == EDITMODE_MouseDragTransform)
		{
			
			

			//for(HyPrimitive2d *pPrim : m_PrimPreviewList)
			//{
			//	HyShape2d tmpShape;
			//	pPrim->CalcLocalBoundingShape(tmpShape);
			//	glm::mat4 mtxTransform = glm::translate(mtxTransform, glm::vec3(vDragDelta, 0.0f));
			//	tmpShape.TransformSelf(mtxTransform);
			//	pPrim->SetAsShape(tmpShape);
			//}
			//
			//for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
			//	pGrabPtView->pos.Offset(vDragDelta);
		}
		break;
	}
}

void GfxShapeHyView::DoGrabPointPreview(EditModeState eEditModeState, EditModeAction eEditModeAction, int iGrabPointIndex, glm::vec2 vDragDelta)
{
	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
	if(iGrabPointIndex < 0 || iGrabPointIndex >= grabPointModelList.size())
	{
		HyGuiLog("GfxShapeHyView::DoHoverGrabPoint - invalid m_iGrabPointIndex", LOGTYPE_Error);
		return;
	}

	// Apply grab point drag logic based on shape type
	switch(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType())
	{
	case SHAPE_Box: // Lock vertices together to keep box form
		if(m_pModel->GetNumGrabPointsSelected() == 1)
		{
			glm::vec2 ptVertPos = grabPointModelList[iGrabPointIndex].GetPos();

			// Find the opposite/locked vertex by iterating over all vertices and finding the one that is farthest from iGrabPointIndex
			int iLockedVertIndex = -1;
			float fMaxDistance = -1.0f;
			for(int i = 0; i < grabPointModelList.size(); ++i)
			{
				float fDistance = glm::distance(ptVertPos, grabPointModelList[i].GetPos());
				if(fDistance > fMaxDistance)
				{
					fMaxDistance = fDistance;
					iLockedVertIndex = i;
				}
			}

			// Translate the selected vertex
			m_GrabPointViewList[iGrabPointIndex]->pos.Set(ptVertPos + vDragDelta);

			// Determine the lower/upper bounds based on the selected and locked vertices
			glm::vec2 ptSelectedVert = m_GrabPointViewList[iGrabPointIndex]->pos.Get();
			glm::vec2 ptLockedVert = m_GrabPointViewList[iLockedVertIndex]->pos.Get();
			glm::vec2 ptLowerBound, ptUpperBound;
			HySetVec(ptLowerBound, ptLockedVert.x < ptSelectedVert.x ? ptLockedVert.x : ptSelectedVert.x, ptLockedVert.y < ptSelectedVert.y ? ptLockedVert.y : ptSelectedVert.y);
			HySetVec(ptUpperBound, ptLockedVert.x >= ptSelectedVert.x ? ptLockedVert.x : ptSelectedVert.x, ptLockedVert.y >= ptSelectedVert.y ? ptLockedVert.y : ptSelectedVert.y);

			// Update the other 2 vertices that aren't the selected or locked
			for(int i = 0; i < grabPointModelList.size(); ++i)
			{
				if(i == iGrabPointIndex || i == iLockedVertIndex)
					continue;

				glm::vec2 ptCurrentVert = m_GrabPointViewList[i]->pos.Get();
				if(ptCurrentVert.x == ptLockedVert.x)
					ptCurrentVert.x = ptUpperBound.x;
				else
					ptCurrentVert.x = ptLowerBound.x;
				if(ptCurrentVert.y == ptLockedVert.y)
					ptCurrentVert.y = ptUpperBound.y;
				else
					ptCurrentVert.y = ptLowerBound.y;
				m_GrabPointViewList[i]->pos.Set(ptCurrentVert);
			}
		}
		else // TODO: Better control when 2 verts selected
		{
			
		}
		break;
	case SHAPE_Circle:
		break;
	case SHAPE_LineSegment:
		break;
	case SHAPE_Polygon:
		break;
	case SHAPE_Capsule:
		break;

	default:
		HyGuiLog("GfxShapeHyView::DoHoverGrabPoint - Unsupported shape type for grab point transform: " % QString::number(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType()), LOGTYPE_Error);
		break;
	}
}
