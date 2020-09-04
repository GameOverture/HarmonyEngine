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
	m_ePlayListMode(PLAYLIST_Unknown)
{
	HyJsonArray assetsArray = itemObj["assets"].GetArray();
	for(uint32 i = 0; i < assetsArray.Size(); ++i)
	{
		uint32 uiChecksum = assetsArray[i].GetUint();
		IHyFile *pAudioFile = assetsRef.GetFileWithAsset(HYFILE_AudioBank, uiChecksum);
		
		m_RequiredAudio.Set(pAudioFile->GetManifestIndex());
		m_SoundChecksumList.push_back(uiChecksum);
	}

	std::string sType = itemObj["playListMode"].GetString();
	std::transform(sType.begin(), sType.end(), sType.begin(), ::tolower);
	if(sType == "shuffle")
		m_ePlayListMode = PLAYLIST_Shuffle;
	else if(sType == "weighted")
		m_ePlayListMode = PLAYLIST_Weighted;
	else if(sType == "sequentialLocal")
		m_ePlayListMode = PLAYLIST_SequentialLocal;
	else if(sType == "sequentialGlobal")
		m_ePlayListMode = PLAYLIST_SequentialGlobal;

	HyAssert(m_ePlayListMode != PLAYLIST_Unknown, "HyAudioData " << sPath << " has unknown Play List Mode");
}

HyAudioData::~HyAudioData(void)
{
}

uint32 HyAudioData::GetSound(IHyNode *pAudioNode) const
{
	switch(m_ePlayListMode)
	{
	case PLAYLIST_Shuffle:
		return m_SoundChecksumList[0];

	default:
		HyError("HyAudioData::GetSound - Unhandled cue type");
		return 0;
	}
}
