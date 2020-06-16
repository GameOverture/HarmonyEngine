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

extern std::string Hy_DataDir();

HyAudioBank::HyAudioBank(const std::string &sDataDir, const std::string &sNameId, const jsonxx::Object &initObj, IHyAudioBank *pInternal) :
	IHyFileData(sDataDir, HYFILE_AudioBank),
	m_sNAME_ID(sNameId),
	m_sPATH(initObj.get<jsonxx::String>("filePath")),
	m_bIS_MASTER(initObj.get<jsonxx::Boolean>("master")),
	m_pInternal(pInternal)
{
	HyAssert(m_pInternal != nullptr, "HyAudioBank received a nullptr for its internal interface");

	if(m_bIS_MASTER)
	{
		if(m_pInternal->Load(sDataDir + HYASSETS_AudioDir + m_sPATH) == false)
			HyLogError("HyAudioBank internal Load() failed on master bank: " << m_sPATH);
	}
}

HyAudioBank::~HyAudioBank()
{
	delete m_pInternal;
}

bool HyAudioBank::IsMaster() const
{
	return m_bIS_MASTER;
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
