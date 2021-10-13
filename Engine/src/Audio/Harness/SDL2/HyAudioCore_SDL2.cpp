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
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyMath.h"

#if defined(HY_USE_SDL2)

#ifdef HY_PLATFORM_BROWSER
	#include <SDL2/SDL_mixer.h>
#else
	#include "SDL_mixer.h"
#endif

// SDL maps exactly the % of the volume meter in Window's Volume Mixer. But most other programs never approach 
// this maximum, even when you max out their volume dial. Provide the maximum volume allowed in 'HYAUDIO_SDL_VOLUME_DAMPENING'
#define HYAUDIO_SDL_VOLUME_DAMPENING 0.75f

HyAudioCore_SDL2 *HyAudioCore_SDL2::sm_pInstance = nullptr;

HyAudioCore_SDL2::HyAudioCore_SDL2() :
	m_fGlobalSfxVolume(HYAUDIO_SDL_VOLUME_DAMPENING),
	m_fGlobalMusicVolume(HYAUDIO_SDL_VOLUME_DAMPENING)
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
	Mix_ChannelFinished(HyAudioCore_SDL2::OnChannelFinished);

	int32 iNumChannels = Mix_AllocateChannels(HYMAX_AUDIOCHANNELS);
	m_NodeMap.reserve(iNumChannels);
	m_ChannelMap.reserve(iNumChannels);

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

/*virtual*/ void HyAudioCore_SDL2::SetSfxVolume(float fGlobalSfxVolume) /*override*/
{
	m_fGlobalSfxVolume = HyClamp(fGlobalSfxVolume, 0.0f, 1.0f) * HYAUDIO_SDL_VOLUME_DAMPENING;
	
	for(const auto &iter : m_ChannelMap)
	{
		if(iter.second->Is2D())
			Mix_Volume(iter.first, static_cast<int>(MIX_MAX_VOLUME * (static_cast<HyAudio2d *>(iter.second)->volume.Get() * m_fGlobalSfxVolume)));
		else
			Mix_Volume(iter.first, static_cast<int>(MIX_MAX_VOLUME * (static_cast<HyAudio3d *>(iter.second)->volume.Get() * m_fGlobalSfxVolume)));
	}
}

/*virtual*/ void HyAudioCore_SDL2::SetMusicVolume(float fGlobalMusicVolume) /*override*/
{
	m_fGlobalMusicVolume = HyClamp(fGlobalMusicVolume, 0.0f, 1.0f) * HYAUDIO_SDL_VOLUME_DAMPENING;
	Mix_VolumeMusic(static_cast<int>(MIX_MAX_VOLUME * m_fGlobalMusicVolume));
}

/*virtual*/ IHyFileAudioImpl *HyAudioCore_SDL2::AllocateAudioBank(HyJsonObj bankObj) /*override*/
{
	HyFileAudioImpl_SDL2 *pNewFileGuts = HY_NEW HyFileAudioImpl_SDL2(bankObj);
	m_AudioFileList.push_back(pNewFileGuts);

	return pNewFileGuts;
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
		case CUETYPE_Unpause: {
			auto nodeIter = m_NodeMap.find(cue.m_pNODE);
			if(nodeIter == m_NodeMap.end())
			{
				HyLogWarning("HyAudioCore_SDL2 could not find audio node to stop/(un)pause");
				break;
			}
			if(cue.m_eCUE_TYPE == CUETYPE_Stop)
				Mix_HaltChannel(nodeIter->second);
			else if(cue.m_eCUE_TYPE == CUETYPE_Pause)
				Mix_Pause(nodeIter->second);
			else if(cue.m_eCUE_TYPE == CUETYPE_Unpause)
				Mix_Resume(nodeIter->second);
			break; }
		
		case CUETYPE_Attributes:
			if(cue.m_pNODE->Is2D())
				Modify<HyAudio2d>(static_cast<HyAudio2d *>(cue.m_pNODE));
			else
				Modify<HyAudio3d>(static_cast<HyAudio3d *>(cue.m_pNODE));
			break;


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
			
			break;

		default:
			HyLogError("Unknown sound cue type");
			break;
		}
	}

	m_CueList.clear();
}

