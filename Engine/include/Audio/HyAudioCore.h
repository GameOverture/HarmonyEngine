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

class HyInput;

enum HySoundCue
{
	HYSOUNDCUE_PlayOneShotDefault = 0,
	HYSOUNDCUE_PlayOneShot,
	HYSOUNDCUE_Start,
	HYSOUNDCUE_Stop,
	HYSOUNDCUE_Pause,
	HYSOUNDCUE_Unpause,
	HYSOUNDCUE_Attributes,
	HYSOUNDCUE_DeleteInstance,

	NUM_HYSOUNDCUE
};

class HyAudioCore
{
	friend class HyAssets;
	friend class HyScene;

	HyInput &										m_InputRef;

	struct SoundCategory
	{
		const std::string							m_sNAME;
		const int32									m_iID;
		ma_sound_group								m_Group;

		SoundCategory(std::string sName, int32 iId) :
			m_sNAME(sName),
			m_iID(iId)
		{ }
	};
	std::vector<SoundCategory *>					m_CategoryList;
	std::vector<HyFileAudio *>						m_BankList;
	std::map<HyAudioHandle, HySoundAsset *>			m_HotLoadMap;
	uint32											m_uiHotLoadCount;

	struct PlayInfo
	{
		float										m_fVolume = 1.0f;
		float										m_fPitch = 1.0f;
		uint32										m_uiSoundChecksum = 0;
		uint8										m_uiLoops = 0;

		ma_sound *									m_pSoundBuffer = nullptr;
	};
	std::unordered_map<HyAudioNodeHandle, PlayInfo>	m_PlayMap;

#ifdef HY_USE_SDL2_AUDIO
	SDL_AudioDeviceID								m_SdlDeviceId;
#endif

	ma_engine *										m_pEngine;

	// Used in browser environments to queue up loads (and other settings) until user input occurs. Once input occurs we can initialize the audio device (then load all the sounds that are queued up)
	std::vector<HySoundAsset *>						m_DeferredLoadingList;
	float											m_fDeferredGlobalVolume;

public:
	HyAudioCore(HyInput &inputRef);
	virtual ~HyAudioCore(void);

	void InitDevice();
	void StartDevice();
	void StopDevice();

	const char *GetAudioDriver();

	ma_engine *GetEngine();
	ma_sound_group *GetCategory(int32 iId);

	void SetGlobalVolume(float fVolume);

	void DeferLoading(HySoundAsset *pBuffer);

	HyAudioHandle HotLoad(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit);
	void HotUnload(HyAudioHandle hAudioHandle);

protected:
	void Update();

	void AddBank(HyFileAudio *pBankFile);
	void AddCategory(std::string sName, int32 iId);
	void CategoryInit(SoundCategory *pSndCategory);
	void ProcessCue(IHyNode *pNode, HySoundCue eCueType);

	bool StartSound(PlayInfo &playInfoRef); // Returns false if there are no buffers available (instance limit)
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
