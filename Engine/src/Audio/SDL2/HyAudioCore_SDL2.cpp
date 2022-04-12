/**************************************************************************
 *	HyAudioCore_SDL2.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Audio/SDL2/HyAudioCore_SDL2.h"
#include "Audio/SDL2/HyFileAudioImpl_SDL2.h"
#include "Utilities/HyMath.h"

#if defined(HY_USE_SDL2)

#ifdef HY_PLATFORM_BROWSER
	#include <SDL2/SDL_mixer.h>
#else
	#include "SDL_mixer.h"
#endif

// SDL maps exactly the % of the volume meter in Window's Volume Mixer. But most other programs never approach 
// this maximum (it's loud as hell), even when you max out their volume dial. Provide the maximum volume allowed in 'HYAUDIO_SDL_VOLUME_DAMPENING'
#define HYAUDIO_SDL_VOLUME_DAMPENING 0.75f

HyAudioCore_SDL2::HyAudioCore_SDL2()
{
	m_fGlobalSfxVolume = HYAUDIO_SDL_VOLUME_DAMPENING;
	m_fGlobalMusicVolume = HYAUDIO_SDL_VOLUME_DAMPENING;

	int32 iNumDevices = SDL_GetNumAudioDevices(0);
	for(int32 i = 0; i < iNumDevices; ++i)
		m_sDeviceList.push_back(SDL_GetAudioDeviceName(i, 0));

	m_iDesiredFrequency = 48000;
#if defined(HY_ENDIAN_LITTLE)
	m_uiDesiredFormat = AUDIO_S16LSB;//AUDIO_F32LSB;
#else
	m_uiDesiredFormat = AUDIO_S16MSB;//AUDIO_F32MSB;
#endif
	m_iDesiredNumChannels = 2;
	m_iDesiredSamples = 4096;

	/*
	* Note: If you're having issues with Emscripten / EMCC play around with these flags
	*
	* 0                                    Allow no changes
	* SDL_AUDIO_ALLOW_FREQUENCY_CHANGE     Allow frequency changes (e.g. AUDIO_FREQUENCY is 48k, but allow files to play at 44.1k
	* SDL_AUDIO_ALLOW_FORMAT_CHANGE        Allow Format change (e.g. AUDIO_FORMAT may be S32LSB, but allow wave files of S16LSB to play)
	* SDL_AUDIO_ALLOW_CHANNELS_CHANGE      Allow any number of channels (e.g. AUDIO_CHANNELS being 2, allow actual 1)
	* SDL_AUDIO_ALLOW_ANY_CHANGE           Allow all changes above
	*/
	if(Mix_OpenAudioDevice(m_iDesiredFrequency, m_uiDesiredFormat, m_iDesiredNumChannels, m_iDesiredSamples, nullptr, 0) != 0)
	{
		HyLogError("Mix_OpenAudioDevice failed: " << Mix_GetError());
		return;
	}

	// Preload support for OGG
	int iFlags = MIX_INIT_OGG;
	int iInitted = Mix_Init(iFlags);
	if((iInitted & iFlags) != iFlags)
	{
		HyLogError("Mix_Init: Failed to init required ogg support!\n");
		HyLogError("Mix_Init: " << Mix_GetError());
	}

	// Print audio information to log
	const SDL_version *pLinkVersion = Mix_Linked_Version();
	SDL_version compiledVersion;
	SDL_MIXER_VERSION(&compiledVersion);
	HyLog("Compiled SDL_mixer version: " << compiledVersion.major << "." << compiledVersion.minor << "." << compiledVersion.patch);
	HyLog("Linked SDL_mixer version: " << pLinkVersion->major << "." << pLinkVersion->minor << "." << pLinkVersion->patch);

	int iFrequency, iChannels;
	Uint16 uiFormat;
	int iNumTimesOpened = Mix_QuerySpec(&iFrequency, &uiFormat, &iChannels);
	if(!iNumTimesOpened)
		HyLogError("Mix_QuerySpec failed: " << Mix_GetError());
	else
	{
		const char *szFormat = "Unknown";
		switch(uiFormat)
		{
			case AUDIO_U8:		szFormat="U8";		break;
			case AUDIO_S8:		szFormat="S8";		break;
			case AUDIO_U16LSB:	szFormat="U16LSB";	break;
			case AUDIO_S16LSB:	szFormat="S16LSB";	break;
			case AUDIO_U16MSB:	szFormat="U16MSB";	break;
			case AUDIO_S16MSB:	szFormat="S16MSB";	break;
			default:			szFormat="Unknown";	break;
		}
		HyLog("Frequency:        " << iFrequency << "Hz");
		HyLog("Format:           " << szFormat);
		HyLog("Channels:         " << iChannels);
	}

	HyLog("Audio Driver:     " << GetAudioDriver());

	// Set API callbacks
	Mix_ChannelFinished(IHyAudioCore::OnReportFinished);

	int32 iNumChannels = Mix_AllocateChannels(HYMAX_AUDIOCHANNELS);
}

