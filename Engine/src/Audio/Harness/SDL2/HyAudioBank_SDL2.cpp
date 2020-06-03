/**************************************************************************
*	HyAudioBank_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Audio/Harness/SDL2/HyAudioBank_SDL2.h"

HyAudioBank_SDL2::HyAudioBank_SDL2()
{
}

/*virtual*/ HyAudioBank_SDL2::~HyAudioBank_SDL2()
{
}

/*virtual*/ bool HyAudioBank_SDL2::Load(std::string sFilePath) /*override*/
{
	return true;
}
