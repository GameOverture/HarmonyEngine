/**************************************************************************
 *	HyMesh3d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyMesh3d_h__
#define HyMesh3d_h__

#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Draws/IHyDraw3d.h"
#include "Scene/Nodes/Draws/Entities/HyEntity3d.h"

class HyMesh3d : public IHyDraw3d
{
	
public:
	HyMesh3d(const char *szPrefix, const char *szName, HyEntity3d *pParent);
	virtual ~HyMesh3d(void);

	virtual void NodeUpdate() override;
};

#endif /* HyMesh3d_h__ */
