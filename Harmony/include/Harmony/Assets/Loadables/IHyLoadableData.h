/**************************************************************************
*	IHyLoadableData.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLoadableData_h__
#define IHyLoadableData_h__

#include "Afx/HyStdAfx.h"

class IHyRenderer;

class IHyLoadableData
{
	friend class HyAssets;

	const HyLoadableType			m_eLOADABLE_TYPE;

	HyLoadState						m_eLoadState;
	uint32							m_uiRefCount;

public:
	IHyLoadableData(HyLoadableType eType) :	m_eLOADABLE_TYPE(eType),
											m_eLoadState(HYLOADSTATE_Inactive),
											m_uiRefCount(0)
	{ }

	HyLoadableType GetLoadableType()							{ return m_eLOADABLE_TYPE; }
	HyLoadState GetLoadableState()								{ return m_eLoadState; }

	virtual void OnLoadThread() = 0;
	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* IHyLoadableData_h__ */
