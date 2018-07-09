/**************************************************************************
 *	HyEntity3dData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyEntity3dData_h__
#define HyEntity3dData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Loadables/HyAtlas.h"

#include "assimp/Importer.hpp"      // C++ importer interface
#include "assimp/Exporter.hpp"      // C++ importer interface
#include "assimp/scene.h"           // Output data structure
#include "assimp/postprocess.h"     // Post processing flags

class HyEntity3dData : public IHyNodeData
{
	const aiScene *		m_pAiScene;

public:
	HyEntity3dData(const std::string &sPath);
	virtual ~HyEntity3dData(void);
};

#endif /* HyEntity3dData_h__ */
