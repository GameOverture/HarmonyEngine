/**************************************************************************
*	IHyVisable3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyVisable3d_h__
#define IHyVisable3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Scene/Nodes/Loadables/Visables/IHyVisable.h"
#include "Scene/Tweens/HyTweenVec3.h"

class IHyVisable3d : public IHyLoadable3d, public IHyVisable
{
protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTint;
	
public:
	HyTweenVec3						tint;
	HyTweenFloat					alpha;

public:
	IHyVisable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent);
	IHyVisable3d(const IHyVisable3d &copyRef);
	virtual ~IHyVisable3d();

	const IHyVisable3d &operator=(const IHyVisable3d &rhs);

	void SetTint(float fR, float fG, float fB);
	void SetTint(uint32 uiColor);

	float CalculateAlpha();
	const glm::vec3 &CalculateTint();

protected:
	virtual void NodeUpdate() = 0;

private:
	void CalculateColor();

	virtual IHyNode &_VisableGetNodeRef() override final;
	virtual HyEntity2d *_VisableGetParent2dPtr() override final;
	virtual HyEntity3d *_VisableGetParent3dPtr() override final;
};

#endif /* IHyVisable3d_h__ */
