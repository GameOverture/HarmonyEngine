/**************************************************************************
*	HyPrimitive2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyPrimitive2dData_h__
#define HyPrimitive2dData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/Loadables/HyAtlas.h"

class HyPrimitive2dData : public IHyNodeData
{
public:
	HyPrimitive2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAtlasContainer &atlasContainerRef);
	virtual ~HyPrimitive2dData();
};

#endif /* HyPrimitive2dData_h__ */
