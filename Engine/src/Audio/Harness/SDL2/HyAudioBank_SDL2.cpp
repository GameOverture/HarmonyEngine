/**************************************************************************
*	HyAudioBank_SDL2.h
*
*	Harmony Engine
*	Copyright (c) 2020 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Audio/Harness/SDL2/HyAudioBank_SDL2.h"
#include "Diagnostics/Console/HyConsole.h"

#if defined(HY_USE_SDL2)

HyAudioBank_SDL2::HyAudioBank_SDL2()
{
}

/*virtual*/ HyAudioBank_SDL2::~HyAudioBank_SDL2()
{
}

/*virtual*/ bool HyAudioBank_SDL2::Load(std::string sFilePath) /*override*/
{
	SDL_RWops *pBankFile = SDL_RWFromFile(sFilePath.c_str(), "rb");
	if(pBankFile == nullptr)
		return false;

	Sint64 iBankSize = SDL_RWsize(pBankFile);
	m_pBankData = HY_NEW unsigned char[iBankSize + 1];

	Sint64 iTotalReadObjs = 0, iCurReadObjs = 1;
	unsigned char *pFilePtr = m_pBankData;
	while(iTotalReadObjs < iBankSize && iCurReadObjs != 0)
	{
		iCurReadObjs = SDL_RWread(pBankFile, pFilePtr, 1, (iBankSize - iTotalReadObjs));
		iTotalReadObjs += iCurReadObjs;
		pFilePtr += iCurReadObjs;
	}
	SDL_RWclose(pBankFile);

	if(iTotalReadObjs != iBankSize)
	{
		HyLogError("Failed to read " << sFilePath);
		delete[] m_pBankData;
		return false;
	}

	m_pBankData[iTotalReadObjs] = '\0';
	return true;
}

#endif // defined(HY_USE_SDL2)
