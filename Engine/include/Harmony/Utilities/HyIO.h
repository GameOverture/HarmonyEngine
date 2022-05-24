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

class HyIO
{
public:
	// Return the number of UTF-8 characters from a well formed UTF-8 string sequence
	static size_t Utf8Length(const std::string &sStrRef);

	// Erase the number of UTF-8 characters 'uiCount' starting from UTF-8 character 'uiOffset'
	static void Utf8Erase(std::string &sStrRef, size_t uiOffset, size_t uiCount);

	static void Utf8Insert(std::string &sStrRef, size_t uiOffset, const std::string &sUtf8Str);

	static std::string CleanPath(const char *szPath, const char *szExtension, bool bMakeLowercase);
	static bool FileExists(const std::string &sFilePath);

	static void ReadTextFile(const char *szFilePath, std::vector<char> &sContentsOut);
	static void WriteTextFile(const char *szFilePath, const char *szContentBuffer);

	static void ReadBinaryFile(const char *szFilePath, std::vector<uint8> &contentsOut);
	//static uint8 *ReadBinaryFile(const char *szFilePath, uint32 &uiBufferSizeOut); // Returns a dynamically allocated buffer of file contents

	static std::string UrlEncode(std::string sString);
	static std::string UrlDecode(std::string sString);

	static bool SaveImage_DTX5(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);
	static bool SaveImage_DTX1(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData);
};

#endif /* HyFileIO_h__ */
