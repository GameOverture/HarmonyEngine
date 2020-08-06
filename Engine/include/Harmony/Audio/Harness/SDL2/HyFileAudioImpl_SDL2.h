/**************************************************************************
*	HyFileAudioImpl_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioBank_SDL2_h__
#define HyAudioBank_SDL2_h__

#include "Afx/HyStdAfx.h"
#include "Audio/Harness/IHyFileAudioImpl.h"
#include "Diagnostics/Console/HyConsole.h"

#if defined(HY_USE_SDL2)
class HyRawSoundBuffer
{
	const std::string			m_sFILE_NAME;

	uint8_t *					m_pBuffer;
	uint32						m_uiBufferSize;
	SDL_AudioSpec				m_Spec;

public:
	HyRawSoundBuffer(std::string sFileName) :
		m_sFILE_NAME(sFileName),
		m_pBuffer(nullptr),
		m_uiBufferSize(0)
	{
		m_Spec = {};
	}

	const uint8_t *GetBuffer(uint32 uiRemainingBytes) const {
		return m_pBuffer + (m_uiBufferSize - uiRemainingBytes);
	}

	uint32 GetBufferSize() const {
		return m_uiBufferSize;
	}

	SDL_AudioFormat GetFormat() const {
		return m_Spec.format;
	}

	bool Load(std::string sFilePath) {
		std::string s = sFilePath;
		s += "/";
		s += m_sFILE_NAME;

		if(SDL_LoadWAV(s.c_str(), &m_Spec, &m_pBuffer, &m_uiBufferSize) == nullptr)
		{
			HyLogError("HyRawSoundBuffer::Load SDL_LoadWAV failed: " << SDL_GetError());
			return false;
		}

		return true;
	}

	void Unload() {
		if(m_pBuffer)
			SDL_FreeWAV(m_pBuffer);
		m_pBuffer = nullptr;
		m_uiBufferSize = 0;
		m_Spec = {};
	}
};

class HyFileAudioImpl_SDL2 : public IHyFileAudioImpl
{
	std::vector<HyRawSoundBuffer *>			m_SoundBuffers;
	std::map<uint32, HyRawSoundBuffer *>	m_ChecksumMap;

public:
	HyFileAudioImpl_SDL2(HyJsonObj &bankObjRef);
	virtual ~HyFileAudioImpl_SDL2();

	virtual bool ContainsAsset(uint32 uiAssetChecksum) override;

	virtual bool Load(std::string sFilePath) override;
	virtual void Unload() override;

	HyRawSoundBuffer *GetBufferInfo(uint32 uiChecksum);
};
#endif // HY_USE_SDL2

#endif /* HyAudioBank_SDL2_h__ */
