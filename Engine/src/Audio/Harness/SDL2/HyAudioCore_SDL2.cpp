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
#include "Audio/Harness/SDL2/HyAudioCore_SDL2.h"
#include "Audio/Harness/SDL2/HyFileAudioImpl_SDL2.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio3d.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Utilities/HyMath.h"

#if defined(HY_USE_SDL2)

HyAudioCore_SDL2::HyAudioCore_SDL2()
{
	HyLogTitle("SDL2 Audio");

	int32 iNumDevices = SDL_GetNumAudioDevices(0);
	for(uint32 i = 0; i < iNumDevices; ++i)
		m_sDeviceList.push_back(SDL_GetAudioDeviceName(i, 0));

	m_DesiredSpec.freq = 48000;//44100;							// 44100 or 48000
#if defined(HY_ENDIAN_LITTLE)
	m_DesiredSpec.format = AUDIO_S16LSB;//AUDIO_F32LSB;
#else
	m_DesiredSpec.format = AUDIO_S16MSB;//AUDIO_F32MSB;
#endif
	m_DesiredSpec.channels = 2;							// 1 mono, 2 stereo, 4 quad, 6 (5.1)
	m_DesiredSpec.samples = 4096;						// Specifies a unit of audio data to be used at a time. Must be a power of 2
	m_DesiredSpec.callback = HyAudioCore_SDL2::OnCallback;
	m_DesiredSpec.userdata = this;

	/*
	* Note: If you're having issues with Emscripten / EMCC play around with these flags
	*
	* 0                                    Allow no changes
	* SDL_AUDIO_ALLOW_FREQUENCY_CHANGE     Allow frequency changes (e.g. AUDIO_FREQUENCY is 48k, but allow files to play at 44.1k
	* SDL_AUDIO_ALLOW_FORMAT_CHANGE        Allow Format change (e.g. AUDIO_FORMAT may be S32LSB, but allow wave files of S16LSB to play)
	* SDL_AUDIO_ALLOW_CHANNELS_CHANGE      Allow any number of channels (e.g. AUDIO_CHANNELS being 2, allow actual 1)
	* SDL_AUDIO_ALLOW_ANY_CHANGE           Allow all changes above
	*/
	m_hDevice = SDL_OpenAudioDevice(nullptr, SDL_FALSE, &m_DesiredSpec, nullptr, 0);
	if(m_hDevice == 0)
	{
		HyLogError("SDL_OpenAudioDevice failed: " << SDL_GetError());
		return;
	}

	//HyLog("Default Device:   " << atlasGrpArray.size());
	HyLog("Audio Driver:     " << SDL_GetCurrentAudioDriver());

	// Start audio device
	SDL_PauseAudioDevice(m_hDevice, 0);

	m_PlayList.reserve(15); // TODO: Set to max sound events?
}

/*virtual*/ HyAudioCore_SDL2::~HyAudioCore_SDL2(void)
{
	if(m_hDevice > 0)
		SDL_CloseAudioDevice(m_hDevice);
}

const char *HyAudioCore_SDL2::GetAudioDriver()
{
	return SDL_GetCurrentAudioDriver();
}

