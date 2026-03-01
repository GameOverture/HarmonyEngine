/**************************************************************************
 *	GfxGrabPointView.cpp
 *
 *	Harmony Engine - Editor Tool
 *	Copyright (c) 2026 Jason Knobler
 *
 *	Harmony Editor Tool License:
 *	https://github.com/GameOverture/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Global.h"
#include "GfxGrabPointView.h"
#include "GfxGrabPointModel.h"

GfxGrabPointView::GfxGrabPointView(HyEntity2d *pParent) :
	HyEntity2d(pParent),
	m_GrabPt(this)
{
	UseWindowCoordinates(0);
	SetDisplayOrder(DISPLAYORDER_TransformCtrl);
}

/*virtual*/ GfxGrabPointView::~GfxGrabPointView()
{
}

void GfxGrabPointView::GetLocalBoundingShape(HyShape2d &shapeRefOut)
{
	m_GrabPt.CalcLocalBoundingShape(shapeRefOut);
}

void GfxGrabPointView::Sync(const GfxGrabPointModel *pModel)
{
	if(pModel == nullptr)
	{
		m_GrabPt.SetAsNothing(0);
		m_GrabPt.SetAsNothing(1);
		return;
	}
	
	m_GrabPt.SetAsCircle(0, pModel->GetRadius());
	m_GrabPt.SetLayerColor(0, pModel->GetOutlineColor());
	m_GrabPt.SetAsCircle(1, pModel->GetRadius() - 1.0f);
	m_GrabPt.SetLayerColor(1, pModel->GetFillColor());

	glm::vec2 ptPos = pModel->GetPos();
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	pCamera->ProjectToCamera(ptPos, ptPos);
	pos.Set(ptPos);
}

