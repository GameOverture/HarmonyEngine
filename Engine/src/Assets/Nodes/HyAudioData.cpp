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
		for(uint32 j = 0; j < playListArray.Size(); ++j)
		{
			HyJsonObj playListItemObj = playListArray[j].GetObject();
			uint32 uiChecksum = playListItemObj["checksum"].GetUint();
			uint32 uiWeight = playListItemObj["weight"].GetUint();
			IHyFile *pAudioFile = assetsRef.GetFileWithAsset(HYFILE_AudioBank, uiChecksum);
		
			m_RequiredAudio.Set(pAudioFile->GetManifestIndex());
			m_pAudioStates[i].m_PlayList.push_back(std::pair<uint32, uint32>(uiChecksum, uiWeight));
		}

		m_pAudioStates[i].m_ePlayListMode = static_cast<HyPlayListMode>(stateObj["playListMode"].GetInt());
		m_pAudioStates[i].m_fVolume = static_cast<float>(stateObj["volume"].GetDouble());
		m_pAudioStates[i].m_fPitch = static_cast<float>(stateObj["pitch"].GetDouble());
		m_pAudioStates[i].m_iPriority = stateObj["priority"].GetInt();
		m_pAudioStates[i].m_iLoops = stateObj["loops"].GetInt();
		m_pAudioStates[i].m_uiMaxDistance = stateObj["maxDist"].GetInt();
	}

	m_pSequentialCountList = HY_NEW std::vector<uint32>(m_uiNumStates, 0);
}

HyAudioData::HyAudioData(HyAudioHandle hAudioHandle) :
	IHyNodeData(HYASSETS_Hotload),
	m_pAudioStates(nullptr)
{
	m_uiNumStates = 1;
	m_pAudioStates = HY_NEW AudioState[m_uiNumStates];

	for(uint32 i = 0; i < m_uiNumStates; ++i)
	{
		uint32 uiChecksum = hAudioHandle; // Utilize the checksum to hold the HyAudioHandle
		uint32 uiWeight = 1;

		m_pAudioStates[i].m_PlayList.push_back(std::pair<uint32, uint32>(uiChecksum, uiWeight));
		m_pAudioStates[i].m_ePlayListMode = HYPLAYLIST_Shuffle;
		m_pAudioStates[i].m_fVolume = 1.0f;
		m_pAudioStates[i].m_fPitch = 0.0f;
		m_pAudioStates[i].m_iPriority = 0;
		m_pAudioStates[i].m_iLoops = 0;
		m_pAudioStates[i].m_uiMaxDistance = 0;
	}

	m_pSequentialCountList = HY_NEW std::vector<uint32>(m_uiNumStates, 0);
}

HyAudioData::~HyAudioData(void)
{
	delete[] m_pAudioStates;
	delete m_pSequentialCountList;
}

const HyAudioPlayList &HyAudioData::GetPlayList(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_PlayList;
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

uint32 HyAudioData::GetNextSequential(uint32 uiStateIndex) const
{
	uint32 uiNextSound = m_pAudioStates[uiStateIndex].m_PlayList[(*m_pSequentialCountList)[uiStateIndex]].first;
	(*m_pSequentialCountList)[uiStateIndex]++;
	if((*m_pSequentialCountList)[uiStateIndex] >= m_pAudioStates[uiStateIndex].m_PlayList.size())
		(*m_pSequentialCountList)[uiStateIndex] = 0;

	return uiNextSound;
}
