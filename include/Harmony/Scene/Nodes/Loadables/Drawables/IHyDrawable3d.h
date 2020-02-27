/**************************************************************************
*	IHyDrawable3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyDrawable3d_h__
#define IHyDrawable3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Scene/Nodes/Loadables/Drawables/IHyDrawable.h"
#include "Scene/AnimFloats/HyAnimVec3.h"

class IHyDrawable3d : public IHyLoadable3d, public IHyDrawable
{
	friend class HyEntity3d;

protected:
	float							m_fAlpha;
	float							m_fCachedAlpha;
	glm::vec3						m_CachedTint;
	
public:
	HyAnimVec3						tint;
	HyAnimFloat						alpha;

public:
	IHyDrawable3d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity3d *pParent);
	IHyDrawable3d(const IHyDrawable3d &copyRef);
	IHyDrawable3d(IHyDrawable3d &&donor);
	virtual ~IHyDrawable3d();

	IHyDrawable3d &operator=(const IHyDrawable3d &rhs);
	IHyDrawable3d &operator=(IHyDrawable3d &&donor);

	void SetTint(float fR, float fG, float fB);
	void SetTint(uint32 uiColor);

	float CalculateAlpha();
	const glm::vec3 &CalculateTint();

protected:
	virtual void Update() override;

private:
	void CalculateColor();

	virtual IHyNode &_VisableGetNodeRef() override final;
	virtual HyEntity2d *_VisableGetParent2dPtr() override final;
	virtual HyEntity3d *_VisableGetParent3dPtr() override final;

	friend void _CtorSetupNewChild(HyEntity3d &parentRef, IHyDrawable3d &childRef);
};

#endif /* IHyDrawable3d_h__ */
