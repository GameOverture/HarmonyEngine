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
IHyAudio<NODETYPE, ENTTYPE>::IHyAudio(HyAudioHandle hAudioHandle, ENTTYPE *pParent) :
	NODETYPE(HYTYPE_Audio, std::to_string(hAudioHandle), HYASSETS_Hotload, pParent),
	m_hUNIQUE_ID(sm_hUniqueIdCounter++),
	m_uiCueFlags(0),
	m_fVolume(1.0f),
	m_fPitch(0.0f),
	volume(m_fVolume, *this, NODETYPE::DIRTY_Audio),
	pitch(m_fPitch, *this, NODETYPE::DIRTY_Audio)
{

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
void IHyAudio<NODETYPE, ENTTYPE>::PlayOneShot(bool bUseCurrentSettings /*= true*/)
{
	if(bUseCurrentSettings)
		m_uiCueFlags |= (1 << HYSOUNDCUE_PlayOneShot);
	else
		m_uiCueFlags |= (1 << HYSOUNDCUE_PlayOneShotDefault);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::Play()
{
	m_uiCueFlags &= ~(1 << HYSOUNDCUE_Stop);
	m_uiCueFlags |= (1 << HYSOUNDCUE_Start);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::Stop()
{
	m_uiCueFlags &= ~(1 << HYSOUNDCUE_Start);
	m_uiCueFlags |= (1 << HYSOUNDCUE_Stop);
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
/*virtual*/ bool IHyAudio<NODETYPE, ENTTYPE>::SetState(uint32 uiStateIndex) /*override*/
{
	if(this->m_uiState == uiStateIndex || IHyLoadable::SetState(uiStateIndex) == false)
		return false;

	m_CurPlayList.clear();
	return true;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHyAudio<NODETYPE, ENTTYPE>::IsLoadDataValid() /*override*/
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->AcquireData());
	return pData && pData->GetNumStates() != 0;
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyAudio<NODETYPE, ENTTYPE>::PullNextSound()
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->UncheckedGetData());
	switch(m_AudioStateAttribList[this->m_uiState].m_ePlayListMode)
	{
	case HYPLAYLIST_Shuffle: {
		if(m_CurPlayList.empty())
			Shuffle();
		m_uiLastPlayed = m_CurPlayList.back();
		m_CurPlayList.pop_back();
		return m_uiLastPlayed;
	}

	case HYPLAYLIST_Weighted: {
		const HyAudioPlayList &playListRef = pData->GetPlayList(this->m_uiState);
		m_uiLastPlayed = playListRef[PullEntryIndex(playListRef)].first;
		return m_uiLastPlayed;
	}

	case HYPLAYLIST_SequentialLocal: {
		if(m_CurPlayList.empty())
		{
			const HyAudioPlayList &playListRef = pData->GetPlayList(this->m_uiState);
			for(int i = 0; i < playListRef.size(); ++i)
				m_CurPlayList.push_back(playListRef[i].first);
		}
		m_uiLastPlayed = m_CurPlayList.back();
		m_CurPlayList.pop_back();
		return m_uiLastPlayed;
	}

	case HYPLAYLIST_SequentialGlobal:
		m_uiLastPlayed = pData->GetNextSequential(this->m_uiState);
		return m_uiLastPlayed;

	default:
		HyError("HyAudioData::GetSound - Unhandled cue type");
		return 0;
	}
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyAudio<NODETYPE, ENTTYPE>::GetLastPulledSound() const
{
	return m_uiLastPlayed;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyAudio<NODETYPE, ENTTYPE>::OnDataAcquired() /*override*/
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->UncheckedGetData());

	m_AudioStateAttribList.clear();
	for(uint32 i = 0; i < pData->GetNumStates(); ++i)
	{
		AudioStateAttribs stateAttrib(pData->GetPlayListMode(i),
									  true, // TODO: Not implemented yet
									  pData->GetPriority(i),
									  pData->GetLoops(i),
									  pData->GetMaxDistance(i));

		m_AudioStateAttribList.push_back(stateAttrib);
	}
}

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

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::Shuffle()
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->AcquireData());
	const HyAudioPlayList &playListRef = pData->GetPlayList(this->m_uiState);
	HyAudioPlayList tmpPlayList(playListRef);

	m_CurPlayList.clear();
	m_CurPlayList.reserve(playListRef.size());

	for(int i = 0; i < playListRef.size(); ++i)
	{
		uint32 uiIndex = PullEntryIndex(tmpPlayList);

		m_CurPlayList.push_back(tmpPlayList[uiIndex].first);
		tmpPlayList.erase(tmpPlayList.begin() + uiIndex);
	}
}

template<typename NODETYPE, typename ENTTYPE>
uint32 IHyAudio<NODETYPE, ENTTYPE>::PullEntryIndex(const HyAudioPlayList &entriesList)
{
	uint32 uiTotalWeight = 0;
	for(int j = 0; j < entriesList.size(); ++j)
		uiTotalWeight += entriesList[j].second;

	uint32 uiWeight = HyRand::Range(0u, uiTotalWeight);

	// If this rng exceeds the weight table max return the last valid entry
	uint32 uiIndex = 0;
	if(uiWeight >= uiTotalWeight)
	{
		uiIndex = static_cast<uint32>(entriesList.size()) - 1;
		while(entriesList[uiIndex].second == 0 && uiIndex > 0)
			uiIndex--;
	}
	else // Normal lookup
	{
		uint32 tmpCount = entriesList[uiIndex].second;
		while(tmpCount <= uiWeight && uiIndex < entriesList.size())
		{
			uiIndex++;
			tmpCount += entriesList[uiIndex].second;
		}
	}

	return uiIndex;
}

template class IHyAudio<IHyLoadable2d, HyEntity2d>;
template class IHyAudio<IHyLoadable3d, HyEntity3d>;
