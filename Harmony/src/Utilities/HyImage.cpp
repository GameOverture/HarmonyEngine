/**************************************************************************
*	HyImage.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Utilities/HyImage.h"

bool HySaveImage_DTX5(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData)
{
	return 0 != SOIL_save_image_quality(szFilename, SOIL_SAVE_TYPE_DDS, iWidth, iHeight, 4, pUncompressedPixelData, 0);
}

bool HySaveImage_DTX1(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData)
{
	return 0 != SOIL_save_image_quality(szFilename, SOIL_SAVE_TYPE_DDS, iWidth, iHeight, 3, pUncompressedPixelData, 0);
}
