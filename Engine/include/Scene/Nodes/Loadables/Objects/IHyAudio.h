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
#include "Assets/Nodes/HyNodePath.h"

#define HYAUDIO_InfiniteLoops 255

struct HyAudioPlaylistEntry
{
	HyAudioHandle	m_hAudioHandle;
	uint32			m_uiWeight;

	HyAudioPlaylistEntry(uint32 uiFirst, uint32 uiSecond, uint32 uiWeight) :
		m_hAudioHandle(uiFirst, uiSecond),
		m_uiWeight(uiWeight)
	{ }
};
typedef std::vector<HyAudioPlaylistEntry> HyAudioPlaylist;

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
		uint32						m_ePlaylistMode : 3;
		uint32						m_bAllowRepeats : 1;
		uint32						m_uiPriority : 4;
		uint32						m_uiLoops : 8;			// 255 = loop forever (HYAUDIO_InfiniteLoops)
		uint32						m_uiMaxDistance : 16;

		AudioStateAttribs(HyPlaylistMode ePlaylist, bool bAllowRepeats, uint8 uiPriority, uint8 uiLoops, uint16 uiMaxDist)
		{
			m_ePlaylistMode = ePlaylist;
			m_bAllowRepeats = bAllowRepeats ? 1 : 0;
			m_uiPriority = uiPriority;
			m_uiLoops = uiLoops;
			m_uiMaxDistance = uiMaxDist;
		}
	};
	std::vector<AudioStateAttribs>	m_AudioStateAttribList;
	std::vector<HyAudioHandle>		m_SoundOrderList;		// Holds the sorted, currently determined order of sound handles (checksum/bankId) to play
	HyAudioHandle					m_hLastPlayed;			// Checksum/BankId of last played sound from m_SoundOrderList

public:
	HyAnimFloat						volume;
	HyAnimFloat						pitch;

public:
	IHyAudio(const HyNodePath &nodePath, ENTTYPE *pParent);
	IHyAudio(uint32 uiSoundChecksum, uint32 uiBankId, ENTTYPE *pParent);
	IHyAudio(HyAudioHandle hAudioHandle, ENTTYPE *pParent);
	IHyAudio(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit, int32 iCategoryId, ENTTYPE *pParent);
	IHyAudio(const IHyAudio &copyRef);
	virtual ~IHyAudio(void);
	
	const IHyAudio &operator=(const IHyAudio &rhs);

	HyAudioNodeHandle GetHandle() const;

	int32 GetLoops() const;
	void SetLoops(int32 iLoops);
	int32 GetPriority() const;
	void SetPriority(int32 iPriority);
	bool IsPlaying() const;	// Does not return true for playing OneShots
	void PlayOneShot(bool bUseCurrentSettings /*= true*/);
	void Play();
	void SetPause(bool bPause);
	float GetElapsedPlayTime() const;
	void Stop();

	virtual bool SetState(uint32 uiStateIndex) override;
	virtual bool IsLoadDataValid() override;

	HyAudioHandle PullNextSound();	// Returns the next sound 'HyAudioHandle' (checksum/bankId) to be played
	HyAudioHandle GetLastPulledSound() const;

protected:
	virtual void OnDataAcquired() override;
	virtual void OnLoadedUpdate() override;
};

#endif /* IHyAudio_h__ */
