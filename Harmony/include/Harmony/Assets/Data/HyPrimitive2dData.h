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

#include "Assets\Data\IHy2dData.h"
#include "Assets\HyFactory.h"

class HyPrimitive2dData : public IHy2dData
{
	friend class HyFactory<HyPrimitive2dData>;

	// Only allow HyFactory instantiate
	HyPrimitive2dData(const std::string &sPath, int32 iShaderId);

public:
	virtual ~HyPrimitive2dData();

	virtual void DoFileLoad() override;
};

#endif /* __HyPrimitive2dData_h__ */
