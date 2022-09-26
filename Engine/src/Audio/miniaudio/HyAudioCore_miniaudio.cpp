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

////// SDL maps exactly the % of the volume meter in Window's Volume Mixer. But most other programs never approach 
////// this maximum (it's loud as hell), even when you max out their volume dial. Provide the maximum volume allowed in 'HYAUDIO_SDL_VOLUME_DAMPENING'
////#define HYAUDIO_SDL_VOLUME_DAMPENING 0.75f
//
//HyAudioCore_miniaudio::HyAudioCore_miniaudio()
//{
//	//m_fGlobalSfxVolume = HYAUDIO_SDL_VOLUME_DAMPENING;
//	//m_fGlobalMusicVolume = HYAUDIO_SDL_VOLUME_DAMPENING;
//
//	
//}
//
///*virtual*/ HyAudioCore_miniaudio::~HyAudioCore_miniaudio(void)
//{
//	
//}
//
//
//
//ma_engine &HyAudioCore_miniaudio::GetEngine()
//{
//	return m_Engine;
//}
//
///*virtual*/ void HyAudioCore_miniaudio::OnSetSfxVolume(float fGlobalSfxVolume) /*override*/
//{
//	m_fGlobalSfxVolume = HyClamp(fGlobalSfxVolume, 0.0f, 1.0f);// *HYAUDIO_SDL_VOLUME_DAMPENING;
//	//ma_engine_set_volume
//	//ma_sound_group_init(
//}
//
///*virtual*/ void HyAudioCore_miniaudio::OnSetMusicVolume(float fGlobalMusicVolume) /*override*/
//{
//	m_fGlobalMusicVolume = HyClamp(fGlobalMusicVolume, 0.0f, 1.0f);// *HYAUDIO_SDL_VOLUME_DAMPENING;
//	//ma_sound_group_init(
//}
//
///*virtual*/ void HyAudioCore_miniaudio::OnCue_Play(PlayInfo &playInfoRef) /*override*/
//{
//	playInfoRef.m_uiSoundChecksum
//	// Determine Sound Buffer
//	HySoundBuffer_miniaudio *pBuffer = nullptr;
//	for(auto file : m_AudioFileList)
//	{
//		pBuffer = static_cast<HySoundBuffer_miniaudio *>(file->GetSound(playInfoRef.m_uiSoundChecksum));
//		if(pBuffer)
//			break;
//	}
//	if(pBuffer == nullptr)
//	{
//		HyLogWarning("HyAudioCore_SDL2::OnCue_Play() Could not find audio: " << playInfoRef.m_uiSoundChecksum);
//		return;
//	}
//
//	pBuffer->GetSound();
//
//	HyError("Unfinished");
//}
//
///*virtual*/ void HyAudioCore_miniaudio::OnCue_Stop(PlayInfo &playInfoRef) /*override*/
//{
//	HyError("Unfinished");
//}
//
///*virtual*/ void HyAudioCore_miniaudio::OnCue_Pause(PlayInfo &playInfoRef) /*override*/
//{
//	HyError("Unfinished");
//}
//
///*virtual*/ void HyAudioCore_miniaudio::OnCue_Unpause(PlayInfo &playInfoRef) /*override*/
//{
//	HyError("Unfinished");
//}
//
///*virtual*/ void HyAudioCore_miniaudio::OnCue_Volume(PlayInfo &playInfoRef) /*override*/
//{
//	HyError("Unfinished");
//}
