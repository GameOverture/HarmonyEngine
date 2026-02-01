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
#include "GfxGrabPointView.h"

IGfxEditView::IGfxEditView(HyEntity2d *pParent /*= nullptr*/) :
	HyEntity2d(pParent),
	m_pModel(nullptr)
{
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

void IGfxEditView::RefreshView(ShapeMouseMoveResult eResult, bool bMouseDown)
{
	if(m_pModel)
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
			m_GrabPointViewList[i]->Sync(&grabPointModelList[i]);
	}
	else
		ClearGrabPoints();

	DoRefreshView(eResult, bMouseDown);
	RefreshColor();
}

void IGfxEditView::ClearGrabPoints()
{
	for(GfxGrabPointView *pGrabPtView : m_GrabPointViewList)
		delete pGrabPtView;
	m_GrabPointViewList.clear();
}
