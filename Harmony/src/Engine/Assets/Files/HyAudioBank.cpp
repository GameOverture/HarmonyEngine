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

HyAudioBank::HyAudioBank(const std::string &sFileName) :
	IHyFileData(HYFILE_AudioBank),
	m_sFILENAME(sFileName)
{
}

HyAudioBank::~HyAudioBank()
{
}

/*virtual*/ void HyAudioBank::OnLoadThread() /*override*/
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		std::string sFilePath = Hy_DataDir() + HYASSETS_AudioDir + m_sFILENAME;
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
