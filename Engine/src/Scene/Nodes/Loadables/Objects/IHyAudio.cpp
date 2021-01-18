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
#include "Audio/HyAudioHarness.h"
#include "Scene/HyScene.h"

template<typename NODETYPE, typename ENTTYPE>
IHyAudio<NODETYPE, ENTTYPE>::IHyAudio(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, ENTTYPE *pParent /*= nullptr*/) :
	NODETYPE(HYTYPE_Audio, sPrefix, sName, pParent),
	m_uiCueFlags(0),
	m_fVolume(1.0f),
	m_fPitch(1.0f),
	m_ePlayListMode(HYPLAYLIST_Unknown),
	m_iPriority(0),
	m_iLoops(0),
	m_uiMaxDistance(0),
	volume(m_fVolume, *this, 0),
	pitch(m_fPitch, *this, 0)
{
}

// TODO: copy ctor and move ctor
template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ IHyAudio<NODETYPE, ENTTYPE>::~IHyAudio(void)
{
}

// TODO: assignment operator and move operator
template<typename NODETYPE, typename ENTTYPE>
int32 IHyAudio<NODETYPE, ENTTYPE>::GetLoops() const
{
	return m_iLoops;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::SetLoops(int32 iLoops)
{
	m_iLoops = iLoops;
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::PlayOneShot(bool bUseCurrentSettings /*= true*/)
{
	if(bUseCurrentSettings)
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_PlayOneShot);
	else
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_PlayOneShotDefault);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::Play()
{
	m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Stop);
	m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Start);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::Stop()
{
	m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Start);
	m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Stop);
}

template<typename NODETYPE, typename ENTTYPE>
void IHyAudio<NODETYPE, ENTTYPE>::SetPause(bool bPause)
{
	if(bPause)
	{
		m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Unpause);
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Pause);
	}
	else
	{
		m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Pause);
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Unpause);
	}
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ bool IHyAudio<NODETYPE, ENTTYPE>::IsLoadDataValid() /*override*/
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->AcquireData());
	return pData && pData->GetNumStates() != 0;
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyAudio<NODETYPE, ENTTYPE>::OnDataAcquired() /*override*/
{
	const HyAudioData *pData = static_cast<const HyAudioData *>(this->UncheckedGetData());

	m_ePlayListMode = pData->GetPlayListMode(0);
	m_iPriority = pData->GetPriority(0);
	m_iLoops = pData->GetLoops(0);
	m_uiMaxDistance = pData->GetMaxDistance(0);
	volume = pData->GetVolume(0);
	pitch = pData->GetPitch(0);
}

template<typename NODETYPE, typename ENTTYPE>
/*virtual*/ void IHyAudio<NODETYPE, ENTTYPE>::OnLoadedUpdate() /*override*/
{
	if(IHyNode::IsDirty(IHyNode::DIRTY_Audio))
	{
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Attributes);
		IHyNode::ClearDirty(IHyNode::DIRTY_Audio);
	}

	if(m_uiCueFlags)
	{
		for(uint32 i = 0; i < IHyAudioCore::NUM_CUETYPES; ++i)
		{
			if(0 != (m_uiCueFlags & (1 << i)))
				IHyNode::sm_pScene->AppendAudioCue(this, static_cast<IHyAudioCore::CueType>(i));
		}

		m_uiCueFlags = 0;
	}
}

template class IHyAudio<IHyLoadable2d, HyEntity2d>;
template class IHyAudio<IHyLoadable3d, HyEntity3d>;
