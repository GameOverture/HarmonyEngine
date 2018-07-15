/**************************************************************************
 *	HyEntity3d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyEntity3d_h__
#define HyEntity3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"

class HyEntity3d : public IHyLoadable3d
{
protected:
	std::vector<IHyNode3d *>		m_ChildList;

public:
	HyEntity3d(const char *szPrefix, const char *szName, HyEntity3d *pParent);
	virtual ~HyEntity3d(void);

protected:
	virtual void NodeUpdate() override final;

	// Optional user overrides below
	virtual void OnUpdate() { }
};

#endif /* HyEntity3d_h__ */
