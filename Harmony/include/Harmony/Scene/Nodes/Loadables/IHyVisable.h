/**************************************************************************
*	IHyVisable.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyVisable_h__
#define IHyVisable_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Renderer/Effects/HyStencil.h"
#include "Utilities/HyMath.h"

class IHyNode;

class IHyVisable
{
protected:
	enum ScissorTag
	{
		SCISSORTAG_Disabled = 0,
		SCISSORTAG_Enabled,
	};
	struct ScissorRect
	{
		HyScreenRect<int32>			m_LocalScissorRect;
		HyScreenRect<int32>			m_WorldScissorRect;
	};
	ScissorRect *					m_pScissor;
	HyStencilHandle					m_hStencil;

	int32							m_iCoordinateSystem;	// -1 (or any negative value) means using world/camera coordinates. Otherwise it represents the Window index

public:
	IHyVisable();
	IHyVisable(const IHyVisable &copyRef);
	virtual ~IHyVisable();

	const IHyVisable &operator=(const IHyVisable &rhs);

	bool IsScissorSet() const;
	void GetLocalScissor(HyScreenRect<int32> &scissorOut) const;
	void GetWorldScissor(HyScreenRect<int32> &scissorOut);
	virtual void SetScissor(int32 uiLocalX, int32 uiLocalY, uint32 uiWidth, uint32 uiHeight);
	virtual void ClearScissor(bool bUseParentScissor);

	bool IsStencilSet() const;
	HyStencil *GetStencil() const;
	virtual void SetStencil(HyStencil *pStencil);
	virtual void ClearStencil(bool bUseParentStencil);

	int32 GetCoordinateSystem() const;
	virtual void UseCameraCoordinates();
	virtual void UseWindowCoordinates(int32 iWindowIndex = 0);

protected:
	virtual void NodeUpdate() = 0;

	// Internal Entity propagation function overrides
	virtual void _SetScissor(const HyScreenRect<int32> &worldScissorRectRef, bool bIsOverriding);
	virtual void _SetStencil(HyStencilHandle hHandle, bool bIsOverriding);
	virtual void _SetCoordinateSystem(int32 iWindowIndex, bool bIsOverriding);

private:
	virtual IHyNode &_VisableGetNodeRef() = 0;
};

#endif /* IHyVisable_h__ */
