/**************************************************************************
*	IHyData2d.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyData2d_h__
#define __IHyData2d_h__

#include "Afx/HyStdAfx.h"

#include "FileIO/Data/IHyData.h"
#include "FileIO/HyAtlasManager.h"

class IHyData2d : public IHyData
{
	friend class IHyFileIO;

	static HyAtlasManager *				sm_pAtlasManager;
	std::set<HyAtlasGroup *>			m_AssociatedAtlases;

public:
	IHyData2d(HyInstanceType eInstType, const std::string &sPath);
	virtual ~IHyData2d();

	virtual void SetLoadState(HyLoadState eState);

	HyAtlasGroup *RequestTexture(uint32 uiAtlasGroupId, uint32 uiTextureIndex);
	const std::set<HyAtlasGroup *> &GetAssociatedAtlases();

	// Only invoked on the Load thread
	virtual void DoFileLoad() = 0;
};

#endif /* __IHyData2d_h__ */
