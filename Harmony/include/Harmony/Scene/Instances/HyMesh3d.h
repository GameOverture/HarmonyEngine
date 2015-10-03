/**************************************************************************
 *	HyMesh3d.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyMesh3d_h__
#define __HyMesh3d_h__

#include "Afx/HyStdAfx.h"

#include "Scene/Instances/IHyInst2d.h"

class HyMesh3d : public IHyInst2d
{
	
public:
	HyMesh3d(const char *szPrefix, const char *szName);
	virtual ~HyMesh3d(void);
};

#endif /* __HyMesh3d_h__ */
