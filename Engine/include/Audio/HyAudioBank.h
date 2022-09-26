/**************************************************************************
*	HyAudioBank.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioBank_h__
#define HyAudioBank_h__

#include "Afx/HyStdAfx.h"
#include "Utilities/HyJson.h"
#include "Audio/HySoundBuffers.h"

class HyAudioBank
{
	std::vector<HySoundBuffers *>			m_SoundBuffers;
	std::map<uint32, HySoundBuffers *>		m_ChecksumMap;

public:
	HyAudioBank(HyAudioCore &coreRef, HyJsonObj bankObj, std::string sBankFilePath);
	virtual ~HyAudioBank(void);

	virtual bool ContainsSound(uint32 uiAssetChecksum);
	HySoundBuffers *GetSound(uint32 uiChecksum);

	virtual bool Load();
	virtual void Unload();
};

#endif /* HyAudioBank_h__ */
