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
#include "Assets/HyAssets.h"
#include "Diagnostics/Console/IHyConsole.h"

// stb_vorbis implementation must come after the implementation of miniaudio
#undef STB_VORBIS_HEADER_ONLY
#include "vendor/stb/stb_vorbis.c"

#ifdef HY_USE_SDL2_AUDIO
	extern void SdlDataCallback(void *pUserData, Uint8 *pBuffer, int bufferSizeInBytes);
#endif

HyAudioCore::HyAudioCore(HyInput &inputRef) :
	m_InputRef(inputRef),
	m_pAssets(nullptr),
	m_pEngine(nullptr),
	m_fDeferredGlobalVolume(1.0f)
{
	InitDevice();
}

/*virtual*/ HyAudioCore::~HyAudioCore(void)
{
	for(auto *pCategory : m_CategoryList)
		delete pCategory;

	ma_engine_uninit(m_pEngine);
	delete m_pEngine;
	m_pEngine = nullptr;

#ifdef HY_USE_SDL2_AUDIO
	SDL_CloseAudioDevice(m_SdlDeviceId);
#endif
}

void HyAudioCore::SetHyAssetsPtr(HyAssets *pAssets)
{
	m_pAssets = pAssets;
}

void HyAudioCore::InitDevice()
{
#ifdef HY_PLATFORM_GUI
	return;
#endif

#ifdef HY_PLATFORM_BROWSER
	// Google has implemented a policy in their browsers that prevent automatic media output without first receiving some kind of user input
	// Starting the device may fail if you try to start playback without first handling some kind of user input
	if(m_pEngine || m_InputRef.UserInputOccured() == false)
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

void HyAudioCore::StartDevice()
{
	if(m_pEngine == nullptr)
		return;

#ifdef HY_USE_SDL2_AUDIO
	SDL_PauseAudioDevice(m_SdlDeviceId, 0);
#else
	ma_result eResult = ma_engine_start(m_pEngine);
	if(eResult != MA_SUCCESS)
		HyLogError("HyAudioCore::StartDevice failed: " << eResult);
#endif
}

void HyAudioCore::StopDevice()
{
	if(m_pEngine == nullptr)
		return;

#ifdef HY_USE_SDL2_AUDIO
	SDL_PauseAudioDevice(m_SdlDeviceId, 1);
#else
	ma_result eResult = ma_engine_stop(m_pEngine);
	if(eResult != MA_SUCCESS)
		HyLogError("HyAudioCore::StopDevice failed: " << eResult);
#endif
	
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

ma_sound_group *HyAudioCore::GetCategory(int32 iId)
{
	for(auto *pCategory : m_CategoryList)
	{
		if(pCategory->m_iID == iId)
			return &pCategory->m_Group;
	}

	return nullptr;
}

bool HyAudioCore::IsPlaying(HyAudioNodeHandle hHandle)
{
	if(m_pEngine == nullptr)
		return false;

	return m_PlayMap.count(hHandle) > 0;
}

float HyAudioCore::GetElapsedPlayTime(HyAudioNodeHandle hHandle)
{
	if(m_pEngine == nullptr || m_PlayMap.count(hHandle) == 0)
		return 0.0f;

	uint64 uiFramesPlayed;
	ma_result eResult = ma_sound_get_cursor_in_pcm_frames(m_PlayMap[hHandle].m_pSoundBuffer, &uiFramesPlayed);
	if(eResult != MA_SUCCESS)
		HyLogError("ma_sound_get_cursor_in_pcm_frames failed: " << eResult);

	return static_cast<float>(uiFramesPlayed) / ma_engine_get_sample_rate(m_pEngine);
}

//float HyAudioCore::GetGlobalVolume() const
//{
//	if(m_pEngine == nullptr)
//		return 0.0f;
//	return ma_engine_get_volume(m_pEngine);
//}

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
void HyAudioCore::DeferLoading(HySoundAsset *pBuffer)
{
	m_DeferredLoadingList.push_back(pBuffer);
}
#endif

//HyExternalAudioHandle HyAudioCore::LoadExternalFile(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit)
//{
//	HySoundAsset *pNewSndAsset = HY_NEW HySoundAsset(*this, sFilePath, 0, bIsStreaming, iInstanceLimit);
//	pNewSndAsset->Load();
//	
//	m_uiExternalFileCount++;
//	m_ExternalFileMap.insert({ static_cast<HyExternalAudioHandle>(m_uiExternalFileCount), pNewSndAsset });
//	
//	return static_cast<HyExternalAudioHandle>(m_uiExternalFileCount);
//}
//
//void HyAudioCore::UnloadExternalFile(HyExternalAudioHandle hAudioHandle)
//{
//	if(m_ExternalFileMap.count(hAudioHandle) == 0)
//		return;
//
//	delete m_ExternalFileMap[hAudioHandle];
//	m_ExternalFileMap.erase(hAudioHandle);
//}

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
		if(ma_sound_at_end(playInfoRef.m_pSoundBuffer))
		{
			if(playInfoRef.m_uiLoopsRemaining > 0)
			{
				ma_result eResult = ma_sound_start(playInfoRef.m_pSoundBuffer); // Will implicitly restart the sound
				if(eResult != MA_SUCCESS)
				{
					HyLogError("ma_sound_start failed: " << eResult);
					return;
				}

				if(playInfoRef.m_uiLoopsRemaining != HYAUDIO_InfiniteLoops)
					playInfoRef.m_uiLoopsRemaining--;
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

void HyAudioCore::AddCategory(std::string sName, int32 iId)
{
	SoundCategory *pSndCategory = HY_NEW SoundCategory(sName, iId);
	m_CategoryList.push_back(pSndCategory);
	
	if(m_pEngine)
		CategoryInit(pSndCategory);
}

void HyAudioCore::CategoryInit(SoundCategory *pSndCategory)
{
	ma_result eResult = ma_sound_group_init(m_pEngine, 0, nullptr, &pSndCategory->m_Group);
	if(eResult != MA_SUCCESS)
		HyLogError("HyAudioCore::AddGroup failed: " << eResult);
}

void HyAudioCore::ProcessCue(IHyNode *pNode, HySoundCue eCueType)
{
	HyAudioNodeHandle hHandle = HY_UNUSED_HANDLE;

	switch(eCueType)
	{
	case HYSOUNDCUE_PlayOneShotDefault:
	case HYSOUNDCUE_PlayOneShot: {
		float fVolume = 1.0f;
		float fPitch = 0.0f;
		HyAudioHandle hSound(0, 0);
		PlayInfo tempPlayInfo;
		if(pNode->Is2D())
		{
			if(eCueType == HYSOUNDCUE_PlayOneShot)
			{
				fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
				fPitch = static_cast<HyAudio2d *>(pNode)->pitch.Get();
			}
			hSound = static_cast<HyAudio2d *>(pNode)->PullNextSound();
		}
		else
		{
			if(eCueType == HYSOUNDCUE_PlayOneShot)
			{
				fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
				fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
			}
			hSound = static_cast<HyAudio3d *>(pNode)->PullNextSound();
		}

		tempPlayInfo.m_pSoundBuffer = FindIdleBuffer(hSound);
		if(tempPlayInfo.m_pSoundBuffer)
		{
			ma_sound_set_volume(tempPlayInfo.m_pSoundBuffer, fVolume);
			ma_sound_set_pitch(tempPlayInfo.m_pSoundBuffer, fPitch);
			ma_sound_seek_to_pcm_frame(tempPlayInfo.m_pSoundBuffer, 0); // Rewind to beginning

			ma_result eResult = ma_sound_start(tempPlayInfo.m_pSoundBuffer);
			if(eResult != MA_SUCCESS)
				HyLogError("ma_sound_start failed: " << eResult);
		}

		break; }

	case HYSOUNDCUE_Unpause:
		if(pNode->Is2D())
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
		else
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
		if(m_PlayMap.count(hHandle) == 0 || m_PlayMap[hHandle].m_pSoundBuffer != nullptr)
			break;
		[[fallthrough]];
	case HYSOUNDCUE_Start: {
		float fVolume = 1.0f;
		float fPitch = 0.0f;
		HyAudioHandle hSound(0, 0);
		PlayInfo *pPlayInfo = nullptr;
		if(pNode->Is2D())
		{
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
			if(m_PlayMap.find(hHandle) == m_PlayMap.end())
				m_PlayMap[hHandle] = PlayInfo();
			pPlayInfo = &m_PlayMap[hHandle];

			fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
			fPitch = static_cast<HyAudio2d *>(pNode)->pitch.Get();
			hSound = static_cast<HyAudio2d *>(pNode)->PullNextSound();
			pPlayInfo->m_uiLoopsRemaining = static_cast<HyAudio2d *>(pNode)->GetLoops();
		}
		else
		{
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
			if(m_PlayMap.find(hHandle) == m_PlayMap.end())
				m_PlayMap[hHandle] = PlayInfo();
			pPlayInfo = &m_PlayMap[hHandle];

			fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
			fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
			hSound = static_cast<HyAudio3d *>(pNode)->PullNextSound();
			pPlayInfo->m_uiLoopsRemaining = static_cast<HyAudio3d *>(pNode)->GetLoops();
		}

		if(pPlayInfo->m_pSoundBuffer == nullptr)
			pPlayInfo->m_pSoundBuffer = FindIdleBuffer(hSound);

		if(pPlayInfo->m_pSoundBuffer)
		{
			ma_sound_set_volume(pPlayInfo->m_pSoundBuffer, fVolume);
			ma_sound_set_pitch(pPlayInfo->m_pSoundBuffer, fPitch);

			if(eCueType == HYSOUNDCUE_Start)
				ma_sound_seek_to_pcm_frame(pPlayInfo->m_pSoundBuffer, 0); // Rewind to beginning
			else // HYSOUNDCUE_Unpause
				ma_sound_seek_to_pcm_frame(pPlayInfo->m_pSoundBuffer, pPlayInfo->m_uiPauseFrame);

			ma_result eResult = ma_sound_start(pPlayInfo->m_pSoundBuffer);
			if(eResult != MA_SUCCESS)
				HyLogError("ma_sound_start failed: " << eResult);
		}
		else
			m_PlayMap.erase(hHandle);
		break; }

	case HYSOUNDCUE_DeleteInstance:
	case HYSOUNDCUE_Stop:
		if(pNode->Is2D())
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
		else
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
		if(m_PlayMap.count(hHandle) == 0)
			break;

		if(m_PlayMap[hHandle].m_pSoundBuffer) // Might be null if a Pause() was invoked prior
		{
			ma_result eResult = ma_sound_stop(m_PlayMap[hHandle].m_pSoundBuffer);
			if(eResult != MA_SUCCESS)
				HyLogError("ma_sound_stop failed: " << eResult);
		}

		m_PlayMap.erase(hHandle);
		break;

	case HYSOUNDCUE_Pause:
		if(pNode->Is2D())
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
		else
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
		if(m_PlayMap.count(hHandle) == 0)
			break;

		if(m_PlayMap[hHandle].m_pSoundBuffer) // Might be null if sequencial Pause()'s are called
		{
			// When a sound is ma_sound_stop()'ed, it is NOT rewound to the start
			ma_result eResult = ma_sound_stop(m_PlayMap[hHandle].m_pSoundBuffer);
			if(eResult != MA_SUCCESS)
				HyLogError("ma_sound_stop failed: " << eResult);

			// Store the current 'm_PlayMap[hHandle].m_uiPauseFrame'
			eResult = ma_sound_get_cursor_in_pcm_frames(m_PlayMap[hHandle].m_pSoundBuffer, &m_PlayMap[hHandle].m_uiPauseFrame);
			if(eResult != MA_SUCCESS)
				HyLogError("ma_sound_get_cursor_in_pcm_frames failed: " << eResult);

			m_PlayMap[hHandle].m_pSoundBuffer = nullptr; // This sound buffer is free to be used elsewhere since it's not playing anymore
		}
		break;

	case HYSOUNDCUE_Attributes: {
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
		if(m_PlayMap.count(hHandle) == 0 || m_PlayMap[hHandle].m_pSoundBuffer == nullptr)
			break;

		ma_sound_set_volume(m_PlayMap[hHandle].m_pSoundBuffer, fVolume);
		ma_sound_set_pitch(m_PlayMap[hHandle].m_pSoundBuffer, fPitch);
		break; }

	default:
		HyLogError("Unknown sound cue type");
		break;
	}
}

ma_sound *HyAudioCore::FindIdleBuffer(HyAudioHandle hHandle)
{
	HyAssert(m_pAssets, "HyAudioCore::FindIdleBuffer() 'm_pAssets' hasn't been initialized");

	HyFileAudio *pAudioFile = nullptr;
	uint32 uiChecksum = 0;
	if(hHandle.first != 0)
	{
		pAudioFile = static_cast<HyFileAudio *>(m_pAssets->GetFileWithAsset(HYFILE_AudioBank, hHandle.first, hHandle.second));
		uiChecksum = hHandle.first;
	}
	else
	{
		pAudioFile = static_cast<HyFileAudio *>(m_pAssets->GetAuxiliaryFile(hHandle.second));
		uiChecksum = hHandle.second;
	}
	if(pAudioFile == nullptr)
	{
		HyLogError("HyAudioCore::FindIdleBuffer() Could not find audio asset with handle: " << hHandle.first << ", " << hHandle.second);
		return nullptr;
	}

	// Determine the sound buffer
	HySoundAsset *pBuffer = pAudioFile->GetSoundAsset(uiChecksum);
	if(pBuffer == nullptr)
	{
		HyLogError("HyAudioCore::FindIdleBuffer() Could not find any sound with checksum: " << uiChecksum);
		return nullptr;
	}

	return pBuffer->GetIdleBuffer();
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
