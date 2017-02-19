/**************************************************************************
*	HyGfxData.h
*
*	Harmony Engine
*	Copyright (c) 2013 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyGfxData_h__
#define __HyGfxData_h__

#include "Afx/HyStdAfx.h"
#include <set>

class IHyRenderer;

class HyGfxData
{
	friend class HyAssets;

	HyLoadState				m_eLoadState;

	std::set<uint32>		m_RequiredAtlasIds;
	std::set<int32>			m_RequiredCustomShaders;

public:
	HyGfxData();
	virtual ~HyGfxData();

	HyLoadState GetLoadState();

	void Clear();
	void SetRequiredAtlasId(uint32 uiAtlasId);
	void SetRequiredCustomShaderId(int32 iShaderId);

	virtual void OnLoaded() = 0;
};

class IHyLoadableData
{
	friend class HyAssets;

	const HyGfxType			m_eTYPE;

	HyLoadState				m_eLoadState;
	uint32					m_uiRefCount;

public:
	IHyLoadableData(HyGfxType eType) :	m_eTYPE(eType),
										m_eLoadState(HYLOADSTATE_Inactive),
										m_uiRefCount(0)
	{ }

	virtual void OnLoadThread() = 0;
	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
}

#endif /* __HyGfxData_h__ */
