/**************************************************************************
*	HyFileIO.h
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#ifndef HyFileIO_h__
#define HyFileIO_h__

#include "Afx/HyStdAfx.h"
#include <fstream>

class HyIO
{
public:
	static void ReadTextFile(const char *szFilePath, std::string &sContentsOut);

	static void WriteTextFile(const char *szFilePath, const char *szContentBuffer);

	static bool FileExists(const std::string &sFilePath);

	static std::string CleanPath(const char *szPath, const char *szExtension, bool bMakeLowercase);

	static bool SaveImage_DTX5(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);

	static bool SaveImage_DTX1(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);
};

#endif /* HyFileIO_h__ */
