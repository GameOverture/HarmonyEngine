/**************************************************************************
 *	HyAudioCore.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#define STB_VORBIS_HEADER_ONLY
#include "vendor/stb/stb_vorbis.c" // Enables Vorbis decoding for miniaudio
#define MINIAUDIO_IMPLEMENTATION
#include "Audio/HyAudioCore.h"
#include "Input/HyInput.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio3d.h"
#include "Diagnostics/Console/IHyConsole.h"

// stb_vorbis implementation must come after the implementation of miniaudio
#undef STB_VORBIS_HEADER_ONLY
#include "vendor/stb/stb_vorbis.c"

#ifdef HY_USE_SDL2_AUDIO
	extern void SdlDataCallback(void *pUserData, Uint8 *pBuffer, int bufferSizeInBytes);
#endif

HyAudioCore::HyAudioCore(HyInput &inputRef) :
	m_InputRef(inputRef),
	m_pEngine(nullptr),
	m_fDeferredGlobalVolume(1.0f),
	m_uiHotLoadCount(1)
{
	InitDevice();
}

/*virtual*/ HyAudioCore::~HyAudioCore(void)
{
	for(auto *pGrp : m_GroupList)
		delete pGrp;

	ma_engine_uninit(m_pEngine);
	delete m_pEngine;
	m_pEngine = nullptr;

#ifdef HY_USE_SDL2_AUDIO
	SDL_CloseAudioDevice(m_SdlDeviceId);
#endif
}

