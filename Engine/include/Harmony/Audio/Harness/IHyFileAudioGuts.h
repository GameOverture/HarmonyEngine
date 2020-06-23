/**************************************************************************
*	IHyFileAudioGuts.h
*	
*	Harmony Engine
*	Copyright (c) 2019 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef IHyFileAudioGuts_h__
#define IHyFileAudioGuts_h__

#include <string>

class IHyFileAudioGuts
{
public:
	IHyFileAudioGuts() = default;
	virtual ~IHyFileAudioGuts(void) = default;

	virtual bool ContainsAsset(uint32 uiAssetChecksum) = 0;

	virtual bool Load(std::string sFilePath) = 0;
	virtual void Unload() = 0;
};

class HyAudioBank_Null : public IHyFileAudioGuts
{
public:
	HyAudioBank_Null() = default;
	virtual ~HyAudioBank_Null() = default;

	virtual bool ContainsAsset(uint32 uiAssetChecksum) override
	{ return false; }

	virtual bool Load(std::string sFilePath) override
	{ return true; }

	virtual void Unload() override
	{ }
};

#endif /* IHyFileAudioGuts_h__ */
