/**************************************************************************
*	HyAudioBank_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioBank_SDL2_h__
#define HyAudioBank_SDL2_h__

#include "Audio/Harness/IHyAudioBank.h"

class HyAudioBank_SDL2 : public IHyAudioBank
{
	std::map<uint32, std::string>	m_ChecksumMap;

	struct Buffer
	{
		uint8_t *					m_pBuffer = nullptr;
		uint32						m_uiBufferSize = 0;
		SDL_AudioSpec				m_Spec;
	};
	std::vector<Buffer>				m_SoundBuffers;

public:
	HyAudioBank_SDL2(const jsonxx::Object &bankObjRef);
	virtual ~HyAudioBank_SDL2();

	virtual bool Load(std::string sFilePath) override;

	virtual void Unload() override;
};

#endif /* HyAudioBank_SDL2_h__ */
