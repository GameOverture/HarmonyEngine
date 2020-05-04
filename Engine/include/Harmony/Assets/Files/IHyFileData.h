/**************************************************************************
*	IHyFileData.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyLoadableData_h__
#define IHyLoadableData_h__

#include "Afx/HyStdAfx.h"

class IHyRenderer;

class IHyFileData
{
	friend class HyAssets;

	const HyFileType				m_eLOADABLE_TYPE;

	HyLoadState						m_eLoadState;
	uint32							m_uiRefCount;

protected:
	uint32							m_hGfxApiPbo;
	uint8 *							m_pGfxApiPixelBuffer;

public:
	IHyFileData(HyFileType eType) :
		m_eLOADABLE_TYPE(eType),
		m_eLoadState(HYLOADSTATE_Inactive),
		m_uiRefCount(0),
		m_hGfxApiPbo(0),
		m_pGfxApiPixelBuffer(nullptr)
	{ }

	HyFileType GetLoadableType()	{ return m_eLOADABLE_TYPE; }
	HyLoadState GetLoadableState()	{ return m_eLoadState; }

	virtual void OnLoadThread() = 0;
	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* IHyLoadableData_h__ */
