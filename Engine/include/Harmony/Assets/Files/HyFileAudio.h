/**************************************************************************
*	HyFileAudio.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyFileAudio_h__
#define HyFileAudio_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/IHyFile.h"
#include "Audio/Harness/IHyFileAudioImpl.h"

class HyFileAudio : public IHyFile
{
	IHyFileAudioImpl *		m_pInternal;

public:
	HyFileAudio(std::string sFilePath, uint32 uiManifestIndex, IHyFileAudioImpl *pInternal);
	virtual ~HyFileAudio();

	bool ContainsAsset(uint32 uiAssetChecksum);

	virtual std::string AssetTypeName() override;
	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

#endif /* HyFileAudio_h__ */
