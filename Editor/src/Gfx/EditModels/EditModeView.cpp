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
	HyEntity2d(pParent),
	m_pModel(nullptr),
	m_CenterGrabPoint(this)
{
	m_CameraPrim.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 1);
	m_CameraPrim.UseCameraCoordinates();

	m_ScenePrim.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 2);
}

/*virtual*/ EditModeView::~EditModeView()
{
	ClearGrabPoints();

	if(m_pModel)
		m_pModel->RemoveView(this);
}

EditModeModel *EditModeView::GetModel() const
{
	return m_pModel;
}

void EditModeView::SetModel(EditModeModel *pModel)
{
	if(m_pModel == pModel)
		return;
	if(m_pModel)
		m_pModel->RemoveView(this);

	m_pModel = pModel;
	if(m_pModel)
		m_pModel->AddView(this);
}

void EditModeView::SyncColor()
{
	if(m_pModel)
	{
		m_CameraPrim.SetTint(m_pModel->GetColor());
		m_ScenePrim.SetTint(m_pModel->GetColor());
	}
}

void EditModeView::SyncWithModel(EditModeState eEditModeState, EditModeAction eEditModeAction)
{
	if(eEditModeState == EDITMODE_Off || m_pModel == nullptr || m_pModel->GetNumFixtures() == 0 || (m_pModel->IsLineChain() == false && m_pModel->GetShapeType() == SHAPE_None))
	{
		ClearGrabPoints();
		m_CameraPrim.RemoveAllLayers();
		m_ScenePrim.RemoveAllLayers();
		return;
	}

	// Sync Grab Point Views with Model
	const QList<GfxGrabPointModel> &grabPointModelList = m_pModel->GetGrabPointList();
	while(static_cast<uint32>(m_GrabPointViewList.size()) > grabPointModelList.size())
	{
		delete m_GrabPointViewList.back();
		m_GrabPointViewList.pop_back();
	}
	while(static_cast<uint32>(m_GrabPointViewList.size()) < grabPointModelList.size())
		m_GrabPointViewList.push_back(new GfxGrabPointView(this));

	for(int i = 0; i < grabPointModelList.size(); ++i)
		m_GrabPointViewList[i]->Sync(&grabPointModelList[i]);

	m_CenterGrabPoint.Sync(&m_pModel->GetCenterGrabPoint());

	// Sync Fixture Primitive with Model
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);

	if(m_pModel->IsLineChain() || m_pModel->GetShapeType() == SHAPE_Polygon)
	{
		std::vector<glm::vec2> outlinePtList;
		for(const GfxGrabPointModel &pointRef : grabPointModelList)
		{
			glm::vec2 ptCameraPoint;
			pCamera->ProjectToCamera(pointRef.GetPos(), ptCameraPoint);
			outlinePtList.push_back(ptCameraPoint);
		}
		m_CameraPrim.SetAsLineChain(0, outlinePtList, m_pModel->IsLoopClosed(), m_pModel->GetOutline());

		if(m_pModel->GetShapeType() == SHAPE_Polygon)
		{
			int iNumFixtures = m_pModel->GetNumFixtures();
			for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
				m_ScenePrim.SetAsFixture(iIndex, *m_pModel->GetFixture(iIndex), 0.0f);
		}
		else
			m_ScenePrim.RemoveAllLayers();
	}
	else // Shape that isn't Polygon
	{
		switch(m_pModel->GetShapeType())
		{
		case SHAPE_Box:
		case SHAPE_Circle:
		case SHAPE_Capsule: {
			m_CameraPrim.RemoveAllLayers();
			int iNumFixtures = m_pModel->GetNumFixtures();
			for(int iIndex = 0; iIndex < iNumFixtures; ++iIndex)
				m_ScenePrim.SetAsFixture(iIndex, *m_pModel->GetFixture(iIndex), 0.0f);
			break; }
			
		case SHAPE_LineSegment: {
			b2Segment seg = static_cast<const HyShape2d *>(m_pModel->GetFixture(0))->GetAsSegment();
			glm::vec2 ptOne(seg.point1.x, seg.point1.y);
			pCamera->ProjectToCamera(ptOne, ptOne);
			glm::vec2 ptTwo(seg.point2.x, seg.point2.y);
			pCamera->ProjectToCamera(ptTwo, ptTwo);

			m_CameraPrim.SetAsLineSegment(0, ptOne, ptTwo, m_pModel->GetOutline());
			m_ScenePrim.RemoveAllLayers();
			break; }

		default:
			HyGuiLog("EditModeView::SyncWithModel - Unhandled shape type", LOGTYPE_Error);
		}
	}


	//switch(eEditModeAction)
	//{
	//case EDITMODEACTION_Creation:
	//	break;
	//case EDITMODEACTION_Outside:
	//	break;
	//case EDITMODEACTION_Inside:
	//	break;

	//case EDITMODEACTION_AppendVertex:
	//	glm::vec2 ptEndPoint;
	//	if(grabPointModelList.front().IsSelected())
	//		ptEndPoint = grabPointModelList.front().GetPos();
	//	else
	//		ptEndPoint = grabPointModelList.back().GetPos();
	//		
	//	glm::vec2 ptGrabPtPos = m_pModel->GetActiveGrabPoint()->GetPos();
	//	pCamera->ProjectToCamera(ptGrabPtPos, ptGrabPtPos);
	//	pCamera->ProjectToCamera(ptEndPoint, ptEndPoint);
	//		
	//	m_PreviewPrim.RemoveAllLayers();
	//	m_PreviewPrim.SetAsLineSegment(0, ptGrabPtPos, ptEndPoint, 1.0f);
	//	break;

	//case EDITMODEACTION_InsertVertex:
	//	if(grabPointModelList.size() < 2)
	//		HyGuiLog("GfxChainView::RefreshView called with less than 2 grab points", LOGTYPE_Error);

	//	glm::vec2 ptInsertVertex = m_pModel->GetActiveGrabPoint()->GetPos();
	//	ptInsertVertex += m_pModel->GetDragDelta();
	//	pCamera->ProjectToCamera(ptInsertVertex, ptInsertVertex);

	//	glm::vec2 ptConnectPoint1 = grabPointModelList[(m_pModel->GetActiveGrabPointIndex() + 1) % grabPointModelList.size()].GetPos();
	//	pCamera->ProjectToCamera(ptConnectPoint1, ptConnectPoint1);

	//	glm::vec2 ptConnectPoint2;
	//	if(m_pModel->GetActiveGrabPointIndex() == 0)
	//		ptConnectPoint2 = grabPointModelList[grabPointModelList.size() - 1].GetPos();
	//	else
	//		ptConnectPoint2 = grabPointModelList[m_pModel->GetActiveGrabPointIndex() - 1].GetPos();
	//	pCamera->ProjectToCamera(ptConnectPoint2, ptConnectPoint2);

	//	m_PreviewPrim.RemoveAllLayers();
	//	m_PreviewPrim.SetAsLineSegment(0, ptInsertVertex, ptConnectPoint1, 1.0f);
	//	m_PreviewPrim.SetAsLineSegment(1, ptInsertVertex, ptConnectPoint2, 1.0f);
	//	break;

	//case EDITMODEACTION_HoverGrabPoint:
	//	break;
	//case EDITMODEACTION_HoverCenter:
	//	break;
	//}

	SyncColor();
}

void EditModeView::ClearGrabPoints()
{
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		delete pGrabPtView;
	m_GrabPointViewList.clear();

	m_CenterGrabPoint.SetVisible(false);
}
