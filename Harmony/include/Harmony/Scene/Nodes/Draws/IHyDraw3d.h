/**************************************************************************
*	IHyDraw3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNodeDraw3d_h__
#define IHyNodeDraw3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode3d.h"
#include "Scene/Tweens/HyTweenVec3.h"
#include "Utilities/HyMath.h"

class IHyDraw3d : public IHyNode3d
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
	IHyDraw3d(HyType eNodeType, HyEntity3d *pParent);
	IHyDraw3d(const IHyDraw3d &copyRef);
	virtual ~IHyDraw3d();

	const IHyDraw3d &operator=(const IHyDraw3d &rhs);

	bool IsScissorSet() const;
	void GetLocalScissor(HyScreenRect<int32> &scissorOut) const;
	void GetWorldScissor(HyScreenRect<int32> &scissorOut);

	bool IsStencilSet() const;
	HyStencil *GetStencil() const;

protected:
	virtual void NodeUpdate() = 0;
};

#endif /* IHyNodeDraw3d_h__ */
