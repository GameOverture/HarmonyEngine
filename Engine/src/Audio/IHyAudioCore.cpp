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

IHyAudioCore *IHyAudioCore::sm_pInstance = nullptr;

IHyAudioCore::IHyAudioCore() :
	m_fGlobalSfxVolume(1.0f),
	m_fGlobalMusicVolume(1.0f)
{
	sm_pInstance = this;
}

/*virtual*/ IHyAudioCore::~IHyAudioCore(void)
{
	sm_pInstance = nullptr;
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
	case CUETYPE_PlayOneShot: {
		PlayInfo newPlayInfo;
		if(pNode->Is2D())
		{
			if(eCueType == CUETYPE_PlayOneShot)
			{
				newPlayInfo.m_fVolume = static_cast<HyAudio2d *>(pNode)->volume.Get();
				newPlayInfo.m_fPitch = static_cast<HyAudio2d *>(pNode)->pitch.Get();
			}
			newPlayInfo.m_uiSoundChecksum = static_cast<HyAudio2d *>(pNode)->PullNextSound();
			newPlayInfo.m_uiLoops = static_cast<HyAudio2d *>(pNode)->GetLoops();
		}
		else
		{
			if(eCueType == CUETYPE_PlayOneShot)
			{
				newPlayInfo.m_fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
				newPlayInfo.m_fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
			}
			newPlayInfo.m_uiSoundChecksum = static_cast<HyAudio3d *>(pNode)->PullNextSound();
			newPlayInfo.m_uiLoops = static_cast<HyAudio3d *>(pNode)->GetLoops();
		}

		m_OneShotList.push_back(newPlayInfo);
		OnCue_Play(newPlayInfo);
		break; }

	case CUETYPE_Start: {
		PlayInfo *pPlayInfo = nullptr;
		if(pNode->Is2D())
		{
			HyAudioHandle hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
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
			HyAudioHandle hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
			if(m_PlayMap.count(hHandle) == 0)
				m_PlayMap[hHandle] = PlayInfo();
			pPlayInfo = &m_PlayMap[hHandle];

			pPlayInfo->m_fVolume = static_cast<HyAudio3d *>(pNode)->volume.Get();
			pPlayInfo->m_fPitch = static_cast<HyAudio3d *>(pNode)->pitch.Get();
			pPlayInfo->m_uiSoundChecksum = static_cast<HyAudio3d *>(pNode)->PullNextSound();
			pPlayInfo->m_uiLoops = static_cast<HyAudio3d *>(pNode)->GetLoops();
		}

		OnCue_Play(*pPlayInfo);
		break; }

	case CUETYPE_Stop:
	case CUETYPE_Pause:
	case CUETYPE_Unpause: {
		HyAudioHandle hHandle = HY_UNUSED_HANDLE;
		if(pNode->Is2D())
			hHandle = static_cast<HyAudio2d *>(pNode)->GetHandle();
		else
			hHandle = static_cast<HyAudio3d *>(pNode)->GetHandle();
		
		if(m_PlayMap.count(hHandle) == 0)
			break;
		
		if(eCueType == CUETYPE_Stop)
			OnCue_Stop(m_PlayMap[hHandle]);
		else if(eCueType == CUETYPE_Pause)
			OnCue_Pause(m_PlayMap[hHandle]);
		else if(eCueType == CUETYPE_Unpause)
			OnCue_Unpause(m_PlayMap[hHandle]);
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

		if(m_PlayMap.count(hHandle) == 0)
			break;

		m_PlayMap[hHandle].m_fVolume = fVolume;
		OnCue_Volume(m_PlayMap[hHandle]);
		break; }

	default:
		HyLogError("Unknown sound cue type");
		break;
	}
}

/*static*/ void IHyAudioCore::OnReportFinished(int32 iApiData)
{
	for(auto iter = sm_pInstance->m_PlayMap.begin(); iter != sm_pInstance->m_PlayMap.end(); ++iter)
	{
		if(iter->second.m_iApiData == iApiData)
		{
			sm_pInstance->m_PlayMap.erase(iter);
			return;
		}
	}

	for(auto iter = sm_pInstance->m_OneShotList.begin(); iter != sm_pInstance->m_OneShotList.end(); ++iter)
	{
		if(iter->m_iApiData == iApiData)
		{
			sm_pInstance->m_OneShotList.erase(iter);
			return;
		}
	}

	HyLogError("IHyAudioCore::OnReportFinished did not find: " << iApiData);
}
