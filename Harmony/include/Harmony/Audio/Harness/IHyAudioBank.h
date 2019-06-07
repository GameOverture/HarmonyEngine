/**************************************************************************
*	IHyAudioBank.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyAudioBank_h__
#define IHyAudioBank_h__

#include <string>

class IHyAudioBank
{
public:
	IHyAudioBank() = default;
	virtual ~IHyAudioBank(void) = default;

	virtual bool Load(std::string sFilePath) = 0;
};

class HyAudioBank_Null : public IHyAudioBank
{
public:
	HyAudioBank_Null() = default;
	virtual ~HyAudioBank_Null() = default;

	virtual bool Load(std::string sFilePath) override
	{ return true; }
};

#endif /* IHyAudioBank_h__ */
