/**************************************************************************
 *	HyAudioCore_SDL2.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2020 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyAudioCore_SDL2_h__
#define HyAudioCore_SDL2_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyAudioCore.h"

class IHyNode;
class IHyFileAudioImpl;

#if defined(HY_USE_SDL2)
class HyRawSoundBuffer;
class HyFileAudioImpl_SDL2;

class HyAudioCore_SDL2 : public IHyAudioCore
{
	std::vector<std::string>			m_sDeviceList;

	SDL_AudioDeviceID					m_hDevice;
	SDL_AudioSpec						m_DesiredSpec;

	std::vector<HyFileAudioImpl_SDL2 *>	m_AudioFileList;

	// Used in callback thread ///////////////////////////////////////////////
	struct Play
	{
		const IHyNode *					m_pID;

		float							m_fVolume;
		float							m_fPitch;
		bool							m_bPaused;

		HyRawSoundBuffer *				m_pBuffer;
		uint32							m_uiRemainingBytes;

		Play(const IHyNode *pID, float fVolume, float fPitch, bool bPaused, HyRawSoundBuffer *pBuffer, uint32 uiRemainingBytes) :
			m_pID(pID),
			m_fVolume(fVolume),
			m_fPitch(fPitch),
			m_bPaused(bPaused),
			m_pBuffer(pBuffer),
			m_uiRemainingBytes(uiRemainingBytes)
		{ }
	};
	std::vector<Play>					m_PlayList;
	///////////////////////////////////////////////////////////////////////////

public:
	HyAudioCore_SDL2();
	virtual ~HyAudioCore_SDL2(void);

	const char *GetAudioDriver();

	virtual void OnUpdate() override;

	static IHyFileAudioImpl *AllocateBank(IHyAudioCore *pAudio, HyJsonObj &bankObjRef);

private:
	static void OnCallback(void *pUserData, uint8_t *pStream, int32 iLen);
};
#endif // defined(HY_USE_SDL2)

#endif /* HyAudioCore_SDL2_h__ */
