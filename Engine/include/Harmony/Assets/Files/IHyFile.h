/**************************************************************************
*	IHyFile.h
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyFile_h__
#define IHyFile_h__

#include "Afx/HyStdAfx.h"

class IHyRenderer;

class IHyFile
{
	friend class HyAssets;

protected:
	const std::string				m_sFILE_NAME;
	const HyFileType				m_eLOADABLE_TYPE;
	const uint32					m_uiMANIFEST_INDEX;

	HyLoadState						m_eLoadState;
	uint32							m_uiRefCount;

	uint32							m_hGfxApiPbo;
	uint8 *							m_pGfxApiPixelBuffer;

public:
	IHyFile(std::string sFileName, HyFileType eType, uint32 uiManifestIndex) :
		m_sFILE_NAME(sFileName),
		m_eLOADABLE_TYPE(eType),
		m_uiMANIFEST_INDEX(uiManifestIndex),
		m_eLoadState(HYLOADSTATE_Inactive),
		m_uiRefCount(0),
		m_hGfxApiPbo(0),
		m_pGfxApiPixelBuffer(nullptr)
	{ }

	HyFileType GetLoadableType()	{ return m_eLOADABLE_TYPE; }
	HyLoadState GetLoadableState()	{ return m_eLoadState; }

	uint32 GetManifestIndex() const { return m_uiMANIFEST_INDEX; }

	virtual void OnLoadThread() = 0;
	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;
};

#endif /* IHyFile_h__ */
