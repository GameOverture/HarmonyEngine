/**************************************************************************
*	HyAudioManager.h
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioManager_h__
#define HyAudioManager_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudio.h"
#include "Audio/Harness/IHyAudioBank.h"
#include "Audio/Harness/IHyAudioInst.h"

class HyFileAudio;

class HyAudioManager
{
	using fpAllocateHyAudio					= IHyAudio *(*)();
	using fpAllocateHyAudioBank				= IHyAudioBank *(*)(IHyAudio *, const jsonxx::Object &);
	using fpAllocateHyAudioInst				= IHyAudioInst *(*)(IHyAudio *, const char *);

	fpAllocateHyAudio						m_fpAllocateHyAudio;
	fpAllocateHyAudioBank					m_fpAllocateHyAudioBank;
	fpAllocateHyAudioInst					m_fpAllocateHyAudioInst;
	IHyAudio *								m_pInternal;

	std::map<std::string, HyFileAudio *>	m_AudioBankMap;

public:
	HyAudioManager(std::string sDataDir);
	~HyAudioManager();

	IHyAudioBank *AllocateAudioBank(const jsonxx::Object &bankObjRef);
	IHyAudioInst *AllocateAudioInst(const char *szPath);
	HyFileAudio *GetAudioBank(const std::string &sBankName);

	void Update();
};

#endif /* HyAudio_h__ */
