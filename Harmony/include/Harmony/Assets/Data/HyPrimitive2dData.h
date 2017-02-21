/**************************************************************************
*	HyPrimitive2dData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyPrimitive2dData_h__
#define __HyPrimitive2dData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Data/IHyData.h"
#include "Assets/Loadables/HyAtlasGroup.h"

class HyPrimitive2dData : public IHyData
{
public:
	HyPrimitive2dData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAtlasContainer &atlasContainerRef);
	virtual ~HyPrimitive2dData();

	virtual void AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIdsOut) override;
};

#endif /* __HyPrimitive2dData_h__ */
