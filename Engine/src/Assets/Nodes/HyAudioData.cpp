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

HyAudioData::HyAudioData(const std::string &sPath, HyJsonObj &itemObjRef, HyAssets &assetsRef) :
	IHyNodeData(sPath),
	m_AudioRef(assetsRef.GetAudioRef()),
	m_eCueType(CUETYPE_Unknown)
{
	HyJsonArray assetsArray = itemObjRef["assets"].GetArray();
	for(uint32 i = 0; i < assetsArray.Size(); ++i)
	{
		uint32 uiChecksum = assetsArray[i].GetUint();
		IHyFile *pAudioFile = assetsRef.GetFileWithAsset(HYFILE_AudioBank, uiChecksum);
		
		m_RequiredAudio.Set(pAudioFile->GetManifestIndex());
		m_SoundChecksumList.push_back(uiChecksum);
	}

	std::string sType = itemObjRef["type"].GetString();
	std::transform(sType.begin(), sType.end(), sType.begin(), ::tolower);
	if(sType == "single")
		m_eCueType = CUETYPE_Single;

	if(m_eCueType == CUETYPE_Unknown)
		HyLogWarning("HyAudioData has unknown cue type");
}

HyAudioData::~HyAudioData(void)
{
}

uint32 HyAudioData::GetSound() const
{
	switch(m_eCueType)
	{
	case CUETYPE_Single:
		return m_SoundChecksumList[0];

	default:
		HyError("HyAudioData::GetSound - Unhandled cue type");
		return 0;
	}
}
