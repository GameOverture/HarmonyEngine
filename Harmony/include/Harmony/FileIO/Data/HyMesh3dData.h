/**************************************************************************
 *	HyMesh3dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyMesh3dData_h__
#define __HyMesh3dData_h__

#include "Afx/HyStdAfx.h"

#include "FileIO/Data/IHyData.h"
#include "FileIO/HyFactory.h"

class HyMesh3d;

class HyMesh3dData : public IHyData
{
	friend class HyFactory<HyMesh3dData>;

	vec3					m_vAmbientReflectivity;
	vec3					m_vDiffuseReflectivity;
	vec3					m_vSpecularReflectivity;
	float					m_fSpecularShininess;

	// Only allow HyFactory instantiate
	HyMesh3dData(const std::string &sPath);

public:
	virtual ~HyMesh3dData(void);

	virtual void DoFileLoad(HyAtlasManager &atlasManagerRef);
};

#endif /* __HyMesh3dData_h__ */
