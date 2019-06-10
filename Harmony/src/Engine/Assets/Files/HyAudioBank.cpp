/**************************************************************************
*	HyAudioBank.cpp
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyAudioBank.h"
#include "HyEngine.h"

HyAudioBank::HyAudioBank(const std::string &sNameId, const jsonxx::Object &initObj) :
	IHyFileData(HYFILE_AudioBank),
	m_sNAME_ID(sNameId),
	m_sPATH(initObj.get<jsonxx::String>("filePath")),
	m_bIS_MASTER(initObj.get<jsonxx::Boolean>("master"))
{
	m_pInternal = HyAudio::CreateAudioBank();
}

HyAudioBank::~HyAudioBank()
{
	delete m_pInternal;
}

/*virtual*/ void HyAudioBank::OnLoadThread() /*override*/
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		std::string sFilePath = Hy_DataDir() + HYASSETS_AudioDir + m_sPATH;
		if(m_pInternal->Load(sFilePath) == false)
		{
			HyLogError("HyAudioBank::OnLoadThread() failed");
			return;
		}
	}
}

/*virtual*/ void HyAudioBank::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
}
