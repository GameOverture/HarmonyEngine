/**************************************************************************
 *	HyCamera.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Objects/HyCamera.h"
#include "Window/HyWindow.h"

HyCamera2d::HyCamera2d(HyWindow *pWindow) :
	IHyCamera<IHyNode2d>(pWindow)
{ }

HyCamera2d::~HyCamera2d()
{ }

/*virtual*/ float HyCamera2d::GetZoom() const
{
	return 1.0f / scale.Get().x;
}

/*virtual*/ void HyCamera2d::SetZoom(const float fZoom)
{
	// Inverse the value before storing it in scale. This is done because the renderer will glm::inverse(pCamera->GetSceneTransform()) to get the view matrix
	scale.Set(1.0f / fZoom);
}

void HyCamera2d::CalcWorldViewBounds(b2AABB &aabbOut) const
{
	// TODO: Account for camera rotation!

	float fHalfWidth = ((m_pWindowPtr->GetFramebufferSize().x * m_ViewportRect.Width()) * 0.5f) * scale.X();
	float fHalfHeight = ((m_pWindowPtr->GetFramebufferSize().y * m_ViewportRect.Height()) * 0.5f) * scale.Y();

	aabbOut.lowerBound.Set(pos.X() - fHalfWidth, pos.Y() - fHalfHeight);
	aabbOut.upperBound.Set(pos.X() + fHalfWidth, pos.Y() + fHalfHeight);
}

void HyCamera2d::ProjectToCamera(const glm::vec2 &ptWorldPos, glm::vec2 &ptWindowCoordinateOut) const
{
	b2AABB aabbWorld;
	CalcWorldViewBounds(aabbWorld);

	float fFbWidth = (m_ViewportRect.Width() * m_pWindowPtr->GetFramebufferSize().x);
	float fFbHeight = (m_ViewportRect.Height() * m_pWindowPtr->GetFramebufferSize().y);

	glm::vec2 ptNormalized;
	ptNormalized.x = (ptWorldPos.x - aabbWorld.lowerBound.x) / (aabbWorld.GetExtents().x * 2.0f);
	ptNormalized.y = (ptWorldPos.y - aabbWorld.lowerBound.y) / (aabbWorld.GetExtents().y * 2.0f);

	ptWindowCoordinateOut.x = (ptNormalized.x * fFbWidth);
	ptWindowCoordinateOut.y = (ptNormalized.y * fFbHeight);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyCamera3d::HyCamera3d(HyWindow *pWindow) :
	IHyCamera<IHyNode3d>(pWindow)
{ }

HyCamera3d::~HyCamera3d()
{ }

/*virtual*/ float HyCamera3d::GetZoom() const
{
	return scale.Get().z;
}

/*virtual*/ void HyCamera3d::SetZoom(const float fZoom)
{
	// Inverse the value before storing it in scale. This is done because the renderer will glm::inverse(pCamera->GetSceneTransform()) to get the view matrix
	scale.SetZ(1.0f / fZoom);
}
