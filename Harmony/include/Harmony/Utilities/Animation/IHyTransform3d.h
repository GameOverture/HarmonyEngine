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
protected:
	HyAnimVec3		rot;

public:
	IHyTransform3d();
	virtual ~IHyTransform3d();

	virtual void GetLocalTransform(glm::mat4 &outMtx) const;
	virtual void GetLocalTransform_SRT(glm::mat4 &outMtx) const;
	virtual void SetOnDirtyCallback(void(*fpOnDirty)(void *), void *pParam = NULL);

	virtual void OnUpdate() = 0;
};

#endif /* __IHyTransform3d_h__ */
