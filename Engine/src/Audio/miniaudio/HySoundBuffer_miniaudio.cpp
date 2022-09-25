/**************************************************************************
*	HySoundBuffer_miniaudio.cpp
*
*	Harmony Engine
*	Copyright (c) 2022 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Audio/miniaudio/HySoundBuffer_miniaudio.h"
#include "Audio/miniaudio/HyAudioCore_miniaudio.h"
#include "Diagnostics/Console/IHyConsole.h"
#include "Utilities/HyIO.h"

HySoundBuffer_miniaudio::HySoundBuffer_miniaudio(IHyAudioCore &coreRef, std::string sFileName, int32 iGroupId, bool bIsStreaming, int32 iInstanceLimit) :
	IHySoundBuffer(coreRef, sFileName, iGroupId, bIsStreaming, iInstanceLimit)
{
}

/*virtual*/ HySoundBuffer_miniaudio::~HySoundBuffer_miniaudio()
{
	Unload();
}

/*virtual*/ bool HySoundBuffer_miniaudio::Load(std::string sFilePath) /*override*/
{
	std::string sFullPath = sFilePath;
	sFullPath += "/";
	sFullPath += m_sFILE_NAME;

	ma_result eResult = ma_sound_init_from_file(&static_cast<HyAudioCore_miniaudio &>(m_CoreRef).GetEngine(),
												sFullPath.c_str(),
												m_bIS_STREAMING ? MA_SOUND_FLAG_STREAM : 0,
												&static_cast<HyAudioCore_miniaudio &>(m_CoreRef).GetGroup(GetGroupId()),//pGroup,
												nullptr,
												&m_Sound);
	if(eResult != MA_SUCCESS)
	{
		HyLogError("HySoundBuffer_miniaudio::Load failed: " << eResult);
		return false;  // Failed to load sound.
	}

	return true;
}

/*virtual*/ void HySoundBuffer_miniaudio::Unload() /*override*/
{
	ma_sound_uninit(&m_Sound);
}

ma_sound &HySoundBuffer_miniaudio::GetSound()
{
	return m_Sound;
}
