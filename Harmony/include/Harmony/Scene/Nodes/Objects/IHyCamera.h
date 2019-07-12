/**************************************************************************
*	IHyCamera.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyCamera_h__
#define IHyCamera_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyMath.h"
#include "Utilities/HyRand.h"

class HyWindow;

template<typename NODETYPE>
class IHyCamera : public NODETYPE
{
	friend class HyScene;
	friend class IHyRenderer;

protected:
	HyWindow *			m_pWindowPtr;
	HyRectangle<float>	m_ViewportRect;	// Values are [0.0-1.0] representing percentages

	uint32				m_uiCullMaskBit;

	float				m_fCameraShakeRadius;
	float				m_fCameraShakeAngle;
	glm::vec3			m_ptCameraShakeCenter;

protected:
	IHyCamera(HyWindow *pWindow) :
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

	IHyCamera(const IHyCamera &copyRef) = delete;
	virtual ~IHyCamera() { }

public:
	HyWindow &GetWindow()						{ return *m_pWindowPtr; }
	const HyRectangle<float> &GetViewport()		{ return m_ViewportRect; }

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetViewport(float fPosX, float fPosY, float fWidth, float fHeight)
	{
		m_ViewportRect.left = fPosX;
		m_ViewportRect.bottom = fPosY;
		m_ViewportRect.right = m_ViewportRect.left + fWidth;
		m_ViewportRect.top = m_ViewportRect.bottom + fHeight;
	}

	bool IsCameraShake() {
		return m_fCameraShakeRadius != 0.0f;
	}

	void CameraShake(float fRadius)
	{
		if(IsCameraShake() == false)
			HyCopyVec(m_ptCameraShakeCenter, pos.Get());

		fRadius = HyMax(fRadius, 2.5f);

		m_fCameraShakeRadius = fRadius;
		m_fCameraShakeAngle = HyRand::Range(0.0f, 360.0f);
	}

	virtual void SetZoom(const float fZoom) = 0;
	virtual float GetZoom() const = 0;

protected:
	virtual void Update() override
	{
		NODETYPE::Update();

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

private:
	void SetCullMaskBit(uint32 uiBit) {
		m_uiCullMaskBit = uiBit;
	}

	uint32 GetCameraBitFlag() {
		return m_uiCullMaskBit;
	}
};

#endif /* IHyCamera_h__ */
