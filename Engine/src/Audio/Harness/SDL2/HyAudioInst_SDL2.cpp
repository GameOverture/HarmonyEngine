/**************************************************************************
*	HyAudioInst_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Audio/Harness/SDL2/HyAudioInst_SDL2.h"
#include "Audio/Harness/SDL2/HyAudio_SDL2.h"
#include "Diagnostics/Console/HyConsole.h"

#if defined(HY_USE_SDL2)
HyAudioInst_SDL2::HyAudioInst_SDL2(HyAudio_SDL2 &audioRef, const jsonxx::Object &instObjRef) :
	m_AudioRef(audioRef),
	m_eCueType(CUETYPE_Unknown)
{
	jsonxx::Object sdl2Obj = instObjRef.get<jsonxx::Object>("SDL2");

	jsonxx::Array assetsArray = sdl2Obj.get<jsonxx::Array>("assets");
	for(uint32 i = 0; i < assetsArray.size(); ++i)
		m_SoundChecksumList.push_back(assetsArray.get<jsonxx::Number>(i));

	std::string sType = sdl2Obj.get<jsonxx::String>("type");
	std::transform(sType.begin(), sType.end(), sType.begin(), ::tolower);
	if(sType == "single")
		m_eCueType = CUETYPE_Single;

	if(m_eCueType == CUETYPE_Unknown)
		HyLogWarning("HyAudioInst_SDL2 has unknown cue type");
}

/*virtual*/ HyAudioInst_SDL2::~HyAudioInst_SDL2()
{
}

/*virtual*/ void HyAudioInst_SDL2::OnLoaded() /*override*/
{
}

/*virtual*/ void HyAudioInst_SDL2::Start() /*override*/
{
	if(m_eCueType == CUETYPE_Single)
		m_AudioRef.QueueInst(this, m_SoundChecksumList[0]);
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
#endif // defined(HY_USE_SDL2)
