/**************************************************************************
 *	HyCamera.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyCamera_h__
#define HyCamera_h__

#include "Afx/HyStdAfx.h"

#include "Utilities/HyMath.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/IHyNode3d.h"

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
	IHyCamera(HyWindow *pWindow);
	IHyCamera(const IHyCamera &copyRef) = delete;
	virtual ~IHyCamera();

	const IHyNode2d &operator=(const IHyNode2d &rhs) = delete;

public:
	HyWindow &GetWindow()						{ return *m_pWindowPtr; }
	const HyRectangle<float> &GetViewport()		{ return m_ViewportRect; }

	// All values are [0.0 - 1.0] representing percentages of the entire game window
	void SetViewport(float fPosX, float fPosY, float fWidth, float fHeight);

	bool IsCameraShake();
	void CameraShake(float fRadius);

	virtual void SetZoom(const float fZoom) = 0;
	virtual float GetZoom() const = 0;

protected:
	virtual void Update() override;

private:
	void SetCullMaskBit(uint32 uiBit)
	{
		m_uiCullMaskBit = uiBit;
	}

	uint32 GetCameraBitFlag()
	{
		return m_uiCullMaskBit;
	}
};

class HyCamera2d final : public IHyCamera<IHyNode2d>
{
	friend class HyWindow;

	b2AABB		m_aabbViewBounds;

private:
	HyCamera2d(HyWindow *pWindow);
	virtual ~HyCamera2d();
	
public:
	virtual void SetZoom(const float fZoom) override;
	virtual float GetZoom() const override;

	const b2AABB &GetWorldViewBounds();
};

class HyCamera3d final : public IHyCamera<IHyNode3d>
{
	friend class HyWindow;

protected:
	HyCamera3d(HyWindow *pWindow);
	virtual ~HyCamera3d();

public:
	virtual void SetZoom(const float fZoom) override;
	virtual float GetZoom() const override;;
};

#endif /* HyCamera_h__ */
