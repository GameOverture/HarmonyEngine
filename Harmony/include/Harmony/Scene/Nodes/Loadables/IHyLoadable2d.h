/**************************************************************************
*	IHyLoadable2d.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLoadable2d_h__
#define IHyLoadable2d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/IHyNode2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable.h"
#include "Scene/Tweens/HyTweenVec3.h"

class IHyLoadable2d : public IHyNode2d, public IHyLoadable
{
public:
	IHyLoadable2d(HyType eNodeType, const char *szPrefix, const char *szName, HyEntity2d *pParent);
	IHyLoadable2d(const IHyLoadable2d &copyRef);
	virtual ~IHyLoadable2d();

	const IHyLoadable2d &operator=(const IHyLoadable2d &rhs);

private:
	virtual HyType _LoadableGetType() override;
};

#endif /* IHyLoadable2d_h__ */