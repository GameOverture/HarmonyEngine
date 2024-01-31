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
#include "HyEngine.h"

HyAudioData::HyAudioData(const std::string &sPath, HyJsonObj itemObj, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_pAudioStates(nullptr),
	m_pAuxiliaryFile(nullptr)
{
	HyJsonArray audioStateArray = itemObj["stateArray"].GetArray();
	m_uiNumStates = audioStateArray.Size();
	m_pAudioStates = HY_NEW AudioState[m_uiNumStates];

	for(uint32 i = 0; i < m_uiNumStates; ++i)
	{
		HyJsonObj stateObj = audioStateArray[i].GetObject();

		HyJsonArray playListArray = stateObj["playlist"].GetArray();
		for(uint32 j = 0; j < playListArray.Size(); ++j)
		{
			HyJsonObj playListItemObj = playListArray[j].GetObject();
			uint32 uiChecksum = playListItemObj["checksum"].GetUint();
			uint32 uiBankId = playListItemObj["bankId"].GetUint();
			uint32 uiWeight = playListItemObj["weight"].GetUint();
			
			IHyFile *pAudioFile = assetsRef.GetFileWithAsset(HYFILE_AudioBank, uiChecksum, uiBankId);
			m_RequiredFiles[HYFILE_AudioBank].Set(pAudioFile->GetManifestIndex());

			m_pAudioStates[i].m_Playlist.emplace_back(uiChecksum, uiBankId, uiWeight);
		}

		m_pAudioStates[i].m_ePlaylistMode = static_cast<HyPlaylistMode>(stateObj["playlistMode"].GetInt());
		m_pAudioStates[i].m_fVolume = static_cast<float>(stateObj["volume"].GetDouble());
		m_pAudioStates[i].m_fPitch = static_cast<float>(stateObj["pitch"].GetDouble());
		m_pAudioStates[i].m_iPriority = stateObj["priority"].GetInt();
		m_pAudioStates[i].m_iLoops = stateObj["loops"].GetInt();
		m_pAudioStates[i].m_uiMaxDistance = stateObj["maxDist"].GetInt();
	}

	m_pSequentialCountList = HY_NEW std::vector<uint32>(m_uiNumStates, 0);
}

HyAudioData::HyAudioData(uint32 uiChecksum, uint32 uiBankId, HyAssets &assetsRef) :
	IHyNodeData(true),
	m_pAudioStates(nullptr),
	m_pAuxiliaryFile(static_cast<HyFileAudio *>(assetsRef.GetFileWithAsset(HYFILE_AudioBank, uiChecksum, uiBankId)))
{
	m_uiNumStates = 1;
	m_pAudioStates = HY_NEW AudioState[m_uiNumStates];

	uint32 uiWeight = 10;
	m_pAudioStates[0].m_Playlist.emplace_back(uiChecksum, uiBankId, uiWeight);
	m_pAudioStates[0].m_ePlaylistMode = HYPLAYLIST_Shuffle;
	m_pAudioStates[0].m_fVolume = 1.0f;
	m_pAudioStates[0].m_fPitch = 1.0f;
	m_pAudioStates[0].m_iPriority = 0;
	m_pAudioStates[0].m_iLoops = 0;
	m_pAudioStates[0].m_uiMaxDistance = 0;

	m_pSequentialCountList = HY_NEW std::vector<uint32>(m_uiNumStates, 0);
}

HyAudioData::HyAudioData(HyAuxiliaryFileHandle hFileHandle, std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit, int32 iCategoryId, HyAssets &assetsRef) :
	IHyNodeData(true),
	m_pAudioStates(nullptr),
	m_pAuxiliaryFile(HY_NEW HyFileAudio(hFileHandle, sFilePath, bIsStreaming, iInstanceLimit, iCategoryId, assetsRef.GetAudioCore()))
{
	assetsRef.SetAuxiliaryFile(hFileHandle, m_pAuxiliaryFile);

	m_uiNumStates = 1;
	m_pAudioStates = HY_NEW AudioState[m_uiNumStates];

	uint32 uiWeight = 10;
	m_pAudioStates[0].m_Playlist.emplace_back(0, hFileHandle, uiWeight);
	m_pAudioStates[0].m_ePlaylistMode = HYPLAYLIST_Shuffle;
	m_pAudioStates[0].m_fVolume = 1.0f;
	m_pAudioStates[0].m_fPitch = 1.0f;
	m_pAudioStates[0].m_iPriority = 0;
	m_pAudioStates[0].m_iLoops = 0;
	m_pAudioStates[0].m_uiMaxDistance = 0;

	m_pSequentialCountList = HY_NEW std::vector<uint32>(m_uiNumStates, 0);
}

HyAudioData::~HyAudioData(void)
{
	delete[] m_pAudioStates;
	delete m_pSequentialCountList;
}

/*virtual*/ IHyFile *HyAudioData::GetAuxiliaryFile() const /*override*/
{
	HyAssert(IsAuxiliary(), "HyAudioData::GetAuxiliaryFile() was called on an non-auxiliary object");
	return m_pAuxiliaryFile;
}

const HyAudioPlaylist &HyAudioData::GetPlaylist(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_Playlist;
}

HyPlaylistMode HyAudioData::GetPlaylistMode(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_ePlaylistMode;
}

int32 HyAudioData::GetPriority(uint32 uiStateIndex) const
{
	return m_pAudioStates[uiStateIndex].m_iPriority;
}

int32 HyAudioData::GetLoops(uint32 uiStateIndex) const
{
	if(m_pAudioStates[uiStateIndex].m_iLoops < 0 || m_pAudioStates[uiStateIndex].m_iLoops >= HYAUDIO_InfiniteLoops)
		return HYAUDIO_InfiniteLoops;
	
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

int32 HyAudioData::WeightedEntryPull(const HyAudioPlaylist &entriesList) const
{
	uint32 uiTotalWeight = 0;
	for(int j = 0; j < entriesList.size(); ++j)
		uiTotalWeight += entriesList[j].m_uiWeight;

	uint32 uiWeight = HyRand::Range(0u, uiTotalWeight);

	// If this rng exceeds the weight table max return the last valid entry (for instance all weights are zero)
	int32 iIndex = 0;
	if(uiWeight >= uiTotalWeight)
	{
		if(entriesList.empty())
			return -1; // Invalid

		iIndex = static_cast<int32>(entriesList.size()) - 1;
		while(entriesList[iIndex].m_uiWeight == 0 && iIndex > 0)
			iIndex--;
	}
	else // Normal lookup
	{
		uint32 tmpCount = entriesList[iIndex].m_uiWeight;
		while(tmpCount <= uiWeight && iIndex < entriesList.size())
		{
			iIndex++;
			tmpCount += entriesList[iIndex].m_uiWeight;
		}
	}

	return iIndex;
}

void HyAudioData::WeightedShuffle(uint32 uiStateIndex, std::vector<HyAudioHandle> &soundOrderListOut) const
{
	const HyAudioPlaylist &playListRef = GetPlaylist(uiStateIndex);
	HyAudioPlaylist tmpPlayList(playListRef);

	soundOrderListOut.clear();
	soundOrderListOut.reserve(playListRef.size());

	for(int i = 0; i < playListRef.size(); ++i)
	{
		int32 iIndex = WeightedEntryPull(tmpPlayList);

		soundOrderListOut.push_back(tmpPlayList[iIndex].m_hAudioHandle);
		tmpPlayList.erase(tmpPlayList.begin() + iIndex);
	}
}

HyAudioHandle HyAudioData::GetNextSequential(uint32 uiStateIndex) const
{
	HyAudioHandle hNextSound = m_pAudioStates[uiStateIndex].m_Playlist[(*m_pSequentialCountList)[uiStateIndex]].m_hAudioHandle;

	(*m_pSequentialCountList)[uiStateIndex]++;
	if((*m_pSequentialCountList)[uiStateIndex] >= m_pAudioStates[uiStateIndex].m_Playlist.size())
		(*m_pSequentialCountList)[uiStateIndex] = 0;

	return hNextSound;
}
