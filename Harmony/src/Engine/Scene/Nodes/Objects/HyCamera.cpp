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
#include "Utilities/HyRand.h"

IHyCamera::IHyCamera(HyWindow *pWindow) :
	m_pWindowPtr(pWindow),
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

uint32 IHyCamera::GetCameraBitFlag()
{
	return m_uiCullMaskBit;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyCamera2d::HyCamera2d(HyWindow *pWindow) :
	IHyNode2d(HYTYPE_Camera, nullptr),
	IHyCamera(pWindow),
	m_fCameraShakeRadius(0.0f),
	m_fCameraShakeAngle(0.0f),
	m_ptCameraShakeCenter(0.0f)
{ }

HyCamera2d::~HyCamera2d()
{ }

/*virtual*/ void HyCamera2d::GetCameraTransform(glm::mat4 &outMtx) /*override*/
{
	outMtx = GetWorldTransform();
}

/*virtual*/ void HyCamera2d::SetZoom(const float fZoom)
{
	scale.Set(fZoom, fZoom);
}

/*virtual*/ float HyCamera2d::GetZoom() const
{
	return scale.Get().x;
}

void HyCamera2d::CameraShake(float fRadius)
{
	fRadius = HyMax(fRadius, 2.5f);

	m_fCameraShakeRadius = fRadius;
	m_fCameraShakeAngle = HyRand::Range(0.0f, 360.0f);
	m_ptCameraShakeCenter = pos.Get();
}

const b2AABB &HyCamera2d::GetWorldViewBounds()
{
	float fHalfWidth = ((m_pWindowPtr->GetFramebufferSize().x * m_ViewportRect.Width()) * 0.5f) * (1.0f / scale.X());
	float fHalfHeight = ((m_pWindowPtr->GetFramebufferSize().y * m_ViewportRect.Height()) * 0.5f) * (1.0f / scale.Y());

	m_aabbViewBounds.lowerBound.Set(pos.X() - fHalfWidth, pos.Y() - fHalfHeight);
	m_aabbViewBounds.upperBound.Set(pos.X() + fHalfWidth, pos.Y() + fHalfHeight);

	return m_aabbViewBounds;
}

/*virtual*/ void HyCamera2d::Update() /*override*/
{
	IHyNode2d::Update();
	
	if(m_fCameraShakeRadius > 0.0f)
	{
		m_fCameraShakeRadius *= 0.9f;
		if(m_fCameraShakeRadius <= 2.0f)
		{
			// End camera shake
			m_fCameraShakeRadius = 0.0f;
			pos.Set(m_ptCameraShakeCenter);
		}
		else
		{
			m_fCameraShakeAngle += (180.0f - m_fCameraShakeRadius) + HyRand::Range(0.0f, 60.0f); // Adjust angle 
			glm::vec2 vOffset(sin(m_fCameraShakeAngle) * m_fCameraShakeRadius , cos(m_fCameraShakeAngle) * m_fCameraShakeRadius); // Create offset 2d vector
			pos.Set(m_ptCameraShakeCenter + vOffset); // Set center of viewport
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyCamera3d::HyCamera3d(HyWindow *pWindow) :
	IHyNode3d(HYTYPE_Camera, nullptr),
	IHyCamera(pWindow),
	m_fCameraShakeRadius(0.0f),
	m_fCameraShakeAngle(0.0f),
	m_ptCameraShakeCenter(0.0f)
{ }

HyCamera3d::~HyCamera3d()
{ }

/*virtual*/ void HyCamera3d::GetCameraTransform(glm::mat4 &outMtx) /*override*/
{
	outMtx = GetWorldTransform();
}

/*virtual*/ void HyCamera3d::SetZoom(const float fZoom)
{
	scale.Set(1.0f, 1.0f, fZoom);
}

/*virtual*/ float HyCamera3d::GetZoom() const
{
	return scale.Get().z;
}

/*virtual*/ void HyCamera3d::CameraShake(float fRadius) /*override*/
{
	fRadius = HyMax(fRadius, 2.5f);

	m_fCameraShakeRadius = fRadius;
	m_fCameraShakeAngle = HyRand::Range(0.0f, 360.0f);
	m_ptCameraShakeCenter = pos.Get();
}

/*virtual*/ void HyCamera3d::Update() /*override*/
{
	IHyNode3d::Update();

	if(m_fCameraShakeRadius > 0.0f)
	{
		m_fCameraShakeRadius *= 0.9f;
		if(m_fCameraShakeRadius <= 2.0f)
		{
			// End camera shake
			m_fCameraShakeRadius = 0.0f;
			pos.Set(m_ptCameraShakeCenter);
		}
		else
		{
			m_fCameraShakeAngle += (180.0f - m_fCameraShakeRadius) + HyRand::Range(0.0f, 60.0f); // Adjust angle 
			glm::vec3 vOffset(sin(m_fCameraShakeAngle) * m_fCameraShakeRadius , cos(m_fCameraShakeAngle) * m_fCameraShakeRadius, 0.0f); // Create offset 2d vector
			pos.Set(m_ptCameraShakeCenter + vOffset); // Set center of viewport
		}
	}
}