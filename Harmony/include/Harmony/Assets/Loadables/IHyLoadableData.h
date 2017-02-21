/**************************************************************************
*	IHyLoadableData.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __IHyLoadableData_h__
#define __IHyLoadableData_h__

#include "Afx/HyStdAfx.h"

class IHyRenderer;

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

	HyLoadState GetLoadState() { return m_eLoadState; }

	virtual void OnLoadThread() = 0;
	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* __IHyLoadableData_h__ */
