/**************************************************************************
*	HyFileAudio.cpp
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Assets/Files/HyFileAudio.h"
#include "HyEngine.h"

extern std::string Hy_DataDir();

HyFileAudio::HyFileAudio(std::string sFilePath, uint32 uiManifestIndex, IHyFileAudioGuts *pInternal) :
	IHyFile(sFilePath, HYFILE_AudioBank, uiManifestIndex),
	m_pInternal(pInternal)
{
	HyAssert(m_pInternal != nullptr, "HyFileAudio received a nullptr for its internal interface");
}

HyFileAudio::~HyFileAudio()
{
	delete m_pInternal;
}

bool HyFileAudio::ContainsAsset(uint32 uiAssetChecksum)
{
	return m_pInternal->ContainsAsset(uiAssetChecksum);
}

/*virtual*/ void HyFileAudio::OnLoadThread() /*override*/
{
	if(GetLoadableState() == HYLOADSTATE_Queued)
	{
		std::string sFilePath = Hy_DataDir();
		sFilePath += m_sFILE_NAME;
		if(m_pInternal->Load(sFilePath) == false)
		{
			HyLogError("HyFileAudio::OnLoadThread() failed");
			return;
		}
	}
	else
		m_pInternal->Unload();
}

/*virtual*/ void HyFileAudio::OnRenderThread(IHyRenderer &rendererRef) /*override*/
{
}
