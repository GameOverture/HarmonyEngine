/**************************************************************************
*	IHyNode3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyNode3d_h__
#define __IHyNode3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Transforms/Tweens/HyTweenVec3.h"

class IHyNode3d
{
	bool						m_bEnabled;

public:
	HyTweenVec3					pos;
	HyTweenVec3					rot;
	HyTweenVec3					rot_pivot;
	HyTweenVec3					scale;
	HyTweenVec3					scale_pivot;

public:
	IHyNode3d(HyType eInstType, IHyNode2d *pParent);
	virtual ~IHyNode3d();

	bool IsEnabled();

	void GetLocalTransform(glm::mat4 &outMtx) const;
};

#endif /* __IHyNode3d_h__ */
