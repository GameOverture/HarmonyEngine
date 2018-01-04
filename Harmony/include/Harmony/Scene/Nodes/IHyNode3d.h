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
#include "Scene/Tweens/HyTweenVec3.h"

class IHyNode3d : public IHyNode
{
public:
	HyTweenVec3					pos;
	HyTweenVec3					rot;
	HyTweenVec3					rot_pivot;
	HyTweenVec3					scale;
	HyTweenVec3					scale_pivot;

public:
	IHyNode3d(HyType eNodeType, IHyNode3d *pParent);
	IHyNode3d(const IHyNode3d &copyRef);
	virtual ~IHyNode3d();

	const IHyNode3d &operator=(const IHyNode3d &rhs);

	void GetLocalTransform(glm::mat4 &outMtx) const;

protected:
	virtual void PhysicsUpdate() override;
};

#endif /* IHyNode3d_h__ */
