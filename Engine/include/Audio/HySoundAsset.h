/**************************************************************************
*	HySoundAsset.h
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HySoundAsset_h__
#define HySoundAsset_h__

#include "Afx/HyStdAfx.h"
#include "vendor/miniaudio/miniaudio.h"

class HyAudioCore;

class HySoundAsset
{
	HyAudioCore &				m_CoreRef;
	const std::string			m_sFILE_PATH;
	const int32					m_iCATEGORY_ID;
	const bool					m_bIS_STREAMING;
	const int32					m_iINSTANCE_LIMIT;

	std::vector<ma_sound *>		m_SoundBufferList;

public:
	HySoundAsset(HyAudioCore &coreRef, std::string sFilePath, int32 iCategoryId, bool bIsStreaming, int32 iInstanceLimit);
	~HySoundAsset();

	std::string GetFilePath() const;
	int32 GetCategoryId() const;

	bool IsStreaming() const;
	int32 GetInstanceLimit() const;

	bool Load();
	void Unload();

	ma_sound *GetIdleBuffer();

private:
	void InitSoundBuffer(ma_sound *pSound, ma_uint32 uiAdditionalFlags);
};

#endif /* HySoundAsset_h__ */
