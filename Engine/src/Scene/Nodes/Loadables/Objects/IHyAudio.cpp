/**************************************************************************
 *	IHyAudio.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2021 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/

#include "Afx/HyInteropAfx.h"
#include "Scene/Nodes/Loadables/Objects/IHyAudio.h"
#include "Scene/Nodes/Loadables/IHyLoadable2d.h"
#include "Scene/Nodes/Loadables/IHyLoadable3d.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Scene/HyScene.h"
#include "HyEngine.h"

template<typename NODETYPE, typename ENTTYPE>
HyAudioNodeHandle IHyAudio<NODETYPE, ENTTYPE>::sm_hUniqueIdCounter = 1;

template<typename NODETYPE, typename ENTTYPE>
IHyAudio<NODETYPE, ENTTYPE>::IHyAudio(std::string sPrefix, std::string sName, ENTTYPE *pParent) :
	NODETYPE(HYTYPE_Audio, sPrefix, sName, pParent),
	m_hUNIQUE_ID(sm_hUniqueIdCounter++),
	m_uiCueFlags(0),
	m_fVolume(1.0f),
	m_fPitch(0.0f),
	volume(m_fVolume, *this, NODETYPE::DIRTY_Audio),
	pitch(m_fPitch, *this, NODETYPE::DIRTY_Audio)
{
}

template<typename NODETYPE, typename ENTTYPE>
IHyAudio<NODETYPE, ENTTYPE>::IHyAudio(uint32 uiSoundChecksum, uint32 uiBankId, ENTTYPE *pParent) :
	NODETYPE(HYTYPE_Audio, std::to_string(uiSoundChecksum), std::to_string(uiBankId), pParent),
	m_hUNIQUE_ID(sm_hUniqueIdCounter++),
	m_uiCueFlags(0),
	m_fVolume(1.0f),
	m_fPitch(0.0f),
	volume(m_fVolume, *this, NODETYPE::DIRTY_Audio),
	pitch(m_fPitch, *this, NODETYPE::DIRTY_Audio)
{
	m_uiFlags |= SETTING_IsAuxiliary;
}

template<typename NODETYPE, typename ENTTYPE>
IHyAudio<NODETYPE, ENTTYPE>::IHyAudio(HyAudioHandle hAudioHandle, ENTTYPE *pParent) :
	NODETYPE(HYTYPE_Audio, std::to_string(hAudioHandle.first), std::to_string(hAudioHandle.second), pParent),
	m_hUNIQUE_ID(sm_hUniqueIdCounter++),
	m_uiCueFlags(0),
	m_fVolume(1.0f),
	m_fPitch(0.0f),
	volume(m_fVolume, *this, NODETYPE::DIRTY_Audio),
	pitch(m_fPitch, *this, NODETYPE::DIRTY_Audio)
{
	m_uiFlags |= SETTING_IsAuxiliary;
}

template<typename NODETYPE, typename ENTTYPE>
IHyAudio<NODETYPE, ENTTYPE>::IHyAudio(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit, int32 iCategoryId, ENTTYPE *pParent) :
	NODETYPE(HYTYPE_Audio, "", "", pParent),
	m_hUNIQUE_ID(sm_hUniqueIdCounter++),
	m_uiCueFlags(0),
	m_fVolume(1.0f),
	m_fPitch(0.0f),
	volume(m_fVolume, *this, NODETYPE::DIRTY_Audio),
	pitch(m_fPitch, *this, NODETYPE::DIRTY_Audio)
{
	m_uiFlags |= SETTING_IsAuxiliary;

	HyTextureQuadHandle hTexQuadHandle = HyEngine::CreateAudio(HyIO::CleanPath(sFilePath.c_str()), bIsStreaming, iInstanceLimit, iCategoryId);
	m_sPrefix = std::to_string(hTexQuadHandle.first);
	m_sName = std::to_string(hTexQuadHandle.second);
}

template<typename NODETYPE, typename ENTTYPE>
IHyAudio<NODETYPE, ENTTYPE>::IHyAudio(const IHyAudio &copyRef) :
	NODETYPE(copyRef),
	m_hUNIQUE_ID(sm_hUniqueIdCounter++),
	m_uiCueFlags(copyRef.m_uiCueFlags),
	m_fVolume(copyRef.m_fVolume),
	m_fPitch(copyRef.m_fPitch),
	volume(m_fVolume, *this, NODETYPE::DIRTY_Audio),
	pitch(m_fPitch, *this, NODETYPE::DIRTY_Audio)
{
	for(uint32 i = 0; i < static_cast<uint32>(copyRef.m_AudioStateAttribList.size()); ++i)
		m_AudioStateAttribList.push_back(copyRef.m_AudioStateAttribList[i]);

	volume = copyRef.volume;
	pitch = copyRef.pitch;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ IHyAudio<NODETYPE, ENTTYPE>::~IHyAudio(void)
{
	IHyNode::sm_pScene->ProcessAudioCue(this, HYSOUNDCUE_DeleteInstance);
}

template<typename NODETYPE, typename ENTTYPE>
const IHyAudio<NODETYPE, ENTTYPE> &IHyAudio<NODETYPE, ENTTYPE>::operator=(const IHyAudio<NODETYPE, ENTTYPE> &rhs)
{
	NODETYPE::operator=(rhs);

	m_uiCueFlags = rhs.m_uiCueFlags;
	m_fVolume = rhs.m_fVolume;
	m_fPitch = rhs.m_fPitch;
	
	m_AudioStateAttribList.clear();
	for(uint32 i = 0; i < static_cast<uint32>(rhs.m_AudioStateAttribList.size()); ++i)
		m_AudioStateAttribList.push_back(rhs.m_AudioStateAttribList[i]);

	volume = rhs.volume;
	pitch = rhs.pitch;

	return *this;
}

template<typename NODETYPE, typename ENTTYPE>
HyAudioNodeHandle IHyAudio<NODETYPE, ENTTYPE>::GetHandle() const
{
	return m_hUNIQUE_ID;
}

template<typename NODETYPE, typename ENTTYPE>
int32 IHyAudio<NODETYPE, ENTTYPE>::GetLoops() const
{
	return m_AudioStateAttribList[this->m_uiState].m_uiLoops;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::SetLoops(int32 iLoops)
{
	m_AudioStateAttribList[this->m_uiState].m_uiLoops = iLoops;
}

template<typename NODETYPE, typename ENTTYPE>
bool IHyAudio<NODETYPE, ENTTYPE>::IsPlaying() const
{
	return 0 == (m_uiCueFlags & ((1 << HYSOUNDCUE_Stop) | (1 << HYSOUNDCUE_Pause))) && HyEngine::Audio().IsPlaying(m_hUNIQUE_ID);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::PlayOneShot(bool bUseCurrentSettings /*= true*/)
{
	if(this->IsLoaded() == false)
	{
		HyLogWarning("IHyAudio::PlayOneShot() called on a HyAudio that is not loaded: " << this->GetPrefix() << "/" << this->GetName());
		return;
	}

	if(bUseCurrentSettings)
		m_uiCueFlags |= (1 << HYSOUNDCUE_PlayOneShot);
	else
		m_uiCueFlags |= (1 << HYSOUNDCUE_PlayOneShotDefault);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::Play()
{
	if(this->IsLoaded() == false)
	{
		HyLogWarning("IHyAudio::Play() called on a HyAudio that is not loaded: " << this->GetPrefix() << "/" << this->GetName());
		return;
	}

	m_uiCueFlags &= ~(1 << HYSOUNDCUE_Stop);
	m_uiCueFlags |= (1 << HYSOUNDCUE_Start);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::SetPause(bool bPause)
{
	if(bPause)
	{
		m_uiCueFlags &= ~(1 << HYSOUNDCUE_Unpause);
		m_uiCueFlags |= (1 << HYSOUNDCUE_Pause);
	}
	else
	{
		m_uiCueFlags &= ~(1 << HYSOUNDCUE_Pause);
		m_uiCueFlags |= (1 << HYSOUNDCUE_Unpause);
	}
}

template<typename NODETYPE, typename ENTTYPE>
float IHyAudio<NODETYPE, ENTTYPE>::GetElapsedPlayTime() const
{
	return HyEngine::Audio().GetElapsedPlayTime(m_hUNIQUE_ID);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::Stop()
{
	m_uiCueFlags &= ~(1 << HYSOUNDCUE_Start);
	m_uiCueFlags |= (1 << HYSOUNDCUE_Stop);
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHyAudio<NODETYPE, ENTTYPE>::SetState(uint32 uiStateIndex) /*override*/
{
	if(this->m_uiState == uiStateIndex || IHyLoadable::SetState(uiStateIndex) == false)
		return false;

	m_SoundOrderList.clear();
	return true;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHyAudio<NODETYPE, ENTTYPE>::IsLoadDataValid() /*override*/
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->AcquireData());
	return pData && pData->GetNumStates() != 0;
}

template<typename NODETYPE, typename ENTTYPE>
HyAudioHandle IHyAudio<NODETYPE, ENTTYPE>::PullNextSound()
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->UncheckedGetData());
	switch(m_AudioStateAttribList[this->m_uiState].m_ePlaylistMode)
	{
	case HYPLAYLIST_Shuffle: {
		if(m_SoundOrderList.empty())
			pData->WeightedShuffle(this->m_uiState, m_SoundOrderList);

		m_hLastPlayed = m_SoundOrderList.back();
		m_SoundOrderList.pop_back();
		return m_hLastPlayed;
	}

	case HYPLAYLIST_Weighted: {
		const HyAudioPlaylist &playlistRef = pData->GetPlaylist(this->m_uiState);
		m_hLastPlayed = playlistRef[pData->WeightedEntryPull(playlistRef)].m_hAudioHandle;
		return m_hLastPlayed;
	}

	case HYPLAYLIST_SequentialLocal: {
		if(m_SoundOrderList.empty())
		{
			const HyAudioPlaylist &playlistRef = pData->GetPlaylist(this->m_uiState);
			for(int i = 0; i < playlistRef.size(); ++i)
				m_SoundOrderList.push_back(playlistRef[i].m_hAudioHandle);
		}

		m_hLastPlayed = m_SoundOrderList.back();
		m_SoundOrderList.pop_back();
		return m_hLastPlayed;
	}

	case HYPLAYLIST_SequentialGlobal:
		m_hLastPlayed = pData->GetNextSequential(this->m_uiState);
		return m_hLastPlayed;

	default:
		HyError("HyAudioData::GetSound - Unhandled cue type");
		return HyAudioHandle(0, 0);
	}
}

template<typename NODETYPE, typename ENTTYPE>
HyAudioHandle IHyAudio<NODETYPE, ENTTYPE>::GetLastPulledSound() const
{
	return m_hLastPlayed;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyAudio<NODETYPE, ENTTYPE>::OnDataAcquired() /*override*/
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->UncheckedGetData());

	m_AudioStateAttribList.clear();
	for(uint32 i = 0; i < pData->GetNumStates(); ++i)
	{
		AudioStateAttribs stateAttrib(pData->GetPlaylistMode(i),
									  true, // TODO: Not implemented yet
									  pData->GetPriority(i),
									  pData->GetLoops(i),
									  pData->GetMaxDistance(i));

		m_AudioStateAttribList.push_back(stateAttrib);
	}
}

