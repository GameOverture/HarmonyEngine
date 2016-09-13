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

#include "Assets/Data/IHyData.h"
#include "Assets/HyTextures.h"

class IHyData2d : public IHyData
{
	friend class HyAssetManager;

	const int32						m_iSHADER_ID;

	static HyTextures *				sm_pTextures;
	std::set<HyAtlasGroup *>		m_AssociatedAtlases;

public:
	IHyData2d(HyInstanceType eInstType, const std::string &sPath, int32 iShaderId);
	virtual ~IHyData2d();

	int32 GetShaderId();

	virtual void SetLoadState(HyLoadState eState);

	HyAtlasGroup *RequestTexture(uint32 uiAtlasGroupId);
	const std::set<HyAtlasGroup *> &GetAssociatedAtlases();

	virtual void OnLoadThread();

	// Only invoked on the Load thread
	virtual void DoFileLoad() = 0;
};

#endif /* __IHyData2d_h__ */
