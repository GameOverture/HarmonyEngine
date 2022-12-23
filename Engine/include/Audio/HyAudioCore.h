/**************************************************************************
 *	HyAudioCore.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudioCore_h__
#define HyAudioCore_h__

#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFileAudio.h"
#include "Scene/Nodes/IHyNode.h"
#include "Utilities/HyJson.h"

#define HY_DEFAULT_SAMPLE_RATE 48000

#ifdef HY_USE_SDL2_AUDIO
	#define MA_NO_DEVICE_IO // Disables the ma_device API. SDL will provide the backend
#endif
#include "vendor/miniaudio/miniaudio.h"

enum HySoundCue
{
	HYSOUNDCUE_PlayOneShotDefault = 0,
	HYSOUNDCUE_PlayOneShot,
	HYSOUNDCUE_Start,
	HYSOUNDCUE_Stop,
	HYSOUNDCUE_Pause,
	HYSOUNDCUE_Unpause,
	HYSOUNDCUE_Attributes,

	NUM_HYSOUNDCUE
};

class HyAudioCore
{
	friend class HyAssets;
	friend class HyScene;

	struct SoundGroup
	{
		const std::string							m_sNAME;
		const int32									m_iID;
		ma_sound_group								m_Group;

		SoundGroup(std::string sName, int32 iId) :
			m_sNAME(sName),
			m_iID(iId)
		{ }
	};
	std::vector<SoundGroup *>						m_GroupList;
	std::vector<HyFileAudio *>						m_BankList;
	std::map<HyAudioHandle, HySoundBuffers *>		m_HotLoadMap;
	uint32											m_uiHotLoadCount;

	struct PlayInfo
	{
		float										m_fVolume = 1.0f;
		float										m_fPitch = 1.0f;
		uint32										m_uiSoundChecksum = 0;
		uint8										m_uiLoops = 0;

		ma_sound *									m_pSound = nullptr;
	};
	std::unordered_map<HyAudioNodeHandle, PlayInfo>	m_PlayMap;
	std::vector<PlayInfo>							m_OneShotList;

	std::vector<std::string>						m_sDeviceList;

	ma_device_config								m_DevConfig;
	ma_device										m_Device;

#ifdef HY_USE_SDL2_AUDIO
	SDL_AudioDeviceID								m_SdlDeviceId;
#endif

	ma_engine										m_Engine;

	ma_resource_manager_config						m_ResConfig;
	ma_resource_manager								m_ResourceManager;

public:
	HyAudioCore();
	virtual ~HyAudioCore(void);

	const char *GetAudioDriver();

	ma_engine *GetEngine();
	ma_sound_group *GetGroup(int32 iId);

	void SetGlobalVolume(float fVolume);

	HyAudioHandle HotLoad(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit);
	void HotUnload(HyAudioHandle hAudioHandle);

protected:
	void Update();

	void AddBank(HyFileAudio *pBankFile);
	void AddGroup(std::string sName, int32 iId);
	void ProcessCue(IHyNode *pNode, HySoundCue eCueType);

	void StartSound(PlayInfo &playInfoRef);
	void StopSound(PlayInfo &playInfoRef);
	void PauseSound(PlayInfo &playInfoRef);
	void UnpauseSound(PlayInfo &playInfoRef);
	void ManipSound(PlayInfo &playInfoRef);

	static void DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount);

#ifdef HY_USE_SDL2_AUDIO
	friend void SdlDataCallback(void *pUserData, Uint8 *pBuffer, int bufferSizeInBytes);
#endif
};

#endif /* HyAudioCore_h__ */
