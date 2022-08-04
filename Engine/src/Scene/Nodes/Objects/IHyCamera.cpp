/**************************************************************************
*	IHyCamera.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Objects/IHyCamera.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyRand.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/IHyNode3d.h"

class HyWindow;
class HyScene;
class IHyRenderer;

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
/*virtual*/ IHyCamera<NODETYPE>::~IHyCamera()
{
}

template<typename NODETYPE>
HyWindow &IHyCamera<NODETYPE>::GetWindow()
{
	return *m_pWindowPtr;
}

template<typename NODETYPE>
const HyRectangle<float> &IHyCamera<NODETYPE>::GetViewport() const
{
	return m_ViewportRect;
}

// All values are [0.0 - 1.0] representing percentages of the entire window this camera belongs to
template<typename NODETYPE>
void IHyCamera<NODETYPE>::SetViewport(float fNormalizedPosX, float fNormalizedPosY, float fNormalizedWidth, float fNormalizedHeight)
{
	m_ViewportRect.left = fNormalizedPosX;
	m_ViewportRect.bottom = fNormalizedPosY;
	m_ViewportRect.right = m_ViewportRect.left + fNormalizedWidth;
	m_ViewportRect.top = m_ViewportRect.bottom + fNormalizedHeight;
}

template<typename NODETYPE>
bool IHyCamera<NODETYPE>::IsCameraShake()
{
	return m_fCameraShakeRadius > 0.0f;
}

template<typename NODETYPE>
void IHyCamera<NODETYPE>::CameraShake(float fRadius)
{
	if(IsCameraShake() == false)
		HyCopyVec(m_ptCameraShakeCenter, NODETYPE::pos.Get());

	fRadius = HyMax(fRadius, 2.5f);

	m_fCameraShakeRadius = fRadius;
	m_fCameraShakeAngle = HyRand::Range(0.0f, 360.0f);
}

template<typename NODETYPE>
void IHyCamera<NODETYPE>::SetZoomLevel(HyZoomLevel eZoomLevel)
{
	SetZoom(Hy_ZoomLevelList[eZoomLevel]);
}

// Takes the current zoom and sets it to the closest zoom level
template<typename NODETYPE>
HyZoomLevel IHyCamera<NODETYPE>::SetZoomLevel()
{
	float fCurZoom = GetZoom();

	auto iter_geq = std::lower_bound(Hy_ZoomLevelList.begin(), Hy_ZoomLevelList.end(), fCurZoom);
	if(iter_geq == Hy_ZoomLevelList.begin())
	{
		SetZoom(Hy_ZoomLevelList[0]);
		return static_cast<HyZoomLevel>(0);
	}

	float a = *(iter_geq - 1);
	float b = *(iter_geq);

	
	if(fabs(fCurZoom - a) < fabs(fCurZoom - b))
	{
		auto iZoomIndex = iter_geq - Hy_ZoomLevelList.begin() - 1;
		SetZoom(Hy_ZoomLevelList[iZoomIndex]);
		return static_cast<HyZoomLevel>(iZoomIndex);
	}

	auto iZoomIndex = iter_geq - Hy_ZoomLevelList.begin();
	SetZoom(Hy_ZoomLevelList[iZoomIndex]);
	return static_cast<HyZoomLevel>(iZoomIndex);
}

template<typename NODETYPE>
/*virtual*/ void IHyCamera<NODETYPE>::Update() /*override*/
{
	NODETYPE::Update();

	if(IsCameraShake())
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

template<typename NODETYPE>
void IHyCamera<NODETYPE>::SetCullMaskBit(uint32 uiBit)
{
	m_uiCullMaskBit = uiBit;
}

template<typename NODETYPE>
uint32 IHyCamera<NODETYPE>::GetCameraBitFlag()
{
	return m_uiCullMaskBit;
}

template class IHyCamera<IHyNode2d>;
template class IHyCamera<IHyNode3d>;
