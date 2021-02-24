/**************************************************************************
 *	HyAudioData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Nodes/HyAudioData.h"
#include "Assets/Files/HyFileAudio.h"
#include "Audio/HyAudioHarness.h"
#include "Diagnostics/Console/IHyConsole.h"

HyAudioData::HyAudioData(const std::string &sPath, HyJsonObj itemObj, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_pAudioStates(nullptr)
{
	HyJsonArray audioStateArray = itemObj["stateArray"].GetArray();

	m_uiNumStates = audioStateArray.Size();
	m_pAudioStates = HY_NEW AudioState[m_uiNumStates];
	for(uint32 i = 0; i < m_uiNumStates; ++i)
	{
		HyJsonObj stateObj = audioStateArray[i].GetObject();

		HyJsonArray playListArray = stateObj["playList"].GetArray();
		for(uint32 i = 0; i < playListArray.Size(); ++i)
		{
			HyJsonObj playListItemObj = playListArray[i].GetObject();
			uint32 uiChecksum = playListItemObj["checksum"].GetUint();
			IHyFile *pAudioFile = assetsRef.GetFileWithAsset(HYFILE_AudioBank, uiChecksum);
		
			m_RequiredAudio.Set(pAudioFile->GetManifestIndex());
			m_pAudioStates[i].m_PlayList.push_back(uiChecksum);
		}

		m_pAudioStates[i].m_ePlayListMode = static_cast<HyPlayListMode>(stateObj["playListMode"].GetInt());
		HyAssert(m_pAudioStates[i].m_ePlayListMode != HYPLAYLIST_Unknown, "HyAudioData " << sPath << " has unknown Play List Mode");

		m_pAudioStates[i].m_fVolume = static_cast<float>(stateObj["volume"].GetDouble());
		m_pAudioStates[i].m_fPitch = static_cast<float>(stateObj["pitch"].GetDouble());
		m_pAudioStates[i].m_iPriority = stateObj["priority"].GetInt();
		m_pAudioStates[i].m_iLoops = stateObj["loops"].GetInt();
		m_pAudioStates[i].m_uiMaxDistance = stateObj["maxDist"].GetInt();
	}
}

HyAudioData::~HyAudioData(void)
{
	delete[] m_pAudioStates;
}

HyPlayListMode HyAudioData::GetPlayListMode(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_ePlayListMode;
}

int32 HyAudioData::GetPriority(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_iPriority;
}

int32 HyAudioData::GetLoops(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_iLoops;
}

uint32 HyAudioData::GetMaxDistance(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_uiMaxDistance;
}

float HyAudioData::GetVolume(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_fVolume;
}

float HyAudioData::GetPitch(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_fPitch;
}

uint32 HyAudioData::GetSound(IHyNode *pAudioNode) const
{
	// TODO: Implement this!

	switch(m_pAudioStates[0/*pAudioNode->GetState()*/].m_ePlayListMode)
	{
	case HYPLAYLIST_Shuffle:
		return m_pAudioStates[0/*pAudioNode->GetState()*/].m_PlayList[0];

	default:
		HyError("HyAudioData::GetSound - Unhandled cue type");
		return 0;
	}
}