template<typename NODETYPE>
void HyAudioCore_SDL2::Play(CueType ePlayType, NODETYPE *pAudioNode)
{
	// Determine buffer
	HyRawSoundBuffer *pBuffer = nullptr;
	uint32 uiSoundChecksum = pAudioNode->PullNextSound();
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
		Mix_PlayMusic(pBuffer->GetMusicPtr(), pAudioNode->GetLoops());
		
		int32 iVolume = static_cast<int>(MIX_MAX_VOLUME * (fVolume * m_fGlobalMusicVolume));
		Mix_VolumeMusic(iVolume);
	}
	else
	{
		int32 iAssignedChannel = -1;
		if(ePlayType == CUETYPE_Start && m_NodeMap.count(pAudioNode) != 0)
			iAssignedChannel = m_NodeMap[pAudioNode];

		iAssignedChannel = Mix_PlayChannel(iAssignedChannel, pBuffer->GetSfxPtr(), pAudioNode->GetLoops());
		if(iAssignedChannel == -1)
			return;

		int32 iVolume = static_cast<int>(MIX_MAX_VOLUME * (fVolume * m_fGlobalSfxVolume));
		Mix_Volume(iAssignedChannel, iVolume);

		if(ePlayType == CUETYPE_Start)
		{
			m_NodeMap[pAudioNode] = iAssignedChannel;
			m_ChannelMap[iAssignedChannel] = pAudioNode;

			HyLogWarning("m_ChannelMap CUE - " << iAssignedChannel);
		}
	}
}

template<typename NODETYPE>
void HyAudioCore_SDL2::Modify(NODETYPE *pAudioNode)
{
	auto nodeIter = m_NodeMap.find(pAudioNode);
	if(nodeIter == m_NodeMap.end())
		return;

	int32 iVolume = static_cast<int>(MIX_MAX_VOLUME * (pAudioNode->volume.Get() * m_fGlobalSfxVolume));
	Mix_Volume(nodeIter->second, iVolume);

	if(pAudioNode->pitch.Get() != 1.0f)
	{
		Mix_RegisterEffect(nodeIter->second, OnPitchModifer, nullptr, nodeIter->first);
	}
}

/*static*/ void HyAudioCore_SDL2::OnChannelFinished(int32 iChannel)
{
	if(sm_pInstance->m_ChannelMap.count(iChannel) == 0)
	{
		HyLogWarning("HyAudioCore_SDL2::OnChannelFinished() - m_ChannelMap[" << iChannel << "] had zero entires");
		return;
	}

	HyLogWarning("m_ChannelMap ERASE - " << iChannel);

	sm_pInstance->m_NodeMap.erase(sm_pInstance->m_ChannelMap[iChannel]);
	sm_pInstance->m_ChannelMap.erase(iChannel);
}

Uint16 formatSampleSize(Uint16 format)
{
	return (format & 0xFF) / 8;
}

/*static*/ int HyAudioCore_SDL2::computeChunkLengthMillisec(int chunkSize)
{
	/* bytes / samplesize == sample points */
	const Uint32 points = chunkSize / sizeof(int16);//formatSampleSize(sm_pInstance->m_uiDesiredFormat);

	/* sample points / channels == sample frames */
	const Uint32 frames = (points / sm_pInstance->m_iDesiredNumChannels);

	/* (sample frames * 1000) / frequency == play length, in ms */
	return ((frames * 1000) / sm_pInstance->m_iDesiredFrequency);
}

