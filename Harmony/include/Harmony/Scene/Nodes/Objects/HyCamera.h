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
