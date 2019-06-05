/**************************************************************************
*	HyAudio_FMOD.h
*
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudio_FMOD_h__
#define HyAudio_FMOD_h__

#include "Afx/HyStdAfx.h"

#if defined(HY_FMOD)
#include "Audio/IHyAudio.h"

#include "3rdParty/FMOD/fmod_studio.hpp"
#include "3rdParty/FMOD/fmod.hpp"
#include "3rdParty/FMOD/fmod_errors.h"

void ERRCHECK_fn(FMOD_RESULT result, const char *file, int line);
#define ERRCHECK(_result) ERRCHECK_fn(_result, __FILE__, __LINE__)

class HyAudio_FMOD : public IHyAudio
{
	FMOD::Studio::System *		m_pSystem;

public:
	HyAudio_FMOD();
	virtual ~HyAudio_FMOD();
};

#endif // HY_FMOD

#endif /* HyAudio_FMOD_h__ */
