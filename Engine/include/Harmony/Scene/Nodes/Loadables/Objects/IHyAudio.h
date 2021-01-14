/**************************************************************************
 *	IHyAudio.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2016 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef IHyAudio_h__
#define IHyAudio_h__

#include "Afx/HyInteropAfx.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Audio/HyAudioHarness.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Scene/HyScene.h"

template<typename NODETYPE, typename ENTTYPE>
class IHyAudio : public NODETYPE
{
	uint32						m_uiCueFlags;
	float						m_fVolume;
	float						m_fPitch;
	
	// Configurable
	HyPlayListMode				m_ePlayListMode;
	int32						m_iPriority;
	int32						m_iLoops;
	uint32						m_uiMaxDistance;

public:
	HyAnimFloat					volume;
	HyAnimFloat					pitch;

public:
	IHyAudio(std::string sPrefix = "", std::string sName = "", ENTTYPE *pParent = nullptr) :
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
	virtual ~IHyAudio(void)
	{
	}
	// TODO: assignment operator and move operator

	int32 GetLoops() const
	{
		return m_iLoops;
	}
	void SetLoops(int32 iLoops)
	{
		m_iLoops = iLoops;
	}

	void PlayOneShot(bool bUseCurrentSettings /*= true*/)
	{
		if(bUseCurrentSettings)
			m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_PlayOneShot);
		else
			m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_PlayOneShotDefault);
	}

	void Play()
	{
		m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Stop);
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Start);
	}

	void Stop()
	{
		m_uiCueFlags &= ~(1 << IHyAudioCore::CUETYPE_Start);
		m_uiCueFlags |= (1 << IHyAudioCore::CUETYPE_Stop);
	}

	void SetPause(bool bPause)
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

	virtual bool IsLoadDataValid() override
	{
		const HyAudioData *pData = static_cast<const HyAudioData *>(this->AcquireData());
		return pData && pData->GetNumStates() != 0;
	}

protected:
	virtual void OnDataAcquired() override
	{
		const HyAudioData *pData = static_cast<const HyAudioData *>(this->UncheckedGetData());

		m_ePlayListMode = pData->GetPlayListMode(0);
		m_iPriority = pData->GetPriority(0);
		m_iLoops = pData->GetLoops(0);
		m_uiMaxDistance = pData->GetMaxDistance(0);
		volume = pData->GetVolume(0);
		pitch = pData->GetPitch(0);
	}

	virtual void OnLoadedUpdate() override
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
};

#endif /* IHyAudio_h__ */
