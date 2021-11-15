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
	friend class HyAssets;
	friend class HyScene;

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
	float											m_fGlobalSfxVolume;
	float											m_fGlobalMusicVolume;

	std::vector<IHyFileAudioImpl *>					m_AudioFileList;

	// Keep track of non-oneshot active sfx
	struct PlayInfo
	{
		float				m_fVolume = 1.0f;
		float				m_fPitch = 1.0f;
		uint32				m_uiSoundChecksum = 0;
		uint8				m_uiLoops = 0;

		int32				m_iApiData = 0;
	};
	std::unordered_map<HyAudioHandle, PlayInfo>		m_PlayMap;

public:
	IHyAudioCore();
	virtual ~IHyAudioCore(void);

	void SetSfxVolume(float fGlobalSfxVolume);
	void SetMusicVolume(float fGlobalMusicVolume);

protected:
	IHyFileAudioImpl *AllocateAudioBank(HyJsonObj bankObj);
	void ProcessCue(IHyNode *pNode, CueType eCueType);

	// Derived classes must invoke this when 'hHandle' audio is finished playing
	void ReportFinished(HyAudioHandle hHandle);

	virtual IHyFileAudioImpl *OnAllocateAudioBank(HyJsonObj bankObj) = 0;
	virtual void OnSetSfxVolume(float fGlobalSfxVolume) = 0;
	virtual void OnSetMusicVolume(float fGlobalMusicVolume) = 0;

	virtual void OnCue_Play(HyAudioHandle hHandle, PlayInfo &playInfoRef) = 0;
	virtual void OnCue_Stop(HyAudioHandle hHandle) = 0;
	virtual void OnCue_Pause(HyAudioHandle hHandle) = 0;
	virtual void OnCue_Unpause(HyAudioHandle hHandle) = 0;
	virtual void OnCue_Volume(HyAudioHandle hHandle) = 0;
};

class HyAudioCore_Null : public IHyAudioCore
{
public:
	HyAudioCore_Null() = default;
	virtual ~HyAudioCore_Null() = default;

	virtual IHyFileAudioImpl *OnAllocateAudioBank(HyJsonObj bankObj) override
	{
		return HY_NEW HyAudioBank_Null();
	}

	virtual void OnSetSfxVolume(float fGlobalSfxVolume) override
	{ }

	virtual void OnSetMusicVolume(float fGlobalMusicVolume) override
	{ }
};

#endif /* IHyAudioCore_h__ */