/*static*/ void HyAudioCore_SDL2::OnPitchModifer(int iChannel, void *pStream, int iLength, void *pData)
{
	// TODO: Check for 3D node
	HyAudio2d *pNode = reinterpret_cast<HyAudio2d *>(pData);
	
	HyRawSoundBuffer *pBuffer = nullptr;
	uint32 uiSoundChecksum = pNode->GetLastPlayed();
	for(auto file : sm_pInstance->m_AudioFileList)
	{
		pBuffer = file->GetBufferInfo(uiSoundChecksum);
		if(pBuffer)
			break;
	}

	if(pBuffer == nullptr)
		return;


	const float speedFactor = pNode->pitch.Get(); // speed
	const int channelCount = sm_pInstance->m_iDesiredNumChannels;
	const int frequency = sm_pInstance->m_iDesiredFrequency;

	const Sint16* chunkData = reinterpret_cast<Sint16*>(pBuffer->GetSfxPtr()->abuf);

	Sint16* buffer = static_cast<Sint16*>(pStream);
	const int bufferSize = iLength / sizeof(int16);  // buffer size (as array)
	const int bufferDuration = computeChunkLengthMillisec(iLength);  // buffer time duration

	//if(not touched)  // if playback is still untouched
	//{
	//	// if playback is still untouched and no pitch is requested this time, skip pitch routine and leave stream untouched.
	//	if(speedFactor == 1.0f)
	//	{
	//		// if there is still sound to be played
	//		if(position < duration or loop)
	//		{
	//			// just update position
	//			position += bufferDuration;

	//			// reset position if looping
	//			if(loop) while(position > duration)
	//				position -= duration;
	//		}
	//		else  // if we already played the whole sound, halt channel
	//		{
	//			// set silence on the buffer since Mix_HaltChannel() poops out some of it for a few ms.
	//			for(int i = 0; i < bufferSize; i++)
	//				buffer[i] = 0;

	//			Mix_HaltChannel(mixChannel);
	//		}

	//		return;  // skipping pitch routine
	//	}
	//	// if pitch is required for the first time
	//	else
	//		touched = true;  // mark as touched and proceed to the pitch routine.
	//}

	//// if there is still sound to be played
	//if(position < duration or loop)
	//{
		//const float delta = 1000.0/frequency,   // normal duration of each sample
		//			delta2 = delta*speedFactor; // virtual stretched duration, scaled by 'speedFactor'

		//for(int i = 0; i < bufferSize; i += channelCount)
		//{
		//	const int j = i/channelCount; // j goes from 0 to size/channelCount, incremented 1 by 1
		//	const float x = position + j*delta2;  // get "virtual" index. its corresponding value will be interpolated.
		//	const int k = floor(x / delta);  // get left index to interpolate from original chunk data (right index will be this plus 1)
		//	const float proportion = (x / delta) - k;  // get the proportion of the right value (left will be 1.0 minus this)

		//	// usually just 2 channels: 0 (left) and 1 (right), but who knows...
		//	for(int c = 0; c < channelCount; c++)
		//	{
		//		// check if k will be within bounds
		//		if(k*channelCount + channelCount - 1 < iLength/* or loop*/)
		//		{
		//			Sint16  leftValue =  chunkData[(  k   * channelCount + c) % iLength],
		//								rightValue = chunkData[((k+1) * channelCount + c) % iLength];

		//			// put interpolated value on 'data' (linear interpolation)
		//			buffer[i + c] = (1-proportion)*leftValue + proportion*rightValue;
		//		}
		//		else  // if k will be out of bounds (chunk bounds), it means we already finished; thus, we'll pass silence
		//		{
		//			buffer[i + c] = 0;
		//		}
		//	}
		//}

	//	// update position
	//	position += bufferDuration * speedFactor; // this is not exact since a frame may play less than its duration when finished playing, but its simpler

	//	// reset position if looping
	//	if(loop) while(position > duration)
	//		position -= duration;
	//}
	//else  // if we already played the whole sound but finished earlier than expected by SDL_mixer (due to faster playback speed)
	//{
	//	// set silence on the buffer since Mix_HaltChannel() poops out some of it for a few ms.
	//	for(int i = 0; i < bufferSize; i++)
	//		buffer[i] = 0;

	//	Mix_HaltChannel(mixChannel);
	//}
}

#endif // defined(HY_USE_SDL2)
