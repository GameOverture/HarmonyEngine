/**************************************************************************
*	HyAudioBank.cpp
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyAudioBank.h"
#include "HyEngine.h"

extern std::string Hy_DataDir();

HyAudioBank::HyAudioBank(std::string sFilePath, IHyAudioBank *pInternal) :
	IHyFileData(sFilePath, HYFILE_AudioBank),
	m_pInternal(pInternal)
{
	HyAssert(m_pInternal != nullptr, "HyAudioBank received a nullptr for its internal interface");
}

HyAudioBank::~HyAudioBank()
{
	delete m_pInternal;
}

/*virtual*/ void HyAudioBank::OnLoadThread() /*override*/
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		std::string sFilePath = Hy_DataDir() + HYASSETS_AudioDir + m_sFILE_NAME;
		if(m_pInternal->Load(sFilePath) == false)
		{
			HyLogError("HyAudioBank::OnLoadThread() failed");
			return;
		}
	}
	else
		m_pInternal->Unload();
}

/*virtual*/ void HyAudioBank::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
}
