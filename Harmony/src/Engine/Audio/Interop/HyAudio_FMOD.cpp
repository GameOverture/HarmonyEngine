/**************************************************************************
*	HyAudio_FMOD.h
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/Interop/HyAudio_FMOD.h"

#if defined(HY_FMOD)

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line)
{
	if (result != FMOD_OK)
		HyError(file << "(" << line << "): FMOD error " << result << " - " << FMOD_ErrorString(result));
}

HyAudio_FMOD::HyAudio_FMOD() :
	m_pSystem(nullptr)
{
	ERRCHECK(FMOD::Studio::System::create(&m_pSystem));

	// The example Studio project is authored for 5.1 sound, so set up the system output mode to match
	FMOD::System *pCoreSystem = nullptr;
	ERRCHECK(system->getCoreSystem(&pCoreSystem));
	ERRCHECK(pCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0) );
}

HyAudio_FMOD::~HyAudio_FMOD()
{
}

#endif // HY_FMOD
