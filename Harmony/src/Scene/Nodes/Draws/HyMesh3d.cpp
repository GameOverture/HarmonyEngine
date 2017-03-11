/**************************************************************************
 *	HyMesh3d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Scene/Nodes/Draws/HyMesh3d.h"

HyMesh3d::HyMesh3d(const char *szPrefix, const char *szName, IHyNode *pParent /*= nullptr*/) : IHyDraw2d(HYTYPE_Mesh3d, szPrefix, szName, pParent)
{

}

HyMesh3d::~HyMesh3d(void)
{
}

/*virtual*/ void HyMesh3d::OnUpdate()
{
}

/*virtual*/ void HyMesh3d::OnUpdateUniforms()
{
	//m_ShaderUniforms.Set(...);
}

// This function is responsible for incrementing the passed in reference pointer the size of the data written
/*virtual*/ void HyMesh3d::OnWriteDrawBufferData(char *&pRefDataWritePos)
{
}
