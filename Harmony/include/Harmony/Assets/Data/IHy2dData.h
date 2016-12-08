/**************************************************************************
*	IHy2dData.h
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
#include "Renderer/IHyShader.h"

class IHy2dData : public IHyData
{
	friend class HyAssetManager;

	const int32						m_iSHADER_ID;
	bool							m_bIncRenderRef;

	static HyTextures *				sm_pTextures;
	std::set<HyAtlasGroup *>		m_AssociatedAtlases;

	std::set<IHyShader *>			m_AssociatedShaders;

public:
	IHy2dData(HyType eInstType, const std::string &sPath, int32 iShaderId);
	virtual ~IHy2dData();

	int32 GetShaderId();
	bool IsIncrementRenderRefs();

	virtual void SetLoadState(HyLoadState eState);

	HyAtlasGroup *RequestTexture(uint32 uiAtlasGroupId);
	const std::set<HyAtlasGroup *> &GetAssociatedAtlases();

	const std::set<IHyShader *> &GetAssociatedShaders();

	// Only invoked on the Load thread
	virtual void OnLoadThread();
	virtual void DoFileLoad() = 0;
};

#endif /* __IHyData2d_h__ */
