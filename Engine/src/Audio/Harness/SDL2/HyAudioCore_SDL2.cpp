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
#include "Audio/Harness/SDL2/HyAudioCore_SDL2.h"
#include "Audio/Harness/SDL2/HyFileAudioImpl_SDL2.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio3d.h"
#include "Diagnostics/Console/HyConsole.h"
#include "Utilities/HyMath.h"

#if defined(HY_USE_SDL2)

#include "SDL_mixer.h"

HyAudioCore_SDL2 *HyAudioCore_SDL2::sm_pInstance = nullptr;

HyAudioCore_SDL2::HyAudioCore_SDL2()
{
	HyLogTitle("SDL2 Audio");

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
	if(!iNumTimesOpened) {
		HyLogError("Mix_QuerySpec failed: " << Mix_GetError());
	}
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
	Mix_ChannelFinished(HyAudioCore_SDL2::OnChannelFinished);

	Mix_AllocateChannels(HYMAX_AUDIOCHANNELS);
	m_NodeMap.reserve(HYMAX_AUDIOCHANNELS);
	m_ChannelMap.reserve(HYMAX_AUDIOCHANNELS);

	sm_pInstance = this;
}

/*virtual*/ HyAudioCore_SDL2::~HyAudioCore_SDL2(void)
{
	Mix_CloseAudio();
	Mix_Quit();

	sm_pInstance = nullptr;
}

const char *HyAudioCore_SDL2::GetAudioDriver()
{
	return SDL_GetCurrentAudioDriver();
}

/*virtual*/ void HyAudioCore_SDL2::OnUpdate() /*override*/
{
	if(m_CueList.empty())
		return;

	for(auto cue : m_CueList)
	{
		switch(cue.m_eCUE_TYPE)
		{
		case CUETYPE_PlayOneShotDefault:
		case CUETYPE_PlayOneShot:
		case CUETYPE_Start:
			if(cue.m_pNODE->Is2D())
				Play<HyAudio2d>(cue.m_eCUE_TYPE, static_cast<HyAudio2d *>(cue.m_pNODE));
			else
				Play<HyAudio3d>(cue.m_eCUE_TYPE, static_cast<HyAudio3d *>(cue.m_pNODE));
			break;

		case CUETYPE_Stop:
		case CUETYPE_Pause:
		case CUETYPE_Unpause:
		case CUETYPE_Attributes: {
			//for(auto iter = m_PlayList.begin(); iter != m_PlayList.end(); ++iter)
			//{
			//	if(iter->m_pID == cue.m_pNODE)
			//	{
			//		if(cue.m_eCUE_TYPE == CUETYPE_Stop)
			//			m_PlayList.erase(iter);
			//		else if(cue.m_eCUE_TYPE == CUETYPE_Pause)
			//			iter->m_bPaused = true;
			//		else if(cue.m_eCUE_TYPE == CUETYPE_Unpause)
			//			iter->m_bPaused = false;
			//		else if(cue.m_eCUE_TYPE == CUETYPE_Attributes)
			//		{
			//			if(cue.m_pNODE->Is2D())
			//			{
			//				iter->m_fVolume = static_cast<HyAudio2d *>(cue.m_pNODE)->volume.Get();
			//				iter->m_fPitch = static_cast<HyAudio2d *>(cue.m_pNODE)->pitch.Get();
			//			}
			//			else
			//			{
			//				iter->m_fVolume = static_cast<HyAudio3d *>(cue.m_pNODE)->volume.Get();
			//				iter->m_fPitch = static_cast<HyAudio3d *>(cue.m_pNODE)->pitch.Get();
			//			}
			//		}

			//		break;
			//	}
			//}
			
			break; }

		default:
			HyLogError("Unknown sound cue type");
			break;
		}
	}

	m_CueList.clear();
}

/*static*/ IHyFileAudioImpl *HyAudioCore_SDL2::AllocateBank(IHyAudioCore *pAudio, HyJsonObj bankObj)
{
	HyFileAudioImpl_SDL2 *pNewFileGuts = HY_NEW HyFileAudioImpl_SDL2(bankObj);
	static_cast<HyAudioCore_SDL2 *>(pAudio)->m_AudioFileList.push_back(pNewFileGuts);

	return pNewFileGuts;
}

template<typename NODETYPE>
void HyAudioCore_SDL2::Play(CueType ePlayType, NODETYPE *pAudioNode)
{
	// Determine buffer
	HyRawSoundBuffer *pBuffer = nullptr;
	uint32 uiSoundChecksum = static_cast<const HyAudioData *>(pAudioNode->AcquireData())->GetSound(pAudioNode);
	for(auto file : m_AudioFileList)
	{
		pBuffer = file->GetBufferInfo(uiSoundChecksum);
		if(pBuffer)
			break;
	}
	if(pBuffer == nullptr)
	{
		HyLogWarning("Could not find audio: " << uiSoundChecksum);
		return;
	}

	// Determine attributes
	float fVolume = 1.0f;
	float fPitch = 1.0f;
	if(ePlayType != CUETYPE_PlayOneShotDefault)
	{
		fVolume = pAudioNode->volume.Get();
		fPitch = pAudioNode->pitch.Get();
	}

	// Play in Mixer
	if(pBuffer->IsMusic())
	{
	}
	else
	{
		int32 iAssignedChannel = -1;
		if(ePlayType == CUETYPE_Start && m_NodeMap.count(pAudioNode) != 0)
			iAssignedChannel = m_NodeMap[pAudioNode];

		iAssignedChannel = Mix_PlayChannel(iAssignedChannel, pBuffer->GetSfxPtr(), pAudioNode->GetLoops());
		if(iAssignedChannel == -1)
			return;

		Mix_Volume(iAssignedChannel, static_cast<int>(MIX_MAX_VOLUME * fVolume));

		if(ePlayType == CUETYPE_Start)
		{
			m_NodeMap[pAudioNode] = iAssignedChannel;
			m_ChannelMap[iAssignedChannel] = pAudioNode;
		}
	}
}

/*static*/ void HyAudioCore_SDL2::OnChannelFinished(int32 iChannel)
{
	if(sm_pInstance->m_ChannelMap.count(iChannel) == 0)
		return;

	sm_pInstance->m_NodeMap.erase(sm_pInstance->m_ChannelMap[iChannel]);
	sm_pInstance->m_ChannelMap.erase(iChannel);
}

#endif // defined(HY_USE_SDL2)
