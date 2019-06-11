/**************************************************************************
*	HyAudio.h
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudio_h__
#define HyAudio_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudio.h"
#include "Audio/Harness/IHyAudioBank.h"
#include "Audio/Harness/IHyAudioInst.h"

class HyAudioBank;

using fpCreateHyAudio = IHyAudio *(*)();
using fpCreateHyAudioBank = IHyAudioBank *(*)(IHyAudio *);
using fpCreateHyAudioInst = IHyAudioInst *(*)(IHyAudio *, const char *);

class HyAudio
{
	static fpCreateHyAudio					sm_fpCreateHyAudio;
	static fpCreateHyAudioBank				sm_fpCreateHyAudioBank;
	static fpCreateHyAudioInst				sm_fpCreateHyAudioInst;
	
	static IHyAudio *						sm_pInternal;
	std::map<std::string, HyAudioBank *>	m_AudioBankMap;

public:
	HyAudio(std::string sDataDir);
	~HyAudio();

	static IHyAudioBank *AllocateAudioBank();
	static IHyAudioInst *AllocateAudioInst(const char *szPath);

	HyAudioBank *GetAudioBank(const std::string &sBankName);
	void Update();
};

#endif /* HyAudio_h__ */
