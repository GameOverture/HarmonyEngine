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

#include "Scene/Nodes/Objects/IHyCamera.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/IHyNode3d.h"
#include "Utilities/HyLocomotion.h"

class HyCamera2d final : public IHyCamera<IHyNode2d>
{
	friend class HyWindow;

	HyLocomotion2d		m_PanLocomotion;

private:
	HyCamera2d(HyWindow *pWindow);
	virtual ~HyCamera2d();
	
public:
	void PanUp();
	void PanDown();
	void PanLeft();
	void PanRight();
	bool IsPanning() const;

	virtual float GetZoom() const override;
	virtual void SetZoom(const float fZoom) override;

	void CalcWorldViewBounds(b2AABB &aabbOut) const; // NOTE: Doesn't account for camera rotation!
	void ProjectToCamera(const glm::vec2 &ptWorldPos, glm::vec2 &ptCameraCoordinateOut) const; // 'ptCameraCoordinateOut' will be the bottom left of the camera's viewport in the window. If the camera is using 100% of the window, then 'ptCameraCoordinateOut' is essentially the window coordinates
	void ProjectToWorld(const glm::vec2 &ptCameraCoordinate, glm::vec2 &ptWorldPosOut) const; // 'ptCameraCoordinate' is the bottom left of the camera's viewport in the window. If the camera is using 100% of the window, then 'ptCameraCoordinate' is essentially the window coordinates

protected:
	virtual void Update() override;
};

class HyCamera3d final : public IHyCamera<IHyNode3d>
{
	friend class HyWindow;

	// TODO: Add near and far plane

protected:
	HyCamera3d(HyWindow *pWindow);
	virtual ~HyCamera3d();

public:
	virtual float GetZoom() const override;
	virtual void SetZoom(const float fZoom) override;
};

#endif /* HyCamera_h__ */
