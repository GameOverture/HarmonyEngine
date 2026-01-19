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
	m_GrabOutline(this),
	m_GrabFill(this)
{
	UseWindowCoordinates(0);
	SetDisplayOrder(DISPLAYORDER_TransformCtrl);
}

/*virtual*/ GfxGrabPointView::~GfxGrabPointView()
{
}

void GfxGrabPointView::GetLocalBoundingShape(HyShape2d &shapeRefOut)
{
	m_GrabOutline.CalcLocalBoundingShape(shapeRefOut);
}

void GfxGrabPointView::Sync(const GfxGrabPointModel *pModel)
{
	if(pModel == nullptr)
	{
		m_GrabOutline.SetAsNothing();
		m_GrabFill.SetAsNothing();
		return;
	}
	
	m_GrabOutline.SetAsCircle(pModel->GetRadius());
	m_GrabOutline.SetTint(pModel->GetOutlineColor());
	m_GrabFill.SetAsCircle(pModel->GetRadius() - 1.0f);
	m_GrabFill.SetTint(pModel->GetFillColor());

	glm::vec2 ptPos = pModel->GetPos();
	HyCamera2d *pCamera = HyEngine::Window().GetCamera2d(0);
	pCamera->ProjectToCamera(ptPos, ptPos);
	pos.Set(ptPos);
}

