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
#include "HyEngine.h"

HyCamera2d::HyCamera2d(HyWindow *pWindow) :
	IHyCamera<IHyNode2d>(pWindow),
	m_vCamVelocity(0.0f),
	m_fPanMaxSpeed(250.0f),
	m_fPanAccel(100.0f),
	m_fPanDecel(300.0f),
	m_uiPanFlags(0)
{ }

HyCamera2d::~HyCamera2d()
{ }

void HyCamera2d::PanUp()
{
	m_uiPanFlags |= PAN_UP;
}

void HyCamera2d::PanDown()
{
	m_uiPanFlags |= PAN_DOWN;
}

void HyCamera2d::PanLeft()
{
	m_uiPanFlags |= PAN_LEFT;
}

void HyCamera2d::PanRight()
{
	m_uiPanFlags |= PAN_RIGHT;
}

bool HyCamera2d::IsPanning() const
{
	return m_uiPanFlags || m_vCamVelocity.x != 0.0f || m_vCamVelocity.y != 0.0f;
}

/*virtual*/ float HyCamera2d::GetZoom() const
{
	return 1.0f / scale.Get().x;
}

/*virtual*/ void HyCamera2d::SetZoom(const float fZoom)
{
	// Inverse the value before storing it in scale. This is done because the renderer will glm::inverse(pCamera->GetSceneTransform()) to get the view matrix
	scale.Set(1.0f / fZoom);
}

// NOTE: Doesn't account for camera rotation!
void HyCamera2d::CalcWorldViewBounds(b2AABB &aabbOut) const
{
	float fHalfWidth = ((m_pWindowPtr->GetFramebufferSize().x * m_ViewportRect.Width()) * 0.5f) * scale.X();
	float fHalfHeight = ((m_pWindowPtr->GetFramebufferSize().y * m_ViewportRect.Height()) * 0.5f) * scale.Y();

	aabbOut.lowerBound.Set(pos.X() - fHalfWidth, pos.Y() - fHalfHeight);
	aabbOut.upperBound.Set(pos.X() + fHalfWidth, pos.Y() + fHalfHeight);
}

void HyCamera2d::ProjectToCamera(const glm::vec2 &ptWorldPos, glm::vec2 &ptCameraCoordinateOut) const
{
	b2AABB aabbWorld;
	CalcWorldViewBounds(aabbWorld);

	ptCameraCoordinateOut.x = (ptWorldPos.x - aabbWorld.lowerBound.x) * (1.0f / scale.X());
	ptCameraCoordinateOut.y = (ptWorldPos.y - aabbWorld.lowerBound.y) * (1.0f / scale.Y());
}

void HyCamera2d::ProjectToWorld(const glm::vec2 &ptCameraCoordinate, glm::vec2 &ptWorldPosOut) const
{
	b2AABB aabbWorld;
	CalcWorldViewBounds(aabbWorld);

	glm::vec2 ptLowerBound(aabbWorld.lowerBound.x, aabbWorld.lowerBound.y);
	ptWorldPosOut = ptLowerBound + (ptCameraCoordinate * scale.Get());
}

/*virtual*/ void HyCamera2d::Update() /*override*/
{
	IHyCamera<IHyNode2d>::Update();

	if(IsPanning())
	{
		if(m_uiPanFlags & PAN_LEFT)
			m_vCamVelocity.x -= m_fPanAccel * HyEngine::DeltaTime();
		else if(m_vCamVelocity.x < 0.0f)
			m_vCamVelocity.x = HyMath::Min(0.0f, m_vCamVelocity.x + (m_fPanDecel * HyEngine::DeltaTime()));
		if(m_uiPanFlags & PAN_RIGHT)
			m_vCamVelocity.x += m_fPanAccel * HyEngine::DeltaTime();
		else if(m_vCamVelocity.x > 0.0f)
			m_vCamVelocity.x = HyMath::Max(0.0f, m_vCamVelocity.x - (m_fPanDecel * HyEngine::DeltaTime()));
		if(m_uiPanFlags & PAN_UP)
			m_vCamVelocity.y += m_fPanAccel * HyEngine::DeltaTime();
		else if(m_vCamVelocity.y > 0.0f)
			m_vCamVelocity.y = HyMath::Max(0.0f, m_vCamVelocity.y - (m_fPanDecel * HyEngine::DeltaTime()));
		if(m_uiPanFlags & PAN_DOWN)
			m_vCamVelocity.y -= m_fPanAccel * HyEngine::DeltaTime();
		else if(m_vCamVelocity.y < 0.0f)
			m_vCamVelocity.y = HyMath::Min(0.0f, m_vCamVelocity.y + (m_fPanDecel * HyEngine::DeltaTime()));

		m_vCamVelocity.x = HyMath::Clamp(m_vCamVelocity.x, -m_fPanMaxSpeed, m_fPanMaxSpeed);
		m_vCamVelocity.y = HyMath::Clamp(m_vCamVelocity.y, -m_fPanMaxSpeed, m_fPanMaxSpeed);
		pos.Offset(m_vCamVelocity);

		m_uiPanFlags = 0;
	}
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
