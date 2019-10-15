/**************************************************************************
*	HyPrefab3d.h
*
*	Harmony Engine
*	Copyright (c) 2018 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPrefab3d_h__
#define HyPrefab3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Visables/Drawables/IHyDrawable3d.h"

class HyPrefab3d : public IHyDrawable3d
{
public:
	HyPrefab3d(const char *szPrefix, const char *szName, HyEntity3d *pParent);
	HyPrefab3d(const HyPrefab3d &copyRef);
	virtual ~HyPrefab3d();

	virtual HyPrefab3d *Clone() const override;
};

#endif /* HyPrefab3d_h__ */
