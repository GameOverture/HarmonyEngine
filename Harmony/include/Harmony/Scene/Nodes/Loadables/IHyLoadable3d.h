/**************************************************************************
*	IHyLoadable3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLoadable3d_h__
#define IHyLoadable3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode3d.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Scene/Tweens/HyTweenVec3.h"
#include "Utilities/HyMath.h"

class IHyLoadable3d : public IHyNode3d, public IHyLoadable
{
protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTopColor;
	glm::vec3						m_CachedBotColor;

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
	int32							m_iDisplayOrder;		// Higher values are displayed front-most

public:
	HyTweenVec3						topColor;
	HyTweenVec3						botColor;
	HyTweenFloat					alpha;

public:
	IHyLoadable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent);
	IHyLoadable3d(const IHyLoadable3d &copyRef);
	virtual ~IHyLoadable3d();

	const IHyLoadable3d &operator=(const IHyLoadable3d &rhs);

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
	// Optional overrides for derived classes
	virtual void DrawLoadedUpdate() { }			// Invoked once after OnLoaded(), then once every frame (guarenteed to only be invoked if this instance is loaded)
	virtual void OnDataAcquired() { }			// Invoked once on the first time this node's data is queried
	virtual void OnLoaded() { }					// HyAssets invokes this once all required IHyLoadables are fully loaded for this node
	virtual void OnUnloaded() { }				// HyAssets invokes this instance's data has been erased

	virtual void NodeUpdate() = 0;

private:
	virtual HyType _LoadableGetType() override;
};

#endif /* IHyLoadable3d_h__ */
