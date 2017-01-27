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

#include "Scene/Nodes/Draws/IHyDraw2d.h"

class HyMesh3d : public IHyDraw2d
{
	
public:
	HyMesh3d(const char *szPrefix, const char *szName);
	virtual ~HyMesh3d(void);

	virtual void OnUpdate() override;
	virtual void OnUpdateUniforms() override;

	// This function is responsible for incrementing the passed in reference pointer the size of the data written
	virtual void OnWriteDrawBufferData(char *&pRefDataWritePos) override;
};

#endif /* __HyMesh3d_h__ */
