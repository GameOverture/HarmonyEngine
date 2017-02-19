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
#include "Assets/Containers/HyNodeDataContainer.h"

class HyPrimitive2dData : public IHyData
{
	friend class HyNodeDataContainer<HyPrimitive2dData>;

	// Only allow HyNodeDataContainer instantiate
	HyPrimitive2dData(const std::string &sPath);

public:
	virtual ~HyPrimitive2dData();

	virtual void SetRequiredAtlasIds(HyGfxData &gfxDataOut) override;
};

#endif /* __HyPrimitive2dData_h__ */
