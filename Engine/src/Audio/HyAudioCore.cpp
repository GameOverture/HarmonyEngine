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
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio3d.h"
#include "Diagnostics/Console/IHyConsole.h"

// stb_vorbis implementation must come after the implementation of miniaudio
#undef STB_VORBIS_HEADER_ONLY
#include "vendor/stb/stb_vorbis.c"

HyAudioCore::HyAudioCore() :
	m_uiHotLoadCount(1)
{
	// Device
	//m_DevConfig = ma_device_config_init(ma_device_type_playback);
	////m_DevConfig.playback.pDeviceID = &pPlaybackInfos[chosenPlaybackDeviceIndex].id;
	//m_DevConfig.playback.format = ma_format_s16;	// Set to ma_format_unknown to use the device's native format.
	//m_DevConfig.playback.channels = 2;				// Set to 0 to use the device's native channel count.
	//m_DevConfig.sampleRate = 48000;					// Set to 0 to use the device's native sample rate.
	//m_DevConfig.dataCallback = DataCallback;		// This function will be called when miniaudio needs more data.
	//m_DevConfig.pUserData = this;					// Can be accessed from the device object (device.pUserData).

	ma_result eResult;
	//eResult = ma_device_init(NULL, &m_DevConfig, &m_Device);
	//if(eResult != MA_SUCCESS)
	//{
	//	HyLogError("ma_device_init failed: " << eResult);
	//	return;  // Failed to initialize the device.
	//}
	//eResult = ma_device_start(&m_Device);     // The device is sleeping by default so you'll need to start it manually.
	//if(eResult != MA_SUCCESS)
	//{
	//	HyLogError("ma_device_start failed: " << eResult);
	//	return;  // Failed to initialize the device.
	//}

	// Resource Manager
	//m_ResConfig = ma_resource_manager_config_init();
	//m_ResConfig.decodedFormat = m_Device.playback.format;
	//m_ResConfig.decodedChannels = m_Device.playback.channels;
	//m_ResConfig.decodedSampleRate = m_Device.sampleRate;

	//eResult = ma_resource_manager_init(&m_ResConfig, &m_ResourceManager);
	//if(eResult != MA_SUCCESS)
	//{
	//	ma_device_uninit(&m_Device);
	//	HyLogError("Failed to initialize the resource manager: " << eResult);
	//	return;
	//}

	// Engine
	m_EngConfig = ma_engine_config_init();
	//m_EngConfig.pResourceManager = &m_ResourceManager;
	//m_EngConfig.pDevice = &m_Device;

	eResult = ma_engine_init(&m_EngConfig, &m_Engine);
	if(eResult != MA_SUCCESS)
	{
		HyLogError("HyAudioCore_miniaudio failed: " << eResult);  // Failed to initialize the engine.
		return;
	}
}

/*virtual*/ HyAudioCore::~HyAudioCore(void)
{
	for(auto *pGrp : m_GroupList)
		delete pGrp;

	//ma_device_uninit(&m_Device);
	ma_engine_uninit(&m_Engine);
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

	//// Loop over each device info and do something with it. Here we just print the name with their index. You may want
	//// to give the user the opportunity to choose which device they'd prefer.
	//for(ma_uint32 iDevice = 0; iDevice < playbackCount; iDevice += 1) {
	//	printf("%d - %s\n", iDevice, pPlaybackInfos[iDevice].name);
	//}
}

ma_engine *HyAudioCore::GetEngine()
{
	return &m_Engine;
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
	ma_result eResult = ma_engine_set_volume(&m_Engine, fVolume);
	if(eResult != MA_SUCCESS)
		HyLogError("ma_engine_set_volume failed: " << eResult);
}

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

void HyAudioCore::AddBank(HyFileAudio *pBankFile)
{
	m_BankList.push_back(pBankFile);
}

void HyAudioCore::AddGroup(std::string sName, int32 iId)
{
	m_GroupList.push_back(HY_NEW SoundGroup(sName, iId));
	ma_result eResult = ma_sound_group_init(&m_Engine, 0, nullptr, &m_GroupList.back()->m_Group);
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
	ma_result eResult = ma_sound_stop(playInfoRef.m_pSound); // When a sound is stopped, it is not rewound to the start
	if(eResult != MA_SUCCESS)
		HyLogError("ma_sound_stop failed: " << eResult);
}

void HyAudioCore::UnpauseSound(PlayInfo &playInfoRef)
{
	ma_result eResult = ma_sound_start(playInfoRef.m_pSound); // When a sound is stopped, it is not rewound to the start
	if(eResult != MA_SUCCESS)
		HyLogError("ma_sound_stop failed: " << eResult);
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
	ma_engine_read_pcm_frames(&pThis->m_Engine, pOutput, frameCount, nullptr);
}
