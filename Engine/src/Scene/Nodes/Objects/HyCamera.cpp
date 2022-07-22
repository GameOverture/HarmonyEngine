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


const b2AABB &HyCamera2d::GetWorldViewBounds()
{
	// TODO: Account for camera rotation!

	float fHalfWidth = ((m_pWindowPtr->GetFramebufferSize().x * m_ViewportRect.Width()) * 0.5f) * scale.X();
	float fHalfHeight = ((m_pWindowPtr->GetFramebufferSize().y * m_ViewportRect.Height()) * 0.5f) * scale.Y();

	m_aabbViewBounds.lowerBound.Set(pos.X() - fHalfWidth, pos.Y() - fHalfHeight);
	m_aabbViewBounds.upperBound.Set(pos.X() + fHalfWidth, pos.Y() + fHalfHeight);

	return m_aabbViewBounds;
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
