/**************************************************************************
*	IHyBody3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyBody3d_h__
#define IHyBody3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Scene/Nodes/Loadables/Bodies/IHyBody.h"
#include "Scene/AnimFloats/HyAnimVec3.h"

class IHyBody3d : public IHyLoadable3d, public IHyBody
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
	IHyBody3d(HyType eNodeType, std::string sPrefix, std::string sName, HyEntity3d *pParent);
	IHyBody3d(const IHyBody3d &copyRef);
	IHyBody3d(IHyBody3d &&donor) noexcept;
	virtual ~IHyBody3d();

	IHyBody3d &operator=(const IHyBody3d &rhs);
	IHyBody3d &operator=(IHyBody3d &&donor);

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
};

#endif /* IHyBody3d_h__ */
