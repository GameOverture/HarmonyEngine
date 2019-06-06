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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyAudio_FMOD::HyAudio_FMOD() :
	m_pSystem(nullptr)
{
	ERRCHECK(Studio::System::create(&m_pSystem));

	// The example Studio project is authored for 5.1 sound, so set up the system output mode to match
	System *pCoreSystem = nullptr;
	ERRCHECK(m_pSystem->getCoreSystem(&pCoreSystem));
	ERRCHECK(pCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0));

	// Initialize
	ERRCHECK(m_pSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr));
}

/*virtual*/ HyAudio_FMOD::~HyAudio_FMOD()
{
	ERRCHECK(m_pSystem->release());
}

/*virtual*/ void HyAudio_FMOD::OnUpdate() /*override*/
{
	ERRCHECK(m_pSystem->update());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
HyAudioBank_FMOD::HyAudioBank_FMOD(Studio::System *pSystemRef) :
	m_pSystemRef(pSystemRef),
	m_pBank(nullptr)
{
}

/*virtual*/ HyAudioBank_FMOD::~HyAudioBank_FMOD()
{
	ERRCHECK(m_pBank->unload());
}

/*virtual*/ bool HyAudioBank_FMOD::Load(std::string sFilePath) /*override*/
{
	ERRCHECK(m_pSystemRef->loadBankFile(sFilePath.c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &m_pBank));
	return true;
}
