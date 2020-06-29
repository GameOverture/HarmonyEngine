/**************************************************************************
*	HyAudioHarness.h
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioHarness_h__
#define HyAudioHarness_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudioCore.h"
#include "Audio/Harness/IHyFileAudioGuts.h"

class HyFileAudio;

class HyAudioHarness
{
	using fpAllocateHyAudio					= IHyAudioCore *(*)();
	using fpAllocateHyAudioBank				= IHyFileAudioGuts *(*)(IHyAudioCore *, const jsonxx::Object &);

	fpAllocateHyAudio						m_fpAllocateHyAudio;
	fpAllocateHyAudioBank					m_fpAllocateHyAudioBank;
	IHyAudioCore *							m_pCore;

	std::map<std::string, HyFileAudio *>	m_AudioBankMap;

public:
	HyAudioHarness(std::string sDataDir);
	~HyAudioHarness();

	IHyFileAudioGuts *AllocateAudioBank(const jsonxx::Object &bankObjRef);
	HyFileAudio *GetAudioBank(const std::string &sBankName);

	void AppendCue(IHyNode *pNode, IHyAudioCore::CueType eCueType);

	void Update();
};

#endif /* HyAudioHarness_h__ */
