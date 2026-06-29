/**************************************************************************
 *	EditModeView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "EditModeView.h"

EditModeView::EditModeView(HyEntity2d *pParent /*= nullptr*/) :
	IEditModeView(pParent),
	m_CameraPrim(this),
	m_ScenePrim(this),
	m_CenterGrabPoint(this)
{
	m_CameraPrim.UseWindowCoordinates();
}

/*virtual*/ EditModeView::~EditModeView()
{
	ClearGrabPoints();
}

void EditModeView::SyncColor()
{
	if(m_pModel)
	{
		m_CameraPrim.SetTint(static_cast<EditModeModel *>(m_pModel)->GetColor());
		m_ScenePrim.SetTint(static_cast<EditModeModel *>(m_pModel)->GetColor());
	}
}

/*virtual*/ void EditModeView::SyncWithModel(EditModeState eEditModeState) /*override*/
{
	EditModeModel *pVectorModel = static_cast<EditModeModel *>(m_pModel);

	if(pVectorModel == nullptr ||
		(pVectorModel->IsLineChain() == false && pVectorModel->GetShapeType() == SHAPE_None) ||
		pVectorModel->GetEditModeType() == EDITMODETYPE_FixturePoint)
	{
		ClearGrabPoints();
		m_CameraPrim.RemoveAllLayers();
		m_ScenePrim.RemoveAllLayers();

		if(pVectorModel->GetEditModeType() == EDITMODETYPE_FixturePoint)
		{
			m_CenterGrabPoint.Sync(&pVectorModel->GetCenterGrabPoint());
			m_CenterGrabPoint.SetVisible(eEditModeState != EDITMODE_Off);
		}

		return;
	}

	if(pVectorModel->IsFixture())
	{
		m_CameraPrim.SetDisplayOrder(pVectorModel->GetDisplayOrder() + 1);
		m_ScenePrim.SetDisplayOrder(pVectorModel->GetDisplayOrder());
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
		m_CameraPrim.SetAsLineChain(0, outlinePtList, pVectorModel->IsLoopClosed(), pVectorModel->GetOutline());

		if(pVectorModel->GetShapeType() == SHAPE_Polygon)
		{
			int iNumFixtures = pVectorModel->GetNumFixtures();
			for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
				m_ScenePrim.SetAsFixture(iIndex, *pVectorModel->GetFixture(iIndex), 0.0f);
		}
		else
			m_ScenePrim.RemoveAllLayers();
	}
	else if(pVectorModel->GetEditModeType() == EDITMODETYPE_PrimitiveShape || pVectorModel->GetEditModeType() == EDITMODETYPE_FixtureShape) // Shape that isn't Polygon
	{
		switch(pVectorModel->GetShapeType())
		{
		case SHAPE_Box:
		case SHAPE_Circle:
		case SHAPE_Capsule: {
			m_CameraPrim.RemoveAllLayers();
			int iNumFixtures = pVectorModel->GetNumFixtures();
			for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
				m_ScenePrim.SetAsFixture(iIndex, *pVectorModel->GetFixture(iIndex), 0.0f);
			break; }
			
		case SHAPE_LineSegment:
			if(pVectorModel->GetNumFixtures() > 0)
			{
				b2Segment seg = static_cast<const HyShape2d *>(pVectorModel->GetFixture(0))->GetAsSegment();
				glm::vec2 ptOne(seg.point1.x, seg.point1.y);
				pCamera->ProjectToCamera(ptOne, ptOne);
				glm::vec2 ptTwo(seg.point2.x, seg.point2.y);
				pCamera->ProjectToCamera(ptTwo, ptTwo);

				m_CameraPrim.SetAsLineSegment(0, ptOne, ptTwo, pVectorModel->GetOutline());
				m_ScenePrim.RemoveAllLayers();
			}
			break;

		default:
			HyGuiLog("EditModeView::SyncWithModel - Unhandled shape type", LOGTYPE_Error);
		}
	}

	SyncColor();
}

void EditModeView::ClearGrabPoints()
{
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		delete pGrabPtView;
	m_GrabPointViewList.clear();

	m_CenterGrabPoint.SetVisible(false);
}
