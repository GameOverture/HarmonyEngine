/**************************************************************************
 *	IData.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2014 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Creator/Data/IData.h"

#include <algorithm>

/*static*/ vector<HyTexture *>	IData::sm_vTextures;

/*static*/ HyTexture *IData::LoadTexture(const std::string &sTexturePath)
{
	std::string sFilePath(sTexturePath);
	std::transform(sFilePath.begin(), sFilePath.end(), sFilePath.begin(), ::tolower);

	// Returns the texture ptr while and avoiding repeats
	for(uint32 i = 0; i < sm_vTextures.size(); ++i)
	{
		if(sm_vTextures[i]->GetPath() == sFilePath)
			return sm_vTextures[i];
	}

	HyTexture *pNewTexture = new HyTexture(sFilePath);
	sm_vTextures.push_back(pNewTexture);

	return pNewTexture;
}

/*static*/ HyTexture *IData::LoadTexture(const std::string &sTexturePath, int32 iWidth, int32 iHeight, int32 iNum8bitClrChannels, unsigned char *pPixelData)
{
	std::string sFilePath(sTexturePath);
	std::transform(sFilePath.begin(), sFilePath.end(), sFilePath.begin(), ::tolower);

	// Returns the texture ptr while and avoiding repeats
	for(uint32 i = 0; i < sm_vTextures.size(); ++i)
	{
		if(sm_vTextures[i]->GetPath() == sFilePath)
			return sm_vTextures[i];
	}

	HyTexture *pNewTexture = new HyTexture(sFilePath, iWidth, iHeight, iNum8bitClrChannels, pPixelData);
	sm_vTextures.push_back(pNewTexture);

	return pNewTexture;
}