void HyAudioCore::InitDevice()
{
#ifdef HY_PLATFORM_GUI
	return;
#endif

#ifdef HY_PLATFORM_BROWSER
	// Google has implemented a policy in their browsers that prevent automatic media output without first receiving some kind of user input
	// Starting the device may fail if you try to start playback without first handling some kind of user input
	if(m_InputRef.UserInputOccured() == false)
		return;
#endif

	HyLog("--- Initalizing Audio Device ---");
	m_pEngine = HY_NEW ma_engine();

	ma_engine_config engConfig;
	engConfig = ma_engine_config_init();
#ifdef HY_USE_SDL2_AUDIO
	engConfig.noDevice = MA_TRUE;
	engConfig.channels = 2;							// Must be set when not using a device.
	engConfig.sampleRate = HY_DEFAULT_SAMPLE_RATE;	// Must be set when not using a device.
#endif

	ma_result eResult = ma_engine_init(&engConfig, m_pEngine);
	if(eResult != MA_SUCCESS)
	{
		HyLogError("HyAudioCore_miniaudio failed: " << eResult);  // Failed to initialize the engine.
		return;
	}

#ifdef HY_USE_SDL2_AUDIO
	SDL_AudioSpec desiredSpec;
	MA_ZERO_OBJECT(&desiredSpec);
	desiredSpec.freq = ma_engine_get_sample_rate(m_pEngine);
	desiredSpec.format = AUDIO_F32;
	desiredSpec.channels = ma_engine_get_channels(m_pEngine);
	desiredSpec.samples = 512;
	desiredSpec.callback = SdlDataCallback;
	desiredSpec.userdata = this;

	SDL_AudioSpec obtainedSpec;
	m_SdlDeviceId = SDL_OpenAudioDevice(nullptr, 0, &desiredSpec, &obtainedSpec, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if(m_SdlDeviceId == 0)
	{
		HyLogError("Failed to open SDL audio device.");
		return;
	}
	SDL_PauseAudioDevice(m_SdlDeviceId, 0); // Start playback
#endif // HY_USE_SDL2_AUDIO
}

const char *HyAudioCore::GetAudioDriver()
{
	ma_context context;
	ma_result eResult = ma_context_init(nullptr, 0, nullptr, &context);
	if(eResult != MA_SUCCESS)
		HyLogError("ma_context_init failed: " << eResult);

	ma_device_info *pPlaybackInfos;
	ma_uint32 playbackCount;
	ma_device_info *pCaptureInfos;
	ma_uint32 captureCount;
	eResult = ma_context_get_devices(&context, &pPlaybackInfos, &playbackCount, &pCaptureInfos, &captureCount);
	if(eResult != MA_SUCCESS)
		HyLogError("ma_context_get_devices failed: " << eResult);

	return pPlaybackInfos[0].name;
}

ma_engine *HyAudioCore::GetEngine()
{
	return m_pEngine;
}

ma_sound_group *HyAudioCore::GetGroup(int32 iId)
{
	for(auto *pGrp : m_GroupList)
	{
		if(pGrp->m_iID == iId)
			return &pGrp->m_Group;
	}

	return nullptr;
}

void HyAudioCore::SetGlobalVolume(float fVolume)
{
	if(m_pEngine == nullptr)
	{
		m_fDeferredGlobalVolume = fVolume;
		return;
	}

	ma_result eResult = ma_engine_set_volume(m_pEngine, fVolume);
	if(eResult != MA_SUCCESS)
		HyLogError("ma_engine_set_volume failed: " << eResult);
}

#ifdef HY_PLATFORM_BROWSER
void HyAudioCore::DeferLoading(HySoundBuffers *pBuffer)
{
	m_DeferredLoadingList.push_back(pBuffer);
}
#endif

HyAudioHandle HyAudioCore::HotLoad(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit)
{
	HySoundBuffers *pNewSndBuffer = HY_NEW HySoundBuffers(*this, sFilePath, 0, bIsStreaming, iInstanceLimit);
	pNewSndBuffer->Load();
	
	m_uiHotLoadCount++;
	m_HotLoadMap.insert({ static_cast<HyAudioHandle>(m_uiHotLoadCount), pNewSndBuffer });
	
	return static_cast<HyAudioHandle>(m_uiHotLoadCount);
}

void HyAudioCore::HotUnload(HyAudioHandle hAudioHandle)
{
	if(m_HotLoadMap.count(hAudioHandle) == 0)
		return;

	delete m_HotLoadMap[hAudioHandle];
	m_HotLoadMap.erase(hAudioHandle);
}

void HyAudioCore::Update()
{
#ifdef HY_PLATFORM_BROWSER
	if(m_pEngine == nullptr)
	{
		if(m_InputRef.UserInputOccured())
		{
			InitDevice();

			// Now do all the deferred loading and settings
			SetGlobalVolume(m_fDeferredGlobalVolume);
			for(SoundGroup *pGrp : m_GroupList)
				GroupInit(pGrp);
			for(HySoundBuffers *pSndBuff : m_DeferredLoadingList)
				pSndBuff->Load();
			m_DeferredLoadingList.clear();
		}
		else
			return;
	}
#endif

	for(auto iter = m_PlayMap.begin(); iter != m_PlayMap.end(); )
	{
		PlayInfo &playInfoRef = iter->second;
		if(ma_sound_at_end(playInfoRef.m_pSound))
		{
			if(playInfoRef.m_uiLoops > 0)
			{
				ma_sound_seek_to_pcm_frame(playInfoRef.m_pSound, 0); // Rewind sound to beginning
				ma_result eResult = ma_sound_start(playInfoRef.m_pSound);
				if(eResult != MA_SUCCESS)
				{
					HyLogError("ma_sound_seek_to_pcm_frame failed: " << eResult);
					return;
				}

				if(playInfoRef.m_uiLoops != HYAUDIO_InfiniteLoops)
					playInfoRef.m_uiLoops--;
			}
			else
			{
				iter = m_PlayMap.erase(iter);
				continue;
			}
		}

		++iter;
	}
}

void HyAudioCore::AddBank(HyFileAudio *pBankFile)
{
	m_BankList.push_back(pBankFile);
}

void HyAudioCore::AddGroup(std::string sName, int32 iId)
{
	SoundGroup *pSndGrp = HY_NEW SoundGroup(sName, iId);
	m_GroupList.push_back(pSndGrp);
	
	if(m_pEngine)
		GroupInit(pSndGrp);
}

void HyAudioCore::GroupInit(SoundGroup *pSndGrp)
{
	ma_result eResult = ma_sound_group_init(m_pEngine, 0, nullptr, &pSndGrp->m_Group);
	if(eResult != MA_SUCCESS)
		HyLogError("HyAudioCore::AddGroup failed: " << eResult);
}

void HyAudioCore::ProcessCue(IHyNode *pNode, HySoundCue eCueType)
{
	switch(eCueType)
	{
	case HYSOUNDCUE_PlayOneShotDefault:
	case HYSOUNDCUE_PlayOneShot: {
		PlayInfo newPlayInfo;
		if(pNode->Is2D())
		{
			if(eCueType == HYSOUNDCUE_PlayOneShot)
			{
				newPlayInfo.m_fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
				newPlayInfo.m_fPitch = static_cast<HyAudio2d *>(pNode)->pitch.Get();
			}
			newPlayInfo.m_uiSoundChecksum = static_cast<HyAudio2d *>(pNode)->PullNextSound();
			newPlayInfo.m_uiLoops = static_cast<HyAudio2d *>(pNode)->GetLoops();
		}
		else
		{
			if(eCueType == HYSOUNDCUE_PlayOneShot)
			{
				newPlayInfo.m_fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
				newPlayInfo.m_fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
			}
			newPlayInfo.m_uiSoundChecksum = static_cast<HyAudio3d *>(pNode)->PullNextSound();
			newPlayInfo.m_uiLoops = static_cast<HyAudio3d *>(pNode)->GetLoops();
		}

		m_OneShotList.push_back(newPlayInfo);

		StartSound(m_OneShotList.back());
		
		break; }

	case HYSOUNDCUE_Start: {
		PlayInfo *pPlayInfo = nullptr;
		if(pNode->Is2D())
		{
			HyAudioNodeHandle hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
			if(m_PlayMap.count(hHandle) == 0)
				m_PlayMap[hHandle] = PlayInfo();
			pPlayInfo = &m_PlayMap[hHandle];

			pPlayInfo->m_fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
			pPlayInfo->m_fPitch = static_cast<HyAudio2d *>(pNode)->pitch.Get();
			pPlayInfo->m_uiSoundChecksum = static_cast<HyAudio2d *>(pNode)->PullNextSound();
			pPlayInfo->m_uiLoops = static_cast<HyAudio2d *>(pNode)->GetLoops();
		}
		else
		{
			HyAudioNodeHandle hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
			if(m_PlayMap.count(hHandle) == 0)
				m_PlayMap[hHandle] = PlayInfo();
			pPlayInfo = &m_PlayMap[hHandle];

			pPlayInfo->m_fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
			pPlayInfo->m_fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
			pPlayInfo->m_uiSoundChecksum = static_cast<HyAudio3d *>(pNode)->PullNextSound();
			pPlayInfo->m_uiLoops = static_cast<HyAudio3d *>(pNode)->GetLoops();
		}

		StartSound(*pPlayInfo);
		break; }

	case HYSOUNDCUE_Stop:
	case HYSOUNDCUE_Pause:
	case HYSOUNDCUE_Unpause: {
		HyAudioNodeHandle hHandle = HY_UNUSED_HANDLE;
		if(pNode->Is2D())
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
		else
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
		
		if(m_PlayMap.count(hHandle) == 0)
			break;
		
		if(eCueType == HYSOUNDCUE_Stop)
			StopSound(m_PlayMap[hHandle]);
		else if(eCueType == HYSOUNDCUE_Pause)
			PauseSound(m_PlayMap[hHandle]);
		else if(eCueType == HYSOUNDCUE_Unpause)
			UnpauseSound(m_PlayMap[hHandle]);
		break; }

	case HYSOUNDCUE_Attributes: {
		HyAudioNodeHandle hHandle = HY_UNUSED_HANDLE;
		float fVolume = 1.0f;
		float fPitch = 0.0f;
		if(pNode->Is2D())
		{
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
			fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
			fPitch = static_cast<HyAudio2d *>(pNode)->pitch.Get();
		}
		else
		{
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
			fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
			fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
		}

		if(m_PlayMap.count(hHandle) == 0)
			break;

		m_PlayMap[hHandle].m_fVolume = fVolume;
		m_PlayMap[hHandle].m_fPitch = fPitch;

		ManipSound(m_PlayMap[hHandle]);
		break; }

	default:
		HyLogError("Unknown sound cue type");
		break;
	}
}

void HyAudioCore::StartSound(PlayInfo &playInfoRef)
{
	// Determine the sound buffer
	HySoundBuffers *pBuffer = nullptr;
	for(auto *pBank : m_BankList)
	{
		pBuffer = static_cast<HySoundBuffers *>(pBank->GetSound(playInfoRef.m_uiSoundChecksum));
		if(pBuffer)
			break;
	}
	if(pBuffer == nullptr)
	{
		// Not found in standard assets, check if it was hotloaded
		for(auto &hotLoad : m_HotLoadMap)
		{
			// When looking through hotloads, the checksum is assigned to the same value as its HyAudioHandle
			if(hotLoad.first == playInfoRef.m_uiSoundChecksum)
			{
				pBuffer = hotLoad.second;
				break;
			}
		}
		if(pBuffer == nullptr)
		{
			HyLogWarning("HyAudioCore::StartPlay() Could not find audio: " << playInfoRef.m_uiSoundChecksum);
			return;
		}
	}

	playInfoRef.m_pSound = pBuffer->GetFreshBuffer();
	if(playInfoRef.m_pSound)
	{
		ma_result eResult = ma_sound_start(playInfoRef.m_pSound);
		if(eResult != MA_SUCCESS)
			HyLogError("ma_sound_start failed: " << eResult);
	}
	else
	{
		HyLogWarning("No available audio instance for: " << pBuffer->GetFilePath());
		return;
	}
}

void HyAudioCore::StopSound(PlayInfo &playInfoRef)
{
	ma_result eResult = ma_sound_stop(playInfoRef.m_pSound);
	if(eResult != MA_SUCCESS)
	{
		HyLogError("ma_sound_stop failed: " << eResult);
		return;
	}

	eResult = ma_sound_seek_to_pcm_frame(playInfoRef.m_pSound, 0); // Rewind sound to beginning
	if(eResult != MA_SUCCESS)
	{
		HyLogError("ma_sound_seek_to_pcm_frame failed: " << eResult);
		return;
	}
}

void HyAudioCore::PauseSound(PlayInfo &playInfoRef)
{
	ma_result eResult = ma_sound_stop(playInfoRef.m_pSound); // When a sound is ma_sound_stop()'ed, it is not rewound to the start
	if(eResult != MA_SUCCESS)
		HyLogError("ma_sound_stop failed: " << eResult);
}

void HyAudioCore::UnpauseSound(PlayInfo &playInfoRef)
{
	ma_result eResult = ma_sound_start(playInfoRef.m_pSound); // When a sound is ma_sound_stop()'ed, it is not rewound to the start
	if(eResult != MA_SUCCESS)
		HyLogError("ma_sound_start failed: " << eResult);
}

void HyAudioCore::ManipSound(PlayInfo &playInfoRef)
{
	ma_sound_set_volume(playInfoRef.m_pSound, playInfoRef.m_fVolume);
	ma_sound_set_pitch(playInfoRef.m_pSound, playInfoRef.m_fPitch);
}

/*static*/ void HyAudioCore::DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
	// In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
	// pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than frameCount frames.
	HyAudioCore *pThis = reinterpret_cast<HyAudioCore *>(pDevice->pUserData);
	ma_engine_read_pcm_frames(pThis->m_pEngine, pOutput, frameCount, nullptr);
}

// Data Callbacks
#ifdef HY_USE_SDL2_AUDIO
	/*friend*/ void SdlDataCallback(void *pUserData, Uint8 *pBuffer, int bufferSizeInBytes)
	{
		HyAudioCore *pThis = reinterpret_cast<HyAudioCore *>(pUserData);

		// Reading for SDL is just a matter of reading straight from the engine.
		ma_uint32 bufferSizeInFrames = (ma_uint32)bufferSizeInBytes / ma_get_bytes_per_frame(ma_format_f32, ma_engine_get_channels(pThis->m_pEngine));
		ma_engine_read_pcm_frames(pThis->m_pEngine, pBuffer, bufferSizeInFrames, NULL);
	}
#endif
