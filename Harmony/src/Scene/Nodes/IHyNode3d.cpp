/**************************************************************************
*	IHyNode3d.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/IHyNode3d.h"

IHyNode3d::IHyNode3d(HyType eNodeType, IHyNode3d *pParent) :	IHyNode(eNodeType),
																pos(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																rot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																rot_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																scale(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
																scale_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB)
{
	scale.Set(1.0f);
}

IHyNode3d::IHyNode3d(const IHyNode3d &copyRef) :	IHyNode(copyRef),
													pos(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													rot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													rot_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													scale(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB),
													scale_pivot(*this, DIRTY_Transform | DIRTY_Scissor | DIRTY_WorldAABB)
{
	pos.Set(copyRef.pos.Get());
	rot.Set(copyRef.rot.Get());
	rot_pivot.Set(copyRef.rot_pivot.Get());
	scale.Set(copyRef.scale.Get());
	scale_pivot.Set(copyRef.scale_pivot.Get());
}

IHyNode3d::~IHyNode3d()
{
}

const IHyNode3d &IHyNode3d::operator=(const IHyNode3d &rhs)
{
	IHyNode::operator=(rhs);

	pos.Set(rhs.pos.Get());
	rot.Set(rhs.rot.Get());
	rot_pivot.Set(rhs.rot_pivot.Get());
	scale.Set(rhs.scale.Get());
	scale_pivot.Set(rhs.scale_pivot.Get());

	return *this;
}

void IHyNode3d::GetLocalTransform(glm::mat4 &outMtx) const
{
	outMtx = glm::mat4(1.0f);

	outMtx = glm::translate(outMtx, pos.Get());

	outMtx = glm::translate(outMtx, rot_pivot.Get());
	outMtx = glm::rotate(outMtx, rot.Get().x, glm::vec3(1, 0, 0));
	outMtx = glm::rotate(outMtx, rot.Get().y, glm::vec3(0, 1, 0));
	outMtx = glm::rotate(outMtx, rot.Get().z, glm::vec3(0, 0, 1));
	outMtx = glm::translate(outMtx, rot_pivot.Get() * -1.0f);

	outMtx = glm::translate(outMtx, scale_pivot.Get());
	outMtx = glm::scale(outMtx, scale.Get());
	outMtx = glm::translate(outMtx, scale_pivot.Get() * -1.0f);
}

/*virtual*/ void IHyNode3d::PhysicsUpdate() /*override*/
{
}
