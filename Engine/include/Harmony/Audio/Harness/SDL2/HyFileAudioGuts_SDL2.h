/**************************************************************************
*	HyFileAudioGuts_SDL2.h
*	
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyAudioBank_SDL2_h__
#define HyAudioBank_SDL2_h__

#include "Audio/Harness/IHyFileAudioGuts.h"

#if defined(HY_USE_SDL2)
class HyFileAudioGuts_SDL2 : public IHyFileAudioGuts
{
	struct Buffer
	{
		std::string					m_sFileName;
		uint8_t *					m_pBuffer;
		uint32						m_uiBufferSize;
		SDL_AudioSpec				m_Spec;

		Buffer(std::string sFileName) :
			m_sFileName(sFileName),
			m_pBuffer(nullptr),
			m_uiBufferSize(0)
		{ m_Spec = {}; }
	};
	std::vector<Buffer *>			m_SoundBuffers;
	std::map<uint32, Buffer *>		m_ChecksumMap;

public:
	HyFileAudioGuts_SDL2(const jsonxx::Object &bankObjRef);
	virtual ~HyFileAudioGuts_SDL2();

	virtual bool ContainsAsset(uint32 uiAssetChecksum) override;

	virtual bool Load(std::string sFilePath) override;
	virtual void Unload() override;

	bool GetBufferInfo(uint32 uiChecksum, uint8_t *&pBufferOut, uint32 &uiSizeOut, SDL_AudioSpec &audioSpecOut);
};
#endif // HY_USE_SDL2

#endif /* HyAudioBank_SDL2_h__ */
