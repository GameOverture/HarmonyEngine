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

#include "Assets\Data\HyDataDraw.h"
#include "Assets\HyManager_Data.h"

class HyPrimitive2dData : public HyDataDraw
{
	friend class HyNodeDataContainer<HyPrimitive2dData>;

	// Only allow HyNodeDataContainer instantiate
	HyPrimitive2dData(const std::string &sPath, int32 iShaderId);

public:
	virtual ~HyPrimitive2dData();

	virtual void DoFileLoad() override;
};

#endif /* __HyPrimitive2dData_h__ */
