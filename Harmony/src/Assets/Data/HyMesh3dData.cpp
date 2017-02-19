/**************************************************************************
 *	HyMesh3dData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Assets/Data/HyMesh3dData.h"

HyMesh3dData::HyMesh3dData(const std::string &sPath) :	IHyData(HYTYPE_Mesh3d, sPath)
{
	m_vAmbientReflectivity.x = 0.1f; m_vAmbientReflectivity.y = 0.1f; m_vAmbientReflectivity.z = 0.1f;
	m_vDiffuseReflectivity.x = 0.4f; m_vDiffuseReflectivity.y = 0.4f; m_vDiffuseReflectivity.z = 0.4f;
	m_vSpecularReflectivity.x = 0.9f; m_vSpecularReflectivity.y = 0.9f; m_vSpecularReflectivity.z = 0.9f;

	m_fSpecularShininess = 180.0f;
}

HyMesh3dData::~HyMesh3dData(void)
{
}

/*virtual*/ void HyMesh3dData::SetRequiredAtlasIds(HyGfxData &gfxDataOut)
{
	HyError("Not implemented");
}
