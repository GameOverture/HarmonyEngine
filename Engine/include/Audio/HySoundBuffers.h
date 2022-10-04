/**************************************************************************
*	HySoundBuffers.h
*	
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HySoundBuffers_h__
#define HySoundBuffers_h__

#include "Afx/HyStdAfx.h"
#include "vendor/miniaudio/miniaudio.h"

class HyAudioCore;

class HySoundBuffers
{
	HyAudioCore &				m_CoreRef;
	const std::string			m_sFILE_PATH;
	const int32					m_iGROUP_ID;
	const bool					m_bIS_STREAMING;
	const int32					m_iINSTANCE_LIMIT;

	std::vector<ma_sound *>		m_SoundList;

public:
	HySoundBuffers(HyAudioCore &coreRef, std::string sFilePath, int32 iGroupId, bool bIsStreaming, int32 iInstanceLimit);
	~HySoundBuffers();

	std::string GetFilePath() const;
	int32 GetGroupId() const;

	bool IsStreaming() const;
	int32 GetInstanceLimit() const;

	bool Load();
	void Unload();

	ma_sound *GetFreshBuffer();
	void AppendBuffer();
};

#endif /* HySoundBuffers_h__ */
