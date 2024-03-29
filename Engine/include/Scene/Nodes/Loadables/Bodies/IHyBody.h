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
public:
	enum ScissorTag
	{
		SCISSORTAG_Disabled = 0,
		SCISSORTAG_Enabled,
	};

protected:
	struct ScissorRect
	{
		HyScreenRect<int32>			m_LocalScissorRect;
		HyScreenRect<int32>			m_WorldScissorRect;
	};
	ScissorRect *					m_pScissor;
	HyStencilHandle					m_hStencil;

	int32							m_iCoordinateSystem;	// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index

public:
	IHyBody();
	IHyBody(const IHyBody &copyRef);
	IHyBody(IHyBody &&donor);
	virtual ~IHyBody();

	IHyBody &operator=(const IHyBody &rhs);
	IHyBody &operator=(IHyBody &&donor);

	bool IsScissorSet() const;
	void GetLocalScissor(HyScreenRect<int32> &scissorOut) const;
	void GetWorldScissor(HyScreenRect<int32> &scissorOut, float fExtrapolatePercent);
	virtual void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	virtual void ClearScissor(bool bUseParentScissor);

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
	virtual void _SetScissor(const ScissorRect *pParentScissor, bool bIsOverriding);
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding);
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding);

private:
	virtual IHyNode &_VisableGetNodeRef() = 0;
	virtual HyEntity2d *_VisableGetParent2dPtr() = 0;
	virtual HyEntity3d *_VisableGetParent3dPtr() = 0;
};

#endif /* IHyBody_h__ */
