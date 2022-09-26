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

//HySoundBuffer_miniaudio::HySoundBuffer_miniaudio(IHyAudioCore &coreRef, std::string sFileName, int32 iGroupId, bool bIsStreaming, int32 iInstanceLimit) :
//	IHySoundBuffer(coreRef, sFileName, iGroupId, bIsStreaming, iInstanceLimit)
//{
//}
//
///*virtual*/ HySoundBuffer_miniaudio::~HySoundBuffer_miniaudio()
//{
//	Unload();
//}
//
///*virtual*/ bool HySoundBuffer_miniaudio::Load(std::string sFilePath) /*override*/
//
//}
//
///*virtual*/ void HySoundBuffer_miniaudio::Unload() /*override*/
//{
//	
//}
//
//ma_sound &HySoundBuffer_miniaudio::GetSound()
//{
//	return m_Sound;
//}
