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
	const HyFileType				m_eLOADABLE_TYPE;
	const std::string				m_sFILE_NAME;
	const uint32					m_uiBANK_ID;
	const uint32					m_uiMANIFEST_INDEX;

	HyLoadState						m_eLoadState;
	uint32							m_uiRefCount;

	uint32							m_hGfxApiPbo;
	uint8 *							m_pGfxApiPixelBuffer;

public:
	IHyFile(HyFileType eType, std::string sFileName, uint32 uiBankId, uint32 uiManifestIndex) :
		m_eLOADABLE_TYPE(eType),
		m_sFILE_NAME(sFileName),
		m_uiBANK_ID(uiBankId),
		m_uiMANIFEST_INDEX(uiManifestIndex),
		m_eLoadState(HYLOADSTATE_Inactive),
		m_uiRefCount(0),
		m_hGfxApiPbo(0),
		m_pGfxApiPixelBuffer(nullptr)
	{ }

	bool IsExtrinsic() const		{ return m_uiMANIFEST_INDEX == std::numeric_limits<uint32>::max(); }

	HyFileType GetLoadableType()	{ return m_eLOADABLE_TYPE; }
	HyLoadState GetLoadableState()	{ return m_eLoadState; }

	uint32 GetBankId() const		{ return m_uiBANK_ID; }

	uint32 GetManifestIndex() const { return m_uiMANIFEST_INDEX; }

	virtual std::string AssetTypeName() = 0;
	virtual void OnLoadThread() = 0;
	virtual void OnRenderThread(IHyRenderer &rendererRef) = 0;

	virtual std::string GetAssetInfo() = 0;
};

#endif /* IHyFile_h__ */
