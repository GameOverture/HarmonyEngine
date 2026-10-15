/**************************************************************************
 *	VectorView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "VectorView.h"
#include "VectorModel.h"

VectorView::VectorView(HyEntity2d *pParent /*= nullptr*/) :
	IEditModeView(pParent),
	m_GrabPointOutline(this),
	m_CenterGrabPoint(this)
{
	m_GrabPointOutline.UseWindowCoordinates();
}

/*virtual*/ VectorView::~VectorView()
{
	ClearGrabPoints();
}

/*virtual*/ void VectorView::CalcLocalBoundingShape(HyShape2d &shapeOut) /*override*/
{
	VectorModel *pVectorModel = static_cast<VectorModel *>(m_pModel);
	pVectorModel->GetHyNode()->CalcLocalBoundingShape(shapeOut);
}

void VectorView::SyncColor()
{
	if(m_pModel)
		m_GrabPointOutline.SetTint(static_cast<VectorModel *>(m_pModel)->GetWidgetColor());
}

/*virtual*/ void VectorView::SyncWithModel(EditModeState eEditModeState) /*override*/
{
	VectorModel *pVectorModel = static_cast<VectorModel *>(m_pModel);

	if(pVectorModel == nullptr ||
		(pVectorModel->IsLineChain() == false && pVectorModel->GetShapeType() == SHAPE_None) ||
		pVectorModel->GetEditModeType() == EDITMODETYPE_FixturePoint)
	{
		ClearGrabPoints();
		m_GrabPointOutline.RemoveAllLayers();

		if(pVectorModel->GetEditModeType() == EDITMODETYPE_FixturePoint)
		{
			m_CenterGrabPoint.Sync(&pVectorModel->GetCenterGrabPoint());
			m_CenterGrabPoint.SetVisible(eEditModeState != EDITMODE_Off);
		}

		return;
	}

	if(pVectorModel->IsFixture())
	{
		m_GrabPointOutline.SetDisplayOrder(pVectorModel->GetDisplayOrder());
	}

	// Sync Grab Point Views with Model
	const QList<GfxGrabPointModel> &grabPointModelList = pVectorModel->GetGrabPointList();
	while(static_cast<uint32>(m_GrabPointViewList.size()) > grabPointModelList.size())
	{
		delete m_GrabPointViewList.back();
		m_GrabPointViewList.pop_back();
	}
	while(static_cast<uint32>(m_GrabPointViewList.size()) < grabPointModelList.size())
		m_GrabPointViewList.push_back(new GfxGrabPointView(this));

	for(int i = 0; i < grabPointModelList.size(); ++i)
		m_GrabPointViewList[i]->Sync(&grabPointModelList[i]);

	m_CenterGrabPoint.Sync(&pVectorModel->GetCenterGrabPoint());

	bool bGrabPtsVisible = (eEditModeState != EDITMODE_Off);
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		pGrabPtView->SetVisible(bGrabPtsVisible);
	m_CenterGrabPoint.SetVisible(bGrabPtsVisible);
	m_GrabPointOutline.SetVisible(bGrabPtsVisible);

	// Sync Fixture Primitive with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);

	if(pVectorModel->IsLineChain() || pVectorModel->GetShapeType() == SHAPE_Polygon)
	{
		std::vector<glm::vec2> outlinePtList;
		for(const GfxGrabPointModel &pointRef : grabPointModelList)
		{
			glm::vec2 ptCameraPoint;
			pCamera->ProjectToCamera(pointRef.GetPos(), ptCameraPoint);
			outlinePtList.push_back(ptCameraPoint);
		}
		m_GrabPointOutline.SetAsLineChain(0, outlinePtList, pVectorModel->IsLoopClosed(), pVectorModel->GetOutline());

		//if(pVectorModel->GetShapeType() == SHAPE_Polygon)
		//{
		//	int iNumFixtures = pVectorModel->GetNumFixtures();
		//	for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
		//		m_ScenePrim.SetAsFixture(iIndex, *pVectorModel->GetFixture(iIndex), 0.0f);
		//}
		//else
		//	m_ScenePrim.RemoveAllLayers();
	}
	else if(pVectorModel->GetEditModeType() == EDITMODETYPE_PrimitiveShape || pVectorModel->GetEditModeType() == EDITMODETYPE_FixtureShape) // Shape that isn't Polygon
	{
		switch(pVectorModel->GetShapeType())
		{
		case SHAPE_Box:
			if(grabPointModelList.empty() == false)
			{
				std::vector<glm::vec2> vertexList;
				glm::vec2 ptCameraPoint;

				pCamera->ProjectToCamera(grabPointModelList[0].GetPos(), ptCameraPoint);
				vertexList.push_back(ptCameraPoint);
				pCamera->ProjectToCamera(grabPointModelList[2].GetPos(), ptCameraPoint);
				vertexList.push_back(ptCameraPoint);
				pCamera->ProjectToCamera(grabPointModelList[4].GetPos(), ptCameraPoint);
				vertexList.push_back(ptCameraPoint);
				pCamera->ProjectToCamera(grabPointModelList[6].GetPos(), ptCameraPoint);
				vertexList.push_back(ptCameraPoint);

				m_GrabPointOutline.SetAsLineChain(0, vertexList, true, 1.0f);
			}
			break;

		case SHAPE_Circle: {
			glm::vec2 ptCenter = pVectorModel->GetCenterGrabPoint().GetPos();
			pCamera->ProjectToCamera(ptCenter, ptCenter);
			float fRadius = glm::length(grabPointModelList[0].GetPos() - pVectorModel->GetCenterGrabPoint().GetPos());
			fRadius *= pCamera->GetZoom();
			m_GrabPointOutline.SetAsCircle(0, ptCenter, fRadius, 1.0f);
			break; }

		case SHAPE_Capsule:
			m_GrabPointOutline.RemoveAllLayers();
			break;
			
		case SHAPE_LineSegment:
			if(pVectorModel->GetNumFixtures() > 0)
			{
				b2Segment seg = static_cast<const HyShape2d *>(pVectorModel->GetFixture(0))->GetAsSegment();
				glm::vec2 ptOne(seg.point1.x, seg.point1.y);
				pCamera->ProjectToCamera(ptOne, ptOne);
				glm::vec2 ptTwo(seg.point2.x, seg.point2.y);
				pCamera->ProjectToCamera(ptTwo, ptTwo);

				m_GrabPointOutline.SetAsLineSegment(0, ptOne, ptTwo, 1.0f);
			}
			break;

		default:
			HyGuiLog("VectorView::SyncWithModel - Unhandled shape type", LOGTYPE_Error);
		}
	}

	SyncColor();
}

void VectorView::ClearGrabPoints()
{
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		delete pGrabPtView;
	m_GrabPointViewList.clear();

	m_CenterGrabPoint.SetVisible(false);
}

/*virtual*/ void VectorView::OnUpdate() /*override*/
{
	//VectorModel *pVectorModel = static_cast<VectorModel *>(m_pModel);
	//if(m_bUpdatePrimNodeFlash == false || pVectorModel == nullptr)
	//	return;

	//int iLayerIndex;
	//HyPrimitive2d *pHyPrimNode = pVectorModel->GetPrimNode(iLayerIndex);
	//if(pHyPrimNode == nullptr)
	//	return;

	////if(pHyPrimNode->GetLayerAlpha(iLayerIndex)->IsAnimating() == false)
	////{
	////	if(pHyPrimNode->GetLayerAlpha(iLayerIndex)->Get() == 1.0f)
	////		pHyPrimNode->GetLayerAlpha(iLayerIndex)->Tween(0.5f, 1.00f, HyTween::QuadIn);
	////	else
	////		pHyPrimNode->GetLayerAlpha(iLayerIndex)->Tween(1.0f, 1.00f, HyTween::QuadOut);
	////}
}
