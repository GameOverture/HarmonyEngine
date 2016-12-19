/**************************************************************************
 *	HyMesh3d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Instances/HyMesh3d.h"

HyMesh3d::HyMesh3d(const char *szPrefix, const char *szName) : IHyInst2d(HYTYPE_Mesh3d, szPrefix, szName)
{

}

HyMesh3d::~HyMesh3d(void)
{
}

/*virtual*/ void HyMesh3d::OnInstUpdate()
{
}

/*virtual*/ void HyMesh3d::OnUpdateUniforms()
{
	//m_ShaderUniforms.Set(...);
}

// This function is responsible for incrementing the passed in reference pointer the size of the data written
/*virtual*/ void HyMesh3d::DefaultWriteDrawBufferData(char *&pRefDataWritePos)
{
}
