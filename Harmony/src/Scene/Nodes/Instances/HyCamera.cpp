/**************************************************************************
 *	HyCamera.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Instances/HyCamera.h"
#include "Renderer/Components/HyWindow.h"

IHyCamera::IHyCamera(HyWindow *pWindow) :	m_pWindowPtr(pWindow),
											m_uiCullMaskBit(0)
{
	m_ViewportRect.left = m_ViewportRect.bottom = 0.0f;
	m_ViewportRect.right = m_ViewportRect.top = 1.0f;
}

IHyCamera::~IHyCamera()
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
void IHyCamera::SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_ViewportRect.left = fPosX;
	m_ViewportRect.bottom = fPosY;
	m_ViewportRect.right = m_ViewportRect.left + fWidth;
	m_ViewportRect.top = m_ViewportRect.bottom + fHeight;
}

void IHyCamera::SetCullMaskBit(uint32 uiBit)
{
	m_uiCullMaskBit = uiBit;
}

uint32 IHyCamera::GetCullMaskBit()
{
	return m_uiCullMaskBit;
}

HyCamera2d::HyCamera2d(HyWindow *pWindow) : IHyInst2d(HYTYPE_Camera2d, nullptr),
											IHyCamera(pWindow)
{ }

HyCamera2d::~HyCamera2d()
{ }

/*virtual*/ void HyCamera2d::SetZoom(const float fZoom)
{
	scale.Set(fZoom, fZoom);
}

/*virtual*/ float HyCamera2d::GetZoom() const
{
	return scale.Get().x;
}

const b2AABB &HyCamera2d::GetWorldViewBounds()
{
	float fHalfWidth = ((m_pWindowPtr->GetFramebufferSize().x * m_ViewportRect.Width()) * 0.5f) * (1.0f / scale.X());
	float fHalfHeight = ((m_pWindowPtr->GetFramebufferSize().y * m_ViewportRect.Height()) * 0.5f) * (1.0f / scale.Y());

	m_aabbViewBounds.lowerBound.Set(pos.X() - fHalfWidth, pos.Y() - fHalfHeight);
	m_aabbViewBounds.upperBound.Set(pos.X() + fHalfWidth, pos.Y() + fHalfHeight);

	return m_aabbViewBounds;
}

//// NOTE: Does not properly calculate camera twist - must be axis aligned
//HyRectangle<float> HyCamera2d::GetWorldViewBounds()
//{
//	HyRectangle<float> returnRect;
//
//	float fHalfWidth = ((m_pWindowPtr->GetFramebufferSize().x * m_ViewportRect.Width()) * 0.5f) * (1.0f / scale.X());
//	float fHalfHeight = ((m_pWindowPtr->GetFramebufferSize().y * m_ViewportRect.Height()) * 0.5f) * (1.0f / scale.Y());
//
//	// TODO: 'pos' may not represent the world coordinates if it is attached to a parent transform hierarchy - GetWorldTransform() should be used first
//	returnRect.left = pos.X() - fHalfWidth;
//	returnRect.bottom = pos.Y() - fHalfHeight;
//	returnRect.right = pos.X() + fHalfWidth;
//	returnRect.top = pos.Y() + fHalfHeight;
//
//	return returnRect;
//}

/*virtual*/ void HyCamera2d::NodeUpdate()
{
}

HyCamera3d::HyCamera3d(HyWindow *pWindow) : IHyNode3d(HYTYPE_Camera3d, nullptr),
											IHyCamera(pWindow)
{ }

HyCamera3d::~HyCamera3d()
{ }

/*virtual*/ void HyCamera3d::SetZoom(const float fZoom)
{
	scale.Set(1.0f, 1.0f, fZoom);
}

/*virtual*/ float HyCamera3d::GetZoom() const
{
	return scale.Get().z;
}

/*virtual*/ void HyCamera3d::NodeUpdate()
{
}
