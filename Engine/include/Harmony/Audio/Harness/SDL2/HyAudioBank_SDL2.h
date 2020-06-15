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
	unsigned char *			m_pBankData;

	std::map<uint32, HyRectangle<int32> *>	m_ChecksumMap;

public:
	HyAudioBank_SDL2();
	virtual ~HyAudioBank_SDL2();

	virtual bool Load(std::string sFilePath) override;
};

#endif /* HyAudioBank_SDL2_h__ */
