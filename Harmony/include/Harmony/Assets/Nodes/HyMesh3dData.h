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

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Loadables/HyAtlas.h"

class HyMesh3dData : public IHyNodeData
{
	glm::vec3				m_vAmbientReflectivity;
	glm::vec3				m_vDiffuseReflectivity;
	glm::vec3				m_vSpecularReflectivity;
	float					m_fSpecularShininess;

public:
	HyMesh3dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAtlasContainer &atlasContainerRef);
	virtual ~HyMesh3dData(void);
};

#endif /* __HyMesh3dData_h__ */
