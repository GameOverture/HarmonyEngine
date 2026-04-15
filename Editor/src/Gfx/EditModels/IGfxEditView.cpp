/**************************************************************************
 *	IGfxEditView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "IGfxEditView.h"
#include "IGfxEditModel.h"

IGfxEditView::IGfxEditView(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_pModel(nullptr),
	m_CenterGrabPoint(this)
{
	m_DataPrim.UseWindowCoordinates();

	m_PreviewPrim.UseWindowCoordinates();
	m_PreviewPrim.SetDisplayOrder(DISPLAYORDER_TransformCtrl - 2);
}

/*virtual*/ IGfxEditView::~IGfxEditView()
{
	ClearGrabPoints();

	if(m_pModel)
		m_pModel->RemoveView(this);
}

IGfxEditModel *IGfxEditView::GetModel() const
{
	return m_pModel;
}

void IGfxEditView::SetModel(IGfxEditModel *pModel)
{
	if(pModel == nullptr)
	{
		m_pModel = nullptr;
		m_pModel->RemoveView(this);
		return;
	}

	m_pModel = pModel;
	m_pModel->AddView(this);
}

void IGfxEditView::SyncColor()
{
	if(m_pModel)
		m_DataPrim.SetTint(m_pModel->GetColor());
}

void IGfxEditView::SyncWithModel(EditModeState eEditModeState, EditModeAction eEditModeAction)
{
	if(eEditModeState != EDITMODE_Off && m_pModel)
	{
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
		{
			m_GrabPointViewList[i]->Sync(&grabPointModelList[i]);
			m_GrabPointViewList[i]->SetVisible(eEditModeState != EDITMODE_Off);
		}

		m_CenterGrabPoint.Sync(&m_pModel->GetCenterGrabPoint());
		m_CenterGrabPoint.SetVisible(eEditModeState != EDITMODE_Off);



		HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
		if(eEditModeAction == EDITMODEACTION_AppendVertex)
		{
			glm::vec2 ptEndPoint;
			if(grabPointModelList.front().IsSelected())
				ptEndPoint = grabPointModelList.front().GetPos();
			else
				ptEndPoint = grabPointModelList.back().GetPos();
			
			glm::vec2 ptGrabPtPos = m_pModel->GetActiveGrabPoint()->GetPos();
			pCamera->ProjectToCamera(ptGrabPtPos, ptGrabPtPos);
			pCamera->ProjectToCamera(ptEndPoint, ptEndPoint);
			
			m_PreviewPrim.RemoveAllLayers();
			m_PreviewPrim.SetAsLineSegment(0, ptGrabPtPos, ptEndPoint, 1.0f);
		}
		else if(eEditModeAction == EDITMODEACTION_InsertVertex)
		{
			if(grabPointModelList.size() < 2)
				HyGuiLog("GfxChainView::RefreshView called with less than 2 grab points", LOGTYPE_Error);

			glm::vec2 ptInsertVertex = m_pModel->GetActiveGrabPoint()->GetPos();
			ptInsertVertex += m_pModel->GetDragDelta();
			pCamera->ProjectToCamera(ptInsertVertex, ptInsertVertex);

			glm::vec2 ptConnectPoint1 = grabPointModelList[(m_pModel->GetActiveGrabPointIndex() + 1) % grabPointModelList.size()].GetPos();
			pCamera->ProjectToCamera(ptConnectPoint1, ptConnectPoint1);

			glm::vec2 ptConnectPoint2;
			if(m_pModel->GetActiveGrabPointIndex() == 0)
				ptConnectPoint2 = grabPointModelList[grabPointModelList.size() - 1].GetPos();
			else
				ptConnectPoint2 = grabPointModelList[m_pModel->GetActiveGrabPointIndex() - 1].GetPos();
			pCamera->ProjectToCamera(ptConnectPoint2, ptConnectPoint2);

			m_PreviewPrim.RemoveAllLayers();
			m_PreviewPrim.SetAsLineSegment(0, ptInsertVertex, ptConnectPoint1, 1.0f);
			m_PreviewPrim.SetAsLineSegment(1, ptInsertVertex, ptConnectPoint2, 1.0f);
		}

		OnSyncModel(eEditModeState, eEditModeAction);
		SyncColor();
	}
	else
	{
		ClearGrabPoints();
		m_PreviewPrim.RemoveAllLayers();
	}
}

void IGfxEditView::ClearGrabPoints()
{
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		delete pGrabPtView;
	m_GrabPointViewList.clear();

	m_CenterGrabPoint.SetVisible(false);
}
