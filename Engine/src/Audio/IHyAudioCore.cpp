/**************************************************************************
 *	IHyAudioCore.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/IHyAudioCore.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio3d.h"
#include "Diagnostics/Console/IHyConsole.h"

IHyAudioCore::IHyAudioCore() :
	m_fGlobalSfxVolume(1.0f),
	m_fGlobalMusicVolume(1.0f)
{
}

/*virtual*/ IHyAudioCore::~IHyAudioCore(void)
{
}

void IHyAudioCore::SetSfxVolume(float fGlobalSfxVolume)
{
	m_fGlobalSfxVolume = fGlobalSfxVolume;
	OnSetSfxVolume(m_fGlobalSfxVolume);
}

void IHyAudioCore::SetMusicVolume(float fGlobalMusicVolume)
{
	m_fGlobalMusicVolume = fGlobalMusicVolume;
	OnSetMusicVolume(m_fGlobalMusicVolume);
}

IHyFileAudioImpl *IHyAudioCore::AllocateAudioBank(HyJsonObj bankObj)
{
	IHyFileAudioImpl *pNewlyAlloc = OnAllocateAudioBank(bankObj);
	m_AudioFileList.push_back(pNewlyAlloc);

	return pNewlyAlloc;
}

void IHyAudioCore::ProcessCue(IHyNode *pNode, CueType eCueType)
{
	switch(eCueType)
	{
	case CUETYPE_PlayOneShotDefault:
	case CUETYPE_PlayOneShot:
	case CUETYPE_Start: {
		HyAudioHandle hHandle = HY_UNUSED_HANDLE;
		PlayInfo newPlayInfo;
		if(pNode->Is2D())
		{
			if(eCueType == CUETYPE_Start)
				hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
			if(eCueType != CUETYPE_PlayOneShotDefault)
			{
				newPlayInfo.m_fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
				newPlayInfo.m_fPitch = static_cast<HyAudio2d *>(pNode)->pitch.Get();
			}
			newPlayInfo.m_uiSoundChecksum = static_cast<HyAudio2d *>(pNode)->PullNextSound();
			newPlayInfo.m_uiLoops = static_cast<HyAudio2d *>(pNode)->GetLoops();
		}
		else
		{
			if(eCueType == CUETYPE_Start)
				hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
			if(eCueType != CUETYPE_PlayOneShotDefault)
			{
				newPlayInfo.m_fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
				newPlayInfo.m_fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
			}
			newPlayInfo.m_uiSoundChecksum = static_cast<HyAudio3d *>(pNode)->PullNextSound();
			newPlayInfo.m_uiLoops = static_cast<HyAudio3d *>(pNode)->GetLoops();
		}

		if(hHandle != HY_UNUSED_HANDLE)
			m_PlayMap.emplace(hHandle, newPlayInfo);

		OnCue_Play(hHandle, newPlayInfo);
		break; }

	case CUETYPE_Stop:
	case CUETYPE_Pause:
	case CUETYPE_Unpause: {
		HyAudioHandle hHandle = HY_UNUSED_HANDLE;
		if(pNode->Is2D())
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
		else
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
		if(m_PlayMap.find(hHandle) == m_PlayMap.end())
			break;
		
		if(eCueType == CUETYPE_Stop)
			OnCue_Stop(hHandle);
		else if(eCueType == CUETYPE_Pause)
			OnCue_Pause(hHandle);
		else if(eCueType == CUETYPE_Unpause)
			OnCue_Unpause(hHandle);
		break; }

	case CUETYPE_Attributes: {
		HyAudioHandle hHandle = HY_UNUSED_HANDLE;
		float fVolume = 1.0f;
		if(pNode->Is2D())
		{
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
			fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
		}
		else
		{
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
			fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
		}
		if(m_PlayMap.find(hHandle) == m_PlayMap.end())
			break;

		m_PlayMap[hHandle].m_fVolume = fVolume;
		OnCue_Volume(hHandle);
		break; }

	default:
		HyLogError("Unknown sound cue type");
		break;
	}
}

void IHyAudioCore::ReportFinished(HyAudioHandle hHandle)
{
	m_PlayMap.erase(hHandle);
}
