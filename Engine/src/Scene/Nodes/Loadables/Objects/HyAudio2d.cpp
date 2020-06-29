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
#include "Audio/HyAudioHarness.h"
#include "Assets/Nodes/HyAudioData.h"

HyAudio2d::HyAudio2d(std::string sPrefix /*= ""*/, std::string sName /*= ""*/, HyEntity2d *pParent /*= nullptr*/) :
	IHyAudio<IHyLoadable2d, HyEntity2d>(sPrefix, sName, pParent)
{
}

/*virtual*/ HyAudio2d::~HyAudio2d(void)
{
}
