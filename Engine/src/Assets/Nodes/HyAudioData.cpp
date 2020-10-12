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
#include "Diagnostics/Console/HyConsole.h"

HyAudioData::HyAudioData(const std::string &sPath, HyJsonObj itemObj, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_AudioRef(assetsRef.GetAudioRef()),
	m_ePlayListMode(HYPLAYLIST_Unknown)
{
	HyJsonArray audioStateArray = itemObj["stateArray"].GetArray();
	for(uint32 i = 0; i < audioStateArray.Size(); ++i)
	{
		HyJsonObj stateObj = audioStateArray[i].GetObject();

		HyJsonArray playListArray = stateObj["playList"].GetArray();
		for(uint32 i = 0; i < playListArray.Size(); ++i)
		{
			HyJsonObj playListItemObj = playListArray[i].GetObject();
			uint32 uiChecksum = playListItemObj["checksum"].GetUint();
			IHyFile *pAudioFile = assetsRef.GetFileWithAsset(HYFILE_AudioBank, uiChecksum);
		
			m_RequiredAudio.Set(pAudioFile->GetManifestIndex());
			m_PlayList.push_back(uiChecksum);
		}

		// Init first state
		if(i == 0)
		{
			m_ePlayListMode = static_cast<HyPlayListMode>(stateObj["playListMode"].GetInt());
			HyAssert(m_ePlayListMode != HYPLAYLIST_Unknown, "HyAudioData " << sPath << " has unknown Play List Mode");

			m_fVolume = static_cast<float>(stateObj["volume"].GetDouble());
			m_fPitch = static_cast<float>(stateObj["pitch"].GetDouble());
			m_iPriority = stateObj["priority"].GetInt();
			m_iLoops = stateObj["loops"].GetInt();
			m_uiMaxDistance = stateObj["maxDist"].GetInt();
		}
	}
}

HyAudioData::~HyAudioData(void)
{
}

HyPlayListMode HyAudioData::GetPlayListMode() const
{
	return m_ePlayListMode;
}

int32 HyAudioData::GetPriority() const
{
	return m_iPriority;
}

int32 HyAudioData::GetLoops() const
{
	return m_iLoops;
}

uint32 HyAudioData::GetMaxDistance() const
{
	return m_uiMaxDistance;
}

float HyAudioData::GetVolume() const
{
	return m_fVolume;
}

float HyAudioData::GetPitch() const
{
	return m_fPitch;
}

uint32 HyAudioData::GetSound(IHyNode *pAudioNode) const
{
	// TODO: Implement this!

	switch(m_ePlayListMode)
	{
	case HYPLAYLIST_Shuffle:
		return m_PlayList[0];

	default:
		HyError("HyAudioData::GetSound - Unhandled cue type");
		return 0;
	}
}
