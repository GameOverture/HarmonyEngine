/**************************************************************************
*	IHyLoadable3d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLoadable3d_h__
#define IHyLoadable3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode3d.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Assets/Nodes/IHyNodeData.h"
#include "Scene/Tweens/HyTweenVec3.h"
#include "Utilities/HyMath.h"

class IHyLoadable3d : public IHyNode3d, public IHyLoadable
{
public:
	IHyLoadable3d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity3d *pParent);
	IHyLoadable3d(const IHyLoadable3d &copyRef);
	virtual ~IHyLoadable3d();

	const IHyLoadable3d &operator=(const IHyLoadable3d &rhs);

private:
	virtual HyType _LoadableGetType() override final;
	virtual IHyLoadable *_LoadableGetParentPtr() override final;
};

#endif /* IHyLoadable3d_h__ */
