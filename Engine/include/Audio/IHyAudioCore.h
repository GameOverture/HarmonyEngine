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
	static IHyAudioCore *							sm_pInstance;	// Only used in ReportFinished() callback

	float											m_fGlobalSfxVolume;
	float											m_fGlobalMusicVolume;

	std::vector<IHyFileAudioImpl *>					m_AudioFileList;

	// Keep track of active SFX's
	struct PlayInfo
	{
		float				m_fVolume = 1.0f;
		float				m_fPitch = 1.0f;
		uint32				m_uiSoundChecksum = 0;
		uint8				m_uiLoops = 0;

		int32				m_iApiData = -1;
	};

	std::unordered_map<HyAudioHandle, PlayInfo>		m_PlayMap;
	std::vector<PlayInfo>							m_OneShotList;

public:
	IHyAudioCore();
	virtual ~IHyAudioCore(void);

	void SetSfxVolume(float fGlobalSfxVolume);
	void SetMusicVolume(float fGlobalMusicVolume);

protected:
	IHyFileAudioImpl *AllocateAudioBank(HyJsonObj bankObj);
	void ProcessCue(IHyNode *pNode, CueType eCueType);

	virtual IHyFileAudioImpl *OnAllocateAudioBank(HyJsonObj bankObj) = 0;
	virtual void OnSetSfxVolume(float fGlobalSfxVolume) = 0;
	virtual void OnSetMusicVolume(float fGlobalMusicVolume) = 0;

	virtual void OnCue_Play(PlayInfo &playInfoRef) = 0;
	virtual void OnCue_Stop(PlayInfo &playInfoRef) = 0;
	virtual void OnCue_Pause(PlayInfo &playInfoRef) = 0;
	virtual void OnCue_Unpause(PlayInfo &playInfoRef) = 0;
	virtual void OnCue_Volume(PlayInfo &playInfoRef) = 0;
	
	// Derived classes must invoke this whenever audio SFX's are finished playing
	static void OnReportFinished(int32 iApiData);
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
	virtual void OnCue_Play(PlayInfo &playInfoRef) override
	{ }
	virtual void OnCue_Stop(PlayInfo &playInfoRef) override
	{ }
	virtual void OnCue_Pause(PlayInfo &playInfoRef) override
	{ }
	virtual void OnCue_Unpause(PlayInfo &playInfoRef) override
	{ }
	virtual void OnCue_Volume(PlayInfo &playInfoRef) override
	{ }
};

#endif /* IHyAudioCore_h__ */
