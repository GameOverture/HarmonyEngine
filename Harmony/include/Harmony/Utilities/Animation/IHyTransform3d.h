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

#include "Utilities\Animation\IHyTransform.h"

class IHyTransform3d : public IHyTransform<HyAnimVec3>
{
public:
	HyAnimVec3						rot;

	IHyTransform3d(HyType eInstType);
	virtual ~IHyTransform3d();

	virtual void GetLocalTransform(glm::mat4 &outMtx) const;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const;

	virtual void OnUpdate() = 0;
};

#endif /* __IHyTransform3d_h__ */
