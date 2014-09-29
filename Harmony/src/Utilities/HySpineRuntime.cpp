/**************************************************************************
 *	HySpineRuntime.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Utilities/HySpineRuntime.h"

#include "Creator/Data/IData.h"

void _spAtlasPage_createTexture(spAtlasPage* self, const char* path)
{
	// THIS IS INVOKED FROM THE LOAD THREAD from any IData::DoLoad()

	// Texture manager avoids loading repeats
	HyTexture *pNewTexture = IData::LoadTexture(path);
	self->rendererObject = pNewTexture;

	self->width = pNewTexture->GetWidth();
	self->height = pNewTexture->GetHeight();
}

void _spAtlasPage_disposeTexture (spAtlasPage* self)
{
}

char* _spUtil_readFile (const char* path, int* length)
{
	// The returned data is freed within the spine API
	return HyFileIO::ReadTextFile(path, length);
}

