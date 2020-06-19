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
#include "Audio/Harness/IHyAudioBank.h"

class HyFileAudio : public IHyFile
{
	IHyAudioBank *					m_pInternal;

public:
	HyFileAudio(std::string sFilePath, uint32 uiManifestIndex, IHyAudioBank *pInternal);
	virtual ~HyFileAudio();

	virtual void OnLoadThread() override;
	virtual void OnRenderThread(IHyRenderer &rendererRef) override;
};

#endif /* HyFileAudio_h__ */
