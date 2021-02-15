/**************************************************************************
 *	IHyAudioCore.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2019 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyAudioCore_h__
#define IHyAudioCore_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFileAudio.h"
#include "Scene/Nodes/IHyNode.h"

class IHyAudioCore
{
public:
	enum CueType
	{
		CUETYPE_PlayOneShotDefault = 0,
		CUETYPE_PlayOneShot,
		CUETYPE_Start,
		CUETYPE_Stop,
		CUETYPE_Pause,
		CUETYPE_Unpause,
		CUETYPE_Attributes,
		
		NUM_CUETYPES
	};

protected:
	struct Cue
	{
		IHyNode * const					m_pNODE;
		const CueType					m_eCUE_TYPE;

		Cue(IHyNode *pNode, CueType eCueType) :
			m_pNODE(pNode),
			m_eCUE_TYPE(eCueType)
		{ }
	};
	std::vector<Cue>					m_CueList;

public:
	IHyAudioCore() = default;
	virtual ~IHyAudioCore(void) = default;

	void AppendCue(IHyNode *pNode, CueType eCueType) {
		m_CueList.emplace_back(pNode, eCueType);
	}

	virtual void SetSfxVolume(float fGlobalSfxVolume) = 0;
	virtual void SetMusicVolume(float fGlobalMusicVolume) = 0;
	virtual IHyFileAudioImpl *AllocateAudioBank(HyJsonObj bankObj) = 0;

	virtual void OnUpdate() = 0;
};

class HyAudioCore_Null : public IHyAudioCore
{
public:
	HyAudioCore_Null() = default;
	virtual ~HyAudioCore_Null() = default;

	virtual void SetSfxVolume(float fGlobalSfxVolume) override
	{ }

	virtual void SetMusicVolume(float fGlobalMusicVolume) override
	{ }

	virtual IHyFileAudioImpl *AllocateAudioBank(HyJsonObj bankObj) override
	{
		return HY_NEW HyAudioBank_Null();
	}

	virtual void OnUpdate() override
	{ }
};

#endif /* IHyAudioCore_h__ */
