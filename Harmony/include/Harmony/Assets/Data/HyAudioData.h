/**************************************************************************
 *	HySfxData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HySfxData_h__
#define __HySfxData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Data/IHyData.h"
#include "Assets/Containers/HyAtlasContainer.h"

//#include "SFML/Audio.hpp"

class HySound;

class HySfxData : public IHyData
{
public:
	HySfxData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAtlasContainer &atlasContainerRef);
	virtual ~HySfxData(void);

	virtual void AppendRequiredAtlasIds(std::set<uint32> &requiredAtlasIdsOut) override;
};

#endif /* __HySfxData_h__ */
