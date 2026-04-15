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
}

/*virtual*/ GfxShapeHyView::~GfxShapeHyView()
{
}

/*virtual*/ void GfxShapeHyView::OnSyncModel(EditModeState eEditModeState, EditModeAction eEditModeAction) /*override*/
{
	if(eEditModeState == EDITMODE_Off || m_pModel == nullptr || GetShapeModel()->GetShapeType() == SHAPE_None || GetShapeModel()->GetNumShapeFixtures() == 0)
	{
		m_CenterGrabPoint.SetVisible(false);
		m_DataPrim.RemoveAllLayers();
		return;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Sync Primitives with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	
	float fOutlineThickness = 1.0f;
	if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() != SHAPE_Polygon)
	{
		m_DataPrim.SetAsShape(LAYER_Fill, *static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0), 0.0f);
		if(m_DataPrim.GetLayerType(LAYER_Fill) != HYFIXTURE_Nothing)
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

				m_DataPrim.SetAsPolygon(LAYER_Outline, projectedVertList, fOutlineThickness);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Circle)
			{
				b2Circle circle = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsCircle();

				glm::vec2 ptCenter(circle.center.x, circle.center.y);
				pCamera->ProjectToCamera(ptCenter, ptCenter);
				float fRadius = circle.radius * pCamera->GetZoom();

				m_DataPrim.SetAsCircle(LAYER_Outline, ptCenter, fRadius, fOutlineThickness);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_LineSegment)
			{
				b2Segment seg = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsSegment();
				glm::vec2 ptOne(seg.point1.x, seg.point1.y);
				pCamera->ProjectToCamera(ptOne, ptOne);
				glm::vec2 ptTwo(seg.point2.x, seg.point2.y);
				pCamera->ProjectToCamera(ptTwo, ptTwo);

				m_DataPrim.SetAsLineSegment(LAYER_Outline, ptOne, ptTwo, fOutlineThickness);
			}
			else if(static_cast<GfxShapeModel *>(m_pModel)->GetShapeType() == SHAPE_Capsule)
			{
				b2Capsule capsule = static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(0)->GetAsCapsule();
				glm::vec2 ptOne(capsule.center1.x, capsule.center1.y);
				pCamera->ProjectToCamera(ptOne, ptOne);
				glm::vec2 ptTwo(capsule.center2.x, capsule.center2.y);
				pCamera->ProjectToCamera(ptTwo, ptTwo);
				float fRadius = capsule.radius * pCamera->GetZoom();

				m_DataPrim.SetAsCapsule(LAYER_Outline, ptOne, ptTwo, fRadius, SHAPE_Capsule);
			}
			else
				HyGuiLog("GfxShapeHyView::RefreshView - Unsupported shape type for primitive sync", LOGTYPE_Error);
		}
		else
			m_DataPrim.SetAsNothing(LAYER_Outline);
	}
	else // SHAPE_Polygon
	{
		int iNumFixtures = static_cast<GfxShapeModel *>(m_pModel)->GetNumShapeFixtures();
		for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
			m_DataPrim.SetAsShape(LAYER_Fill + iIndex, *static_cast<GfxShapeModel *>(m_pModel)->GetShapeFixture(iIndex), 0.0f);

		// Set `m_PrimOutline`
		const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
		std::vector<glm::vec2> projectedVertList;
		for(int i = 0; i < grabPointModelList.size(); ++i)
		{
			glm::vec2 ptScreenPos;
			pCamera->ProjectToCamera(grabPointModelList[i].GetPos(), ptScreenPos);
			projectedVertList.push_back(ptScreenPos);
		}
		m_DataPrim.SetAsLineChain(LAYER_Outline, projectedVertList, static_cast<GfxShapeModel *>(m_pModel)->IsLoopClosed(), fOutlineThickness);
	}

	if(eEditModeAction == EDITMODEACTION_HoverGrabPoint && eEditModeState == EDITMODE_MouseDragTransform)
		DoGrabPointPreview(eEditModeState, eEditModeAction);
}

void GfxShapeHyView::DoGrabPointPreview(EditModeState eEditModeState, EditModeAction eEditModeAction)
{
	int iGrabPointIndex = m_pModel->GetActiveGrabPointIndex();
	glm::vec2 vDragDelta = m_pModel->GetDragDelta();

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

GfxShapeModel *GfxShapeHyView::GetShapeModel()
{
	return static_cast<GfxShapeModel *>(m_pModel);
}
