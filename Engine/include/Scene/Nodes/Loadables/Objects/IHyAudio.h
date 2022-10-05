/**************************************************************************
 *	IHyAudio.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyAudio_h__
#define IHyAudio_h__

#include "Afx/HyStdAfx.h"
#include "Scene/AnimFloats/HyAnimFloat.h"

#define HYAUDIO_InfiniteLoops 255

typedef std::vector<std::pair<uint32, uint32>> HyAudioPlayList;	// <checksum, weight>

template<typename NODETYPE, typename ENTTYPE>
class IHyAudio : public NODETYPE
{
	static HyAudioNodeHandle		sm_hUniqueIdCounter;
	const HyAudioNodeHandle			m_hUNIQUE_ID;

	uint32							m_uiCueFlags;
	float							m_fVolume;
	float							m_fPitch;

	// Configurable
	struct AudioStateAttribs
	{
		uint32						m_ePlayListMode : 3;
		uint32						m_bAllowRepeats : 1;
		uint32						m_uiPriority : 4;
		uint32						m_uiLoops : 8;			// 255 = loop forever (HYAUDIO_InfiniteLoops)
		uint32						m_uiMaxDistance : 16;

		AudioStateAttribs(HyPlayListMode ePlayList, bool bAllowRepeats, uint8 uiPriority, uint8 uiLoops, uint16 uiMaxDist)
		{
			m_ePlayListMode = ePlayList;
			m_bAllowRepeats = bAllowRepeats;
			m_uiPriority = uiPriority;
			m_uiLoops = uiLoops;
			m_uiMaxDistance = uiMaxDist;
		}
	};
	std::vector<AudioStateAttribs>	m_AudioStateAttribList;
	std::vector<uint32>				m_CurPlayList;		// Holds Checksums
	uint32							m_uiLastPlayed;		// Checksum of last played audio

public:
	HyAnimFloat						volume;
	HyAnimFloat						pitch;

public:
	IHyAudio(std::string sPrefix, std::string sName, ENTTYPE *pParent);
	IHyAudio(HyAudioHandle hAudioHandle, ENTTYPE *pParent);
	IHyAudio(const IHyAudio &copyRef);
	virtual ~IHyAudio(void);
	
	const IHyAudio &operator=(const IHyAudio &rhs);

	HyAudioNodeHandle GetHandle() const;

	int32 GetLoops() const;
	void SetLoops(int32 iLoops);
	void PlayOneShot(bool bUseCurrentSettings /*= true*/);
	void Play();
	void Stop();
	void SetPause(bool bPause);

	virtual bool SetState(uint32 uiStateIndex) override;
	virtual bool IsLoadDataValid() override;

	uint32 PullNextSound();	// Returns the next sound checksum to be played
	uint32 GetLastPulledSound() const;

protected:
	virtual void OnDataAcquired() override;
	virtual void OnLoadedUpdate() override;

	void Shuffle();

	uint32 PullEntryIndex(const HyAudioPlayList &entriesList);
};



#endif /* IHyAudio_h__ */
