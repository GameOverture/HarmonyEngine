/**************************************************************************
*	IHyBody.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyBody_h__
#define IHyBody_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Renderer/Effects/HyStencil.h"
#include "Utilities/HyMath.h"

class IHyNode;
class HyEntity2d;
class HyEntity3d;

class IHyBody
{
protected:
	int32							m_iCoordinateSystem;	// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index
	HyStencilHandle					m_hStencil;

public:
	IHyBody();
	IHyBody(const IHyBody &copyRef);
	IHyBody(IHyBody &&donor);
	virtual ~IHyBody();

	IHyBody &operator=(const IHyBody &rhs);
	IHyBody &operator=(IHyBody &&donor);

	bool IsStencilSet() const;
	HyStencil *GetStencil() const;
	virtual void SetStencil(HyStencil *pStencil);
	virtual void ClearStencil(bool bUseParentStencil);

	int32 GetCoordinateSystem() const;
	virtual void UseCameraCoordinates();
	virtual void UseWindowCoordinates(int32 iWindowIndex = 0);

	// Synonym for UseCameraCoordinates() & UseWindowCoordinates()
	void SetCameraCoordinates();
	void SetWindowCoordinates(int32 iWindowIndex = 0);

	virtual float GetWidth(float fPercent = 1.0f) = 0;
	virtual float GetHeight(float fPercent = 1.0f) = 0;

protected:
	// Internal Entity propagation function overrides
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding);
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding);

private:
	virtual IHyNode &_VisableGetNodeRef() = 0;
	virtual HyEntity2d *_VisableGetParent2dPtr() = 0;
	virtual HyEntity3d *_VisableGetParent3dPtr() = 0;
};

#endif /* IHyBody_h__ */
