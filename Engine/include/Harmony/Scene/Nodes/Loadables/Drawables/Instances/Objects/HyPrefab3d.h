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
#include "Scene/Nodes/Loadables/Drawables/Instances/IHyInstance3d.h"

class HyPrefab3d : public IHyInstance3d
{
public:
	HyPrefab3d(std::string sPrefix = "", std::string sName = "", HyEntity3d *pParent = nullptr);
	HyPrefab3d(const HyPrefab3d &copyRef);
	virtual ~HyPrefab3d();
};

#endif /* HyPrefab3d_h__ */
