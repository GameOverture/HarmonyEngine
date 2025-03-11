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
#include "HyEngine.h"

class HyWindow;
class HyScene;
class IHyRenderer;

template<typename NODETYPE>
IHyCamera<NODETYPE>::IHyCamera(HyWindow *pWindow) :
	NODETYPE(HYTYPE_Camera, nullptr),
	m_pWindowPtr(pWindow),
	m_uiCullMaskBit(0),
	m_ptCameraShakeCenter(0.0f),
	m_fCameraShakeAngle(0.0f),
	m_fCameraShakeIntensity(0.0f),
	m_fCameraShakeDuration(0.0f),
	m_fCameraShakeElapsedTime(0.0f)
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
const HyUvCoord &IHyCamera<NODETYPE>::GetViewport() const
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
	return m_fCameraShakeDuration > 0.0f;
}

template<typename NODETYPE>
void IHyCamera<NODETYPE>::CameraShake(float fIntensity, float fDuration)
{
	if(IsCameraShake() == false)
		HySetVec(m_ptCameraShakeCenter, NODETYPE::pos.Get());

	m_fCameraShakeAngle = HyRand::Range(0.0f, 359.0f);
	m_fCameraShakeIntensity = fIntensity;
	m_fCameraShakeDuration = fDuration;
	m_fCameraShakeElapsedTime = 0.0f;
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

//template<typename NODETYPE>
///*virtual*/ void IHyCamera<NODETYPE>::SetDirty(uint32 uiDirtyFlags) /*override*/
//{
//	NODETYPE::SetDirty(uiDirtyFlags);
//
//	if((uiDirtyFlags & DIRTY_Transform) != 0 && IsCameraShake())
//		HySetVec(m_ptCameraShakeCenter, NODETYPE::pos.Get());
//}

template<typename NODETYPE>
/*virtual*/ void IHyCamera<NODETYPE>::Update() /*override*/
{
	NODETYPE::Update();

	if(IsCameraShake())
	{
		m_fCameraShakeElapsedTime += HyEngine::DeltaTime();
		if(m_fCameraShakeElapsedTime >= m_fCameraShakeDuration)
		{
			// End camera shake
			NODETYPE::pos.Set(m_ptCameraShakeCenter);
			m_fCameraShakeAngle = 0.0f;
			m_fCameraShakeIntensity = 0.0f;
			m_fCameraShakeDuration = 0.0f;
			m_fCameraShakeElapsedTime = 0.0f;
		}
		else
		{
			// TODO: Implement different shake types: Perlin Noise, Sin (sinusoidal) Wave, etc.
			float fCurIntensity = m_fCameraShakeIntensity * (1.0f - (m_fCameraShakeElapsedTime / m_fCameraShakeDuration));
			m_fCameraShakeAngle += (180.0f - m_fCameraShakeAngle) + HyRand::Range(0.0f, 60.0f); // Adjust angle

			// Create offset 2d vector
			glm::vec3 vOffset(sin(m_fCameraShakeAngle) * fCurIntensity,
							  cos(m_fCameraShakeAngle) * fCurIntensity, 0.0f);
			
			NODETYPE::pos.Set(m_ptCameraShakeCenter + vOffset);
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