//template<typename NODETYPE, typename ENTTYPE>
///*virtual*/ void IHyAudio<NODETYPE, ENTTYPE>::OnLoaded() /*override*/
//{
//	IHyLoadable2d::OnLoaded();
//
//	if(IsExternalFile()) // Do blocking load of audio
//	{
//	}
//}
//
//template<typename NODETYPE, typename ENTTYPE>
///*virtual*/ void IHyAudio<NODETYPE, ENTTYPE>::OnUnloaded() /*override*/
//{
//	IHyLoadable2d::OnUnloaded();
//
//	if(IsExternalFile())
//	{
//	}
//}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyAudio<NODETYPE, ENTTYPE>::OnLoadedUpdate() /*override*/
{
	if(IHyNode::IsDirty(IHyNode::DIRTY_Audio))
	{
		m_uiCueFlags |= (1 << HYSOUNDCUE_Attributes);
		IHyNode::ClearDirty(IHyNode::DIRTY_Audio);
	}

	if(m_uiCueFlags)
	{
		for(uint32 i = 0; i < NUM_HYSOUNDCUE; ++i)
		{
			if(0 != (m_uiCueFlags & (1 << i)))
				IHyNode::sm_pScene->ProcessAudioCue(this, static_cast<HySoundCue>(i));
		}

		m_uiCueFlags = 0;
	}
}

//template<typename NODETYPE, typename ENTTYPE>
//void IHyAudio<NODETYPE, ENTTYPE>::Shuffle()
//{
//
//}
//
//template<typename NODETYPE, typename ENTTYPE>
//int32 IHyAudio<NODETYPE, ENTTYPE>::PullEntryIndex(const HyAudioPlaylist &entriesList)
//{
//
//}

template class IHyAudio<IHyLoadable2d, HyEntity2d>;
template class IHyAudio<IHyLoadable3d, HyEntity3d>;