/*virtual*/ HyAudioCore_SDL2::~HyAudioCore_SDL2(void)
{
	Mix_CloseAudio();
	Mix_Quit();
}

const char *HyAudioCore_SDL2::GetAudioDriver()
{
	return SDL_GetCurrentAudioDriver();
}

/*virtual*/ IHyFileAudioImpl *HyAudioCore_SDL2::OnAllocateAudioBank(HyJsonObj bankObj) /*override*/
{
	return HY_NEW HyFileAudioImpl_SDL2(bankObj);
}

/*virtual*/ void HyAudioCore_SDL2::OnSetSfxVolume(float fGlobalSfxVolume) /*override*/
{
	m_fGlobalSfxVolume = HyClamp(fGlobalSfxVolume, 0.0f, 1.0f) * HYAUDIO_SDL_VOLUME_DAMPENING;
	
	for(const auto &iter : m_PlayMap)
		Mix_Volume(iter.second.m_iApiData, static_cast<int>(MIX_MAX_VOLUME * iter.second.m_fVolume * m_fGlobalSfxVolume));
	
	for(const auto &iter : m_OneShotList)
		Mix_Volume(iter.m_iApiData, static_cast<int>(MIX_MAX_VOLUME * iter.m_fVolume * m_fGlobalSfxVolume));
}

/*virtual*/ void HyAudioCore_SDL2::OnSetMusicVolume(float fGlobalMusicVolume) /*override*/
{
	m_fGlobalMusicVolume = HyClamp(fGlobalMusicVolume, 0.0f, 1.0f) * HYAUDIO_SDL_VOLUME_DAMPENING;
	Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * m_fGlobalMusicVolume));
}

/*virtual*/ void HyAudioCore_SDL2::OnCue_Play(PlayInfo &playInfoRef) /*override*/
{
	// Determine 'HySdlRawSoundBuffer'
	HySdlRawSoundBuffer *pBuffer = nullptr;
	for(auto file : m_AudioFileList)
	{
		pBuffer = static_cast<HyFileAudioImpl_SDL2 *>(file)->GetBufferInfo(playInfoRef.m_uiSoundChecksum);
		if(pBuffer)
			break;
	}
	if(pBuffer == nullptr)
	{
		HyLogWarning("HyAudioCore_SDL2::OnCue_Play() Could not find audio: " << playInfoRef.m_uiSoundChecksum);
		return;
	}

	// Play in Mixer
	if(pBuffer->IsMusic())
	{
		if(Mix_PlayMusic(pBuffer->GetMusicPtr(), playInfoRef.m_uiLoops) == -1)
		{
			HyLogWarning("HyAudioCore_SDL2::OnCue_Play() Mix_PlayMusic failed on: " << playInfoRef.m_uiSoundChecksum);
			return;
		}

		int32 iVolume = static_cast<int>(MIX_MAX_VOLUME * (playInfoRef.m_fVolume * m_fGlobalMusicVolume));
		Mix_VolumeMusic(iVolume);
	}
	else
	{
		playInfoRef.m_iApiData = Mix_PlayChannelTimed(playInfoRef.m_iApiData, pBuffer->GetSfxPtr(), playInfoRef.m_uiLoops, -1);
		if(playInfoRef.m_iApiData == -1)
		{
			HyLogWarning("HyAudioCore_SDL2::OnCue_Play() Mix_PlayChannelTimed failed on: " << playInfoRef.m_uiSoundChecksum);
			return;
		}

		int32 iVolume = static_cast<int>(MIX_MAX_VOLUME * (playInfoRef.m_fVolume * m_fGlobalSfxVolume));
		Mix_Volume(playInfoRef.m_iApiData, iVolume);
	}
}

/*virtual*/ void HyAudioCore_SDL2::OnCue_Stop(PlayInfo &playInfoRef) /*override*/
{
	Mix_HaltChannel(playInfoRef.m_iApiData);
}

/*virtual*/ void HyAudioCore_SDL2::OnCue_Pause(PlayInfo &playInfoRef) /*override*/
{
	Mix_Pause(playInfoRef.m_iApiData);
}

/*virtual*/ void HyAudioCore_SDL2::OnCue_Unpause(PlayInfo &playInfoRef) /*override*/
{
	Mix_Resume(playInfoRef.m_iApiData);
}

/*virtual*/ void HyAudioCore_SDL2::OnCue_Volume(PlayInfo &playInfoRef) /*override*/
{
	int32 iVolume = static_cast<int>(MIX_MAX_VOLUME * (playInfoRef.m_fVolume * m_fGlobalSfxVolume));
	Mix_Volume(playInfoRef.m_iApiData, iVolume);
}

#endif // defined(HY_USE_SDL2)
