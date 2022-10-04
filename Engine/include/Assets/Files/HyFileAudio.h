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
#include "Audio/HySoundBuffers.h"
#include "Utilities/HyJson.h"

class HyFileAudio : public IHyFile
{
	std::vector<HySoundBuffers *>			m_SoundBuffers;
	std::map<uint32, HySoundBuffers *>		m_ChecksumMap;

public:
	HyFileAudio(std::string sFilePath, uint32 uiManifestIndex, HyAudioCore &coreRef, HyJsonObj bankObj);
	virtual ~HyFileAudio();

	bool ContainsAsset(uint32 uiAssetChecksum);
	HySoundBuffers *GetSound(uint32 uiChecksum);

	virtual std::string AssetTypeName() override;
	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

#endif /* HyFileAudio_h__ */
