/**************************************************************************
 *	HyAudio_SDL2.cpp
 *
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/Harness/SDL2/HyAudio_SDL2.h"
#include "Diagnostics/Console/HyConsole.h"

#if defined(HY_USE_SDL2)

HyAudio_SDL2::HyAudio_SDL2()
{
	int32 iNumDevices = SDL_GetNumAudioDevices(0);
	for(uint32 i = 0; i < iNumDevices; ++i)
		m_sDeviceList.push_back(SDL_GetAudioDeviceName(i, 0));

	m_Spec.freq = 48000;							// 44100 or 48000
#if defined(HY_ENDIAN_LITTLE)
	m_Spec.format = AUDIO_F32LSB;
#else
	m_Spec.format = AUDIO_F32MSB;
#endif
	m_Spec.channels = 2;							// 1 mono, 2 stereo, 4 quad, 6 (5.1)
	m_Spec.samples = 4096;							// Specifies a unit of audio data to be used at a time. Must be a power of 2
	m_Spec.callback = HyAudio_SDL2::OnCallback;
	m_Spec.userdata = nullptr;

	/*
	* Note: If you're having issues with Emscripten / EMCC play around with these flags
	*
	* 0                                    Allow no changes
	* SDL_AUDIO_ALLOW_FREQUENCY_CHANGE     Allow frequency changes (e.g. AUDIO_FREQUENCY is 48k, but allow files to play at 44.1k
	* SDL_AUDIO_ALLOW_FORMAT_CHANGE        Allow Format change (e.g. AUDIO_FORMAT may be S32LSB, but allow wave files of S16LSB to play)
	* SDL_AUDIO_ALLOW_CHANNELS_CHANGE      Allow any number of channels (e.g. AUDIO_CHANNELS being 2, allow actual 1)
	* SDL_AUDIO_ALLOW_ANY_CHANGE           Allow all changes above
	*/
	m_hDevice = SDL_OpenAudioDevice(nullptr, 0, &m_Spec, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE);
	if(m_hDevice == 0)
	{
		HyLogError("SDL_OpenAudioDevice failed: " << SDL_GetError());
	}
}

/*virtual*/ HyAudio_SDL2::~HyAudio_SDL2(void)
{
	if(m_hDevice > 0)
		SDL_CloseAudioDevice(m_hDevice);
}

const char *HyAudio_SDL2::GetAudioDriver()
{
	return SDL_GetCurrentAudioDriver();
}

/*virtual*/ void HyAudio_SDL2::OnUpdate() /*override*/
{
}

/*static*/ void HyAudio_SDL2::OnCallback(void *pUserData, uint8_t *pStream, int32 iLen)
{
	//Audio *audio = (Audio *)userdata;
	//Audio *previous = audio;
	//int tempLength;
	//uint8_t music = 0;

	///* Silence the main buffer */
	//SDL_memset(pStream, 0, iLen);

	///* First one is place holder */
	//audio = audio->next;

	//while(audio != NULL)
	//{
	//	if(audio->length > 0)
	//	{
	//		if(audio->fade == 1 && audio->loop == 1)
	//		{
	//			music = 1;

	//			if(audio->volume > 0)
	//			{
	//				audio->volume--;
	//			}
	//			else
	//			{
	//				audio->length = 0;
	//			}
	//		}

	//		if(music && audio->loop == 1 && audio->fade == 0)
	//		{
	//			tempLength = 0;
	//		}
	//		else
	//		{
	//			tempLength = ((uint32_t)iLen > audio->length) ? audio->length : (uint32_t)iLen;
	//		}

	//		SDL_MixAudioFormat(pStream, audio->buffer, AUDIO_FORMAT, tempLength, audio->volume);

	//		audio->buffer += tempLength;
	//		audio->length -= tempLength;

	//		previous = audio;
	//		audio = audio->next;
	//	}
	//	else if(audio->loop == 1 && audio->fade == 0)
	//	{
	//		audio->buffer = audio->bufferTrue;
	//		audio->length = audio->lengthTrue;
	//	}
	//	else
	//	{
	//		previous->next = audio->next;

	//		if(audio->loop == 0)
	//		{
	//			gSoundCount--;
	//		}

	//		audio->next = NULL;
	//		freeAudio(audio);

	//		audio = previous->next;
	//	}
	//}
}

#endif // defined(HY_USE_SDL2)
