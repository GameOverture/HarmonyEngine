/**************************************************************************
*	IHyTransform2d.cpp
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Scene/Nodes/Transforms/HyTransform3d.h"

HyTransform3d::HyTransform3d(HyType eInstType, IHyNode *pParent /*= nullptr*/) :	IHyNode(eInstType, pParent),
																					pos(*this),
																					rot(*this),
																					rot_pivot(*this),
																					scale(*this),
																					scale_pivot(*this)
{
	scale.Set(1.0f);
}

HyTransform3d::~HyTransform3d()
{
}

void HyTransform3d::GetLocalTransform(glm::mat4 &outMtx) const
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
