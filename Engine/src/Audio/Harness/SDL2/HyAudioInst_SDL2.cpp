/**************************************************************************
*	HyAudioInst_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Audio/Harness/SDL2/HyAudioInst_SDL2.h"

HyAudioInst_SDL2::HyAudioInst_SDL2()
{
}

/*virtual*/ HyAudioInst_SDL2::~HyAudioInst_SDL2()
{
}

/*virtual*/ void HyAudioInst_SDL2::OnLoaded() /*override*/
{
}

/*virtual*/ void HyAudioInst_SDL2::PlayOneShot() /*override*/
{
}

/*virtual*/ void HyAudioInst_SDL2::Start() /*override*/
{
}

/*virtual*/ void HyAudioInst_SDL2::Stop(HyAudioStop eStopType /*= HYAUDIOSTOP_AllowFadeOut*/) /*override*/
{
}

/*virtual*/ bool HyAudioInst_SDL2::IsPaused() const /*override*/
{
	return false;
}

/*virtual*/ void HyAudioInst_SDL2::SetPause(bool bPause) /*override*/
{
}

/*virtual*/ float HyAudioInst_SDL2::GetVolume(float *fFinalVolumeOut /*= nullptr*/) const /*override*/
{
	return 0.0f;
}

/*virtual*/ void HyAudioInst_SDL2::SetVolume(float fVolume) /*override*/
{
}

/*virtual*/ float HyAudioInst_SDL2::GetPitch(float *fFinalPitchOut /*= nullptr*/) const /*override*/
{
	return 0.0f;
}

/*virtual*/ void HyAudioInst_SDL2::SetPitch(float fPitch) /*override*/
{
}

/*virtual*/ int HyAudioInst_SDL2::GetTimelinePosition() const /*override*/
{
	return 0;
}

/*virtual*/ void HyAudioInst_SDL2::SetTimelinePosition(int iPosition) /*override*/
{
}

/*virtual*/ float HyAudioInst_SDL2::GetParam(const char *szParam, float *fFinalValueOut /*= nullptr*/) const /*override*/
{
	return 0.0f;
}

/*virtual*/ void HyAudioInst_SDL2::SetParam(const char *szParam, float fValue) /*override*/
{
}

/*virtual*/ float HyAudioInst_SDL2::GetReverb(int iIndex) const /*override*/
{
	return 0.0f;
}

/*virtual*/ void HyAudioInst_SDL2::SetReverb(int iIndex, float fLevel) /*override*/
{
}
