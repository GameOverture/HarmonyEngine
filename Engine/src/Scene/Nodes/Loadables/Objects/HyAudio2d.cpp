/**************************************************************************
 *	HyAudio2d.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Scene/Nodes/Loadables/Objects/HyAudio2d.h"
#include "Assets/HyAssets.h"
#include "HyEngine.h"

HyAudio2d::HyAudio2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable2d, HyEntity2d>(sPrefix, sName, pParent)
{
}

HyAudio2d::HyAudio2d(uint32 uiSoundChecksum, uint32 uiBankId, HyEntity2d *pParent) :
	IHyAudio<IHyLoadable2d, HyEntity2d>(uiSoundChecksum, uiBankId, pParent)
{
}

HyAudio2d::HyAudio2d(HyAudioHandle hAudioHandle, HyEntity2d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable2d, HyEntity2d>(hAudioHandle, pParent)
{
}

HyAudio2d::HyAudio2d(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit /*= 0*/, int32 iCategoryId /*= 0*/, HyEntity2d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable2d, HyEntity2d>(sFilePath, bIsStreaming, iInstanceLimit, iCategoryId, pParent)
{
}

/*virtual*/ HyAudio2d::~HyAudio2d(void)
{
}

void HyAudio2d::Init(uint32 uiAudioChecksum, uint32 uiBankId, HyEntity2d *pParent)
{
	m_uiFlags |= SETTING_IsAuxiliary;
	IHyLoadable2d::Init(std::to_string(uiAudioChecksum), std::to_string(uiBankId), pParent);
}

void HyAudio2d::Init(HyAudioHandle hAudioHandle, HyEntity2d *pParent)
{
	m_uiFlags |= SETTING_IsAuxiliary;
	IHyLoadable2d::Init(std::to_string(hAudioHandle.first), std::to_string(hAudioHandle.second), pParent);
}

void HyAudio2d::Init(std::string sFilePath, bool bIsStreaming, int32 iInstanceLimit, int32 iCategoryId, HyEntity2d *pParent)
{
	m_uiFlags |= SETTING_IsAuxiliary;
	HyAudioHandle hAudioHandle = HyEngine::CreateAudio(HyIO::CleanPath(sFilePath.c_str(), nullptr, false), bIsStreaming, iInstanceLimit, iCategoryId);
	IHyLoadable2d::Init(std::to_string(hAudioHandle.first), std::to_string(hAudioHandle.second), pParent);
}
