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
#include "Audio/Harness/IHyAudio.h"
#include "Audio/Harness/IHyAudioBank.h"
#include "Audio/Harness/IHyAudioInst.h"

class HyFileAudio;

class HyAudioHarness
{
	using fpAllocateHyAudio					= IHyAudio *(*)();
	using fpAllocateHyAudioBank				= IHyAudioBank *(*)(IHyAudio *, const jsonxx::Object &);
	using fpAllocateHyAudioInst				= IHyAudioInst *(*)(IHyAudio *, const jsonxx::Object &);

	fpAllocateHyAudio						m_fpAllocateHyAudio;
	fpAllocateHyAudioBank					m_fpAllocateHyAudioBank;
	fpAllocateHyAudioInst					m_fpAllocateHyAudioInst;
	IHyAudio *								m_pInternal;

	std::map<std::string, HyFileAudio *>	m_AudioBankMap;

public:
	HyAudioHarness(std::string sDataDir);
	~HyAudioHarness();

	IHyAudioBank *AllocateAudioBank(const jsonxx::Object &bankObjRef);
	IHyAudioInst *AllocateAudioInst(const jsonxx::Object &instObjRef);
	HyFileAudio *GetAudioBank(const std::string &sBankName);

	void Update();
};

#endif /* HyAudioHarness_h__ */