/*virtual*/ void HyAudioCore_SDL2::OnUpdate() /*override*/
{
	if(m_CueList.empty())
		return;

	SDL_LockAudioDevice(m_hDevice);
	for(auto cue : m_CueList)
	{
		switch(cue.m_eCUE_TYPE)
		{
		case CUETYPE_PlayOneShotDefault:
		case CUETYPE_PlayOneShot:
		case CUETYPE_Start: {
			uint32 uiSoundChecksum = 0;
			float fVolume = 1.0f;
			float fPitch = 1.0f;
			if(cue.m_pNODE->Is2D())
			{
				uiSoundChecksum = static_cast<const HyAudioData *>(static_cast<HyAudio2d *>(cue.m_pNODE)->AcquireData())->GetSound();
				if(cue.m_eCUE_TYPE != CUETYPE_PlayOneShotDefault)
				{
					fVolume = static_cast<HyAudio2d *>(cue.m_pNODE)->volume.Get();
					fPitch = static_cast<HyAudio2d *>(cue.m_pNODE)->pitch.Get();
				}
			}
			else
			{
				uiSoundChecksum = static_cast<const HyAudioData *>(static_cast<HyAudio3d *>(cue.m_pNODE)->AcquireData())->GetSound();
				if(cue.m_eCUE_TYPE != CUETYPE_PlayOneShotDefault)
				{
					fVolume = static_cast<HyAudio3d *>(cue.m_pNODE)->volume.Get();
					fPitch = static_cast<HyAudio3d *>(cue.m_pNODE)->pitch.Get();
				}
			}

			HyRawSoundBuffer *pBuffer = nullptr;
			for(auto file : m_AudioFileList)
			{
				pBuffer = file->GetBufferInfo(uiSoundChecksum);
				if(pBuffer)
					break;
			}
			if(pBuffer == nullptr)
			{
				HyLogWarning("Could not find audio: " << uiSoundChecksum);
				break;
			}

			bool bFoundPlay = false;
			const IHyNode *pId = nullptr;
			if(cue.m_eCUE_TYPE == CUETYPE_Start)
			{
				// Find any existing play with node ID of cue.m_pNODE
				pId = cue.m_pNODE;

				for(uint32 j = 0; j < static_cast<uint32>(m_PlayList.size()); ++j)
				{
					if(m_PlayList[j].m_pID == pId)
					{
						bFoundPlay = true;

						m_PlayList[j].m_fVolume = fVolume;
						m_PlayList[j].m_fPitch = fPitch;
						m_PlayList[j].m_bPaused = false;
						m_PlayList[j].m_pBuffer = pBuffer;
						m_PlayList[j].m_uiRemainingBytes = pBuffer->GetBufferSize();
					}
				}
			}

			if(bFoundPlay == false)
				m_PlayList.emplace_back(pId, fVolume, fPitch, false, pBuffer, pBuffer->GetBufferSize());
			break; }

		case CUETYPE_Stop:
		case CUETYPE_Pause:
		case CUETYPE_Unpause:
		case CUETYPE_Attributes: {
			for(auto iter = m_PlayList.begin(); iter != m_PlayList.end(); ++iter)
			{
				if(iter->m_pID == cue.m_pNODE)
				{
					if(cue.m_eCUE_TYPE == CUETYPE_Stop)
						m_PlayList.erase(iter);
					else if(cue.m_eCUE_TYPE == CUETYPE_Pause)
						iter->m_bPaused = true;
					else if(cue.m_eCUE_TYPE == CUETYPE_Unpause)
						iter->m_bPaused = false;
					else if(cue.m_eCUE_TYPE == CUETYPE_Attributes)
					{
						if(cue.m_pNODE->Is2D())
						{
							iter->m_fVolume = static_cast<HyAudio2d *>(cue.m_pNODE)->volume.Get();
							iter->m_fPitch = static_cast<HyAudio2d *>(cue.m_pNODE)->pitch.Get();
						}
						else
						{
							iter->m_fVolume = static_cast<HyAudio3d *>(cue.m_pNODE)->volume.Get();
							iter->m_fPitch = static_cast<HyAudio3d *>(cue.m_pNODE)->pitch.Get();
						}
					}

					break;
				}
			}
			
			break; }

		default:
			HyLogError("Unknown sound cue type");
			break;
		}
	}
	SDL_UnlockAudioDevice(m_hDevice);

	m_CueList.clear();
}

/*static*/ void HyAudioCore_SDL2::OnCallback(void *pUserData, uint8_t *pStream, int32 iLen)
{
	HyAudioCore_SDL2 *pThis = reinterpret_cast<HyAudioCore_SDL2 *>(pUserData);
	SDL_memset(pStream, 0, iLen); // If there is nothing to play, this callback should fill the buffer with silence

	for(auto iter = pThis->m_PlayList.begin(); iter != pThis->m_PlayList.end();)
	{
		uint32 uiLength = (static_cast<uint32_t>(iLen) > iter->m_uiRemainingBytes) ? iter->m_uiRemainingBytes : static_cast<uint32_t>(iLen);
		int iVolume = SDL_MIX_MAXVOLUME * iter->m_fVolume;

		SDL_MixAudioFormat(pStream, iter->m_pBuffer->GetBuffer(iter->m_uiRemainingBytes), iter->m_pBuffer->GetFormat(), uiLength, HyClamp(iVolume, 0, SDL_MIX_MAXVOLUME));
		iter->m_uiRemainingBytes -= uiLength;
		if(iter->m_uiRemainingBytes == 0)
			iter = pThis->m_PlayList.erase(iter);
		else
			++iter;
	}
}

/*static*/ IHyFileAudioImpl *HyAudioCore_SDL2::AllocateBank(IHyAudioCore *pAudio, const jsonxx::Object &bankObjRef)
{
	HyFileAudioImpl_SDL2 *pNewFileGuts = HY_NEW HyFileAudioImpl_SDL2(bankObjRef);
	static_cast<HyAudioCore_SDL2 *>(pAudio)->m_AudioFileList.push_back(pNewFileGuts);

	return pNewFileGuts;
}

#endif // defined(HY_USE_SDL2)
