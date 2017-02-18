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

#include "Assets/Data/IHyData.h"
#include "Assets/HyNodeDataContainer.h"

class HyMesh3d;

class HyMesh3dData : public IHyData
{
	friend class HyNodeDataContainer<HyMesh3dData>;

	glm::vec3				m_vAmbientReflectivity;
	glm::vec3				m_vDiffuseReflectivity;
	glm::vec3				m_vSpecularReflectivity;
	float					m_fSpecularShininess;

	// Only allow HyNodeDataContainer instantiate
	HyMesh3dData(const std::string &sPath);

public:
	virtual ~HyMesh3dData(void);

	virtual void DoFileLoad() override;
};

#endif /* __HyMesh3dData_h__ */
