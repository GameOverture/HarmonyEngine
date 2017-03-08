/**************************************************************************
 *	HyAudioData.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyAudioData_h__
#define __HyAudioData_h__

#include "Afx/HyStdAfx.h"

#include "Assets/Nodes/IHyNodeData.h"
#include "Assets/HyAssets.h"

class HySound;

class HyAudioData : public IHyNodeData
{
public:
	HyAudioData(const std::string &sPath, const jsonxx::Value &dataValueRef, HyAssets &assetsRef);
	virtual ~HyAudioData(void);

	virtual void AppendRequiredAtlasIndices(std::set<uint32> &requiredAtlasIdsOut) override;
};

#endif /* __HyAudioData_h__ */
