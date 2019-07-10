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

template<typename NODETYPE>
IHyCamera<NODETYPE>::IHyCamera(HyWindow *pWindow) :
	NODETYPE(HYTYPE_Camera, nullptr),
	m_pWindowPtr(pWindow),
	m_uiCullMaskBit(0),
	m_fCameraShakeRadius(0.0f),
	m_fCameraShakeAngle(0.0f),
	m_ptCameraShakeCenter(0.0f)
{
	m_ViewportRect.left = m_ViewportRect.bottom = 0.0f;
	m_ViewportRect.right = m_ViewportRect.top = 1.0f;
}

template<typename NODETYPE>
IHyCamera<NODETYPE>::~IHyCamera()
{
}

// All values are [0.0 - 1.0] representing percentages of the entire game window
template<typename NODETYPE>
void IHyCamera<NODETYPE>::SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
{
	m_ViewportRect.left = fPosX;
	m_ViewportRect.bottom = fPosY;
	m_ViewportRect.right = m_ViewportRect.left + fWidth;
	m_ViewportRect.top = m_ViewportRect.bottom + fHeight;
}

template<typename NODETYPE>
bool IHyCamera<NODETYPE>::IsCameraShake()
{
	return m_fCameraShakeRadius != 0.0f;
}

template<typename NODETYPE>
void IHyCamera<NODETYPE>::CameraShake(float fRadius)
{
	if(IsCameraShake())
		m_ptCameraShakeCenter = pos.Get();

	fRadius = HyMax(fRadius, 2.5f);

	m_fCameraShakeRadius = fRadius;
	m_fCameraShakeAngle = HyRand::Range(0.0f, 360.0f);
}

template<typename NODETYPE>
/*virtual*/ void IHyCamera<NODETYPE>::Update() /*override*/
{
	NODETYPE::Update();

	if(m_fCameraShakeRadius > 0.0f)
	{
		m_fCameraShakeRadius *= 0.9f;
		if(m_fCameraShakeRadius <= 2.0f)
		{
			// End camera shake
			m_fCameraShakeRadius = 0.0f;
			NODETYPE::pos.Set(m_ptCameraShakeCenter);
		}
		else
		{
			m_fCameraShakeAngle += (180.0f - m_fCameraShakeRadius) + HyRand::Range(0.0f, 60.0f); // Adjust angle 
			glm::vec3 vOffset(sin(m_fCameraShakeAngle) * m_fCameraShakeRadius , cos(m_fCameraShakeAngle) * m_fCameraShakeRadius, 0.0f); // Create offset 2d vector
			NODETYPE::pos.Set(m_ptCameraShakeCenter + vOffset); // Set center of viewport
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyCamera2d::HyCamera2d(HyWindow *pWindow) :
	IHyCamera<IHyNode2d>(pWindow)
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyCamera3d::HyCamera3d(HyWindow *pWindow) :
	IHyCamera<IHyNode3d>(pWindow)
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
