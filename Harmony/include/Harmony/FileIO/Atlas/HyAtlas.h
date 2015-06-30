/**************************************************************************
*	HyTexture.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef __HyAtlas_h__
#define __HyAtlas_h__

#include "FileIO/Atlas/HyTexture.h"

class HyAtlas
{
	HyTexture **	m_ppTextures;
	uint32			m_uiNumTextures;

public:
	HyAtlas();
	~HyAtlas();

	void Initialize(std::string sAtlasInfoFilePath);

	// Only invoked on the Load thread
	HyTexture *GetTexture(uint32 uiTextureIndex, std::string sFilePath);
};

#endif /* __HyAtlas_h__ */
