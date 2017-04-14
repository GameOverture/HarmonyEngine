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

#include "Scene/Nodes/Transforms/IHyTransform.h"

class IHyTransform3d : public IHyTransform<HyTweenVec3>
{
public:
	HyTweenVec3						rot;

	IHyTransform3d(HyType eInstType, IHyNode *pParent = nullptr);
	virtual ~IHyTransform3d();

	virtual void GetLocalTransform(glm::mat4 &outMtx) const override;
};

#endif /* __IHyTransform3d_h__ */
