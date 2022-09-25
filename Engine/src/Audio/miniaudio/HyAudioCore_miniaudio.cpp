/**************************************************************************
 *	HyAudioCore_miniaudio.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2022 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Audio/miniaudio/HyAudioCore_miniaudio.h"
#include "Audio/miniaudio/HySoundBuffer_miniaudio.h"

//// SDL maps exactly the % of the volume meter in Window's Volume Mixer. But most other programs never approach 
//// this maximum (it's loud as hell), even when you max out their volume dial. Provide the maximum volume allowed in 'HYAUDIO_SDL_VOLUME_DAMPENING'
//#define HYAUDIO_SDL_VOLUME_DAMPENING 0.75f

HyAudioCore_miniaudio::HyAudioCore_miniaudio()
{
	//m_fGlobalSfxVolume = HYAUDIO_SDL_VOLUME_DAMPENING;
	//m_fGlobalMusicVolume = HYAUDIO_SDL_VOLUME_DAMPENING;

	// Device
	m_DevConfig = ma_device_config_init(ma_device_type_playback);
	//m_DevConfig.playback.pDeviceID = &pPlaybackInfos[chosenPlaybackDeviceIndex].id;
	m_DevConfig.playback.format = ma_format_s16;	// Set to ma_format_unknown to use the device's native format.
	m_DevConfig.playback.channels = 2;				// Set to 0 to use the device's native channel count.
	m_DevConfig.sampleRate = 48000;					// Set to 0 to use the device's native sample rate.
	m_DevConfig.dataCallback = DataCallback;		// This function will be called when miniaudio needs more data.
	m_DevConfig.pUserData = this;					// Can be accessed from the device object (device.pUserData).

	ma_result eResult = ma_device_init(NULL, &m_DevConfig, &m_Device);
	if(eResult != MA_SUCCESS)
	{
		HyLogError("HyAudioCore_miniaudio failed: " << eResult);
		return;  // Failed to initialize the device.
	}
	ma_device_start(&m_Device);     // The device is sleeping by default so you'll need to start it manually.

	// Resource Manager
	m_ResConfig = ma_resource_manager_config_init();
	m_ResConfig.decodedFormat = m_Device.playback.format;
	m_ResConfig.decodedChannels = m_Device.playback.channels;
	m_ResConfig.decodedSampleRate = m_Device.sampleRate;

	eResult = ma_resource_manager_init(&m_ResConfig, &m_ResourceManager);
	if(eResult != MA_SUCCESS)
	{
		ma_device_uninit(&m_Device);
		HyLogError("Failed to initialize the resource manager: " << eResult);
		return;
	}

	// Engine
	m_EngConfig = ma_engine_config_init();
	m_EngConfig.pResourceManager = &m_ResourceManager;
	m_EngConfig.pDevice = &m_Device;

	eResult = ma_engine_init(&m_EngConfig, &m_Engine);
	if(eResult != MA_SUCCESS)
	{
		HyLogError("HyAudioCore_miniaudio failed: " << eResult);  // Failed to initialize the engine.
		return;
	}
}

/*virtual*/ HyAudioCore_miniaudio::~HyAudioCore_miniaudio(void)
{
	ma_device_uninit(&m_Device);
}

const char *HyAudioCore_miniaudio::GetAudioDriver()
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

	//// Loop over each device info and do something with it. Here we just print the name with their index. You may want
	//// to give the user the opportunity to choose which device they'd prefer.
	//for(ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
	//	printf("%d - %s\n", iDevice, pPlaybackInfos[iDevice].name);
	//}
}

ma_engine &HyAudioCore_miniaudio::GetEngine()
{
	return m_Engine;
}

/*virtual*/ void HyAudioCore_miniaudio::OnSetSfxVolume(float fGlobalSfxVolume) /*override*/
{
	m_fGlobalSfxVolume = HyClamp(fGlobalSfxVolume, 0.0f, 1.0f);// *HYAUDIO_SDL_VOLUME_DAMPENING;
	//ma_engine_set_volume
	//ma_sound_group_init(
}

/*virtual*/ void HyAudioCore_miniaudio::OnSetMusicVolume(float fGlobalMusicVolume) /*override*/
{
	m_fGlobalMusicVolume = HyClamp(fGlobalMusicVolume, 0.0f, 1.0f);// *HYAUDIO_SDL_VOLUME_DAMPENING;
	//ma_sound_group_init(
}

/*virtual*/ void HyAudioCore_miniaudio::OnCue_Play(PlayInfo &playInfoRef) /*override*/
{
	// Determine Sound Buffer
	HySoundBuffer_miniaudio *pBuffer = nullptr;
	for(auto file : m_AudioFileList)
	{
		pBuffer = static_cast<HySoundBuffer_miniaudio *>(file->GetSound(playInfoRef.m_uiSoundChecksum));
		if(pBuffer)
			break;
	}
	if(pBuffer == nullptr)
	{
		HyLogWarning("HyAudioCore_SDL2::OnCue_Play() Could not find audio: " << playInfoRef.m_uiSoundChecksum);
		return;
	}

	pBuffer->GetSound();

	HyError("Unfinished");
}

/*virtual*/ void HyAudioCore_miniaudio::OnCue_Stop(PlayInfo &playInfoRef) /*override*/
{
	HyError("Unfinished");
}

/*virtual*/ void HyAudioCore_miniaudio::OnCue_Pause(PlayInfo &playInfoRef) /*override*/
{
	HyError("Unfinished");
}

/*virtual*/ void HyAudioCore_miniaudio::OnCue_Unpause(PlayInfo &playInfoRef) /*override*/
{
	HyError("Unfinished");
}

/*virtual*/ void HyAudioCore_miniaudio::OnCue_Volume(PlayInfo &playInfoRef) /*override*/
{
	HyError("Unfinished");
}

/*static*/ void HyAudioCore_miniaudio::DataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount)
{
	// In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
	// pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than frameCount frames.
	HyAudioCore_miniaudio *pThis = reinterpret_cast<HyAudioCore_miniaudio *>(pDevice->pUserData);
	ma_engine_read_pcm_frames(&pThis->m_Engine, pOutput, frameCount, nullptr);
}
