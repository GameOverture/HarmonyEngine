/**************************************************************************
*	IHyTransform3d.h
*
*	Harmony Engine
*	Copyright (c) 2014 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyTransform3d_h__
#define __IHyTransform3d_h__

#include "Scene/Nodes/IHyNode.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenVec3.h"

class HyTransform3d : public IHyNode
{
public:
	HyTweenVec3					pos;
	HyTweenVec3					rot;
	HyTweenVec3					rot_pivot;
	HyTweenVec3					scale;
	HyTweenVec3					scale_pivot;

	HyTransform3d(HyType eInstType, IHyNode *pParent = nullptr);
	virtual ~HyTransform3d();

	void GetLocalTransform(glm::mat4 &outMtx) const;
};

#endif /* __IHyTransform3d_h__ */
