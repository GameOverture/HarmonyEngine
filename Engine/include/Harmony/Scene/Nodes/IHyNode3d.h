/**************************************************************************
*	IHyNode3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyNode3d_h__
#define IHyNode3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode.h"
#include "Scene/AnimFloats/HyAnimVec3.h"

class HyEntity3d;
class HyStencil;

class IHyNode3d : public IHyNode
{
	friend class HyEntity3d;

protected:
	HyEntity3d *				m_pParent;
	glm::mat4					m_mtxCached;

public:
	HyAnimVec3					pos;
	HyAnimVec3					rot;
	HyAnimVec3					rot_pivot;
	HyAnimVec3					scale;
	HyAnimVec3					scale_pivot;

public:
	IHyNode3d(HyType eNodeType, HyEntity3d *pParent);
	IHyNode3d(const IHyNode3d &copyRef);
	IHyNode3d(IHyNode3d &&donor);
	virtual ~IHyNode3d();

	IHyNode3d &operator=(const IHyNode3d &rhs);
	IHyNode3d &operator=(IHyNode3d &&donor);

	void ParentDetach();
	HyEntity3d *ParentGet() const;

	void GetLocalTransform(glm::mat4 &outMtx) const;
	const glm::mat4 &GetWorldTransform();

protected:
	virtual void Update() override;

	friend void _CtorSetupNewChild(HyEntity3d &parentRef, IHyNode3d &childRef);
};

#endif /* IHyNode3d_h__ */
