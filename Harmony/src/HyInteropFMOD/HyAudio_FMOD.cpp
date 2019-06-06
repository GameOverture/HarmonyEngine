/**************************************************************************
*	HyAudio_FMOD.cpp
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "stdafx.h"

#include "HyAudio_FMOD.h"

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line)
{
	//if(result != FMOD_OK)
	//	HyError(file << "(" << line << "): FMOD error " << result << " - " << FMOD_ErrorString(result));
}


HyAudio_FMOD::HyAudio_FMOD() :
	m_pSystem(nullptr)
{
	ERRCHECK(FMOD::Studio::System::create(&m_pSystem));

	// The example Studio project is authored for 5.1 sound, so set up the system output mode to match
	FMOD::System *pCoreSystem = nullptr;
	ERRCHECK(m_pSystem->getCoreSystem(&pCoreSystem));
	ERRCHECK(pCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0) );
}

/*virtual*/ HyAudio_FMOD::~HyAudio_FMOD()
{
}

/*virtual*/ void HyAudio_FMOD::OnUpdate() /*override*/
{
}

HyAudioData_FMOD::HyAudioData_FMOD()
{
}

/*virtual*/ HyAudioData_FMOD::~HyAudioData_FMOD()
{
}
