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
#include "HyStorage.h"

class HyIO
{
public:
	static HyStorage	SessionStorage;
	static HyStorage	LocalStorage;

	static void MakeLowercase(std::string &sStringOut);
	static void TrimWhitespace(std::string &sStringOut);	// Trim whitespace on the ends of sStringOut

	static size_t Utf8Length(const std::string &sStrRef); // Return the number of UTF-8 characters from a well formed UTF-8 string sequence
	static void Utf8Erase(std::string &sStrRef, size_t uiOffset, size_t uiCount); // Erase the number of UTF-8 characters 'uiCount' starting from UTF-8 character 'uiOffset'
	static void Utf8Insert(std::string &sStrRef, size_t uiOffset, const std::string &sUtf8Str);
	static bool Utf8IsAlnum(const std::string &sStrRef); // Returns true if all characters in sStrRef are alphanumeric
	static bool Utf8IsWhitespace(const std::string &sStrRef); // Returns true if all characters in sStrRef are whitespace

	static uint32 Utf8_to_Utf32(const char *pChar, uint32 &uiNumBytesUsedRef); // Converts a given UTF-8 encoded character (array) to its UTF-32 LE equivalent
	static std::string Utf32_to_Utf8(uint32 uiChar); // Converts a given UTF-32 LE character to its UTF-8 encoded equivalent

	static std::string UrlEncode(std::string sString);
	static std::string UrlDecode(std::string sString);
	static std::string HtmlDecode(std::string sString);

	static std::string CleanPath(const std::string &sDirtyPath, const std::string &sExtension = "");
	static std::string GetWorkingDirectory();
	static std::string GetFileNameFromPath(const std::string &sPath);
	static std::string GetDirectoryFromPath(const std::string &sPath); // Returns the directory of sPath. If sPath is a directory, then it returns the parent directory of sPath. Return value will have a trailing slash.
	static std::string GetExtensionFromPath(const std::string &sPath);
	static std::string GetFileNameWithoutExtension(const std::string &sPath);
	static bool FileExists(const std::string &sFilePath, bool bRegularFile = true);
	static bool DirectoryExists(const std::string &sDirPath);
	static std::vector<std::string> GetFileList(const std::string &sDirPath, const std::string &sFilterExtension, bool bRecursively); // sFilterExtension can be empty or in the form of example: ".png" or ".txt"

	static bool ReadTextFile(const std::string &sFilePath, std::vector<char> &sContentsOut);
	static bool WriteTextFile(const std::string &sFilePath, const char *szContentBuffer);
	static bool ReadBinaryFile(const std::string &sFilePath, std::vector<uint8> &contentsOut);
	static bool WriteBinaryFile(const std::string &sFilePath, const std::vector<uint8> &data);
	
	static uint8 *ReadImage(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut);
	static void DeleteImage(uint8 *pImageData);
	static bool WriteImage(const std::string &sFilePath, HyImageInfo imageInfo, uint8 *pData);

private:
	static uint8 *ReadImage_PNG(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut);
	static uint8 *ReadImage_HYTX(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut);
	static uint8 *ReadImage_DDS(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut);
	static uint8 *ReadImage_ASTC(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut);

	static bool WriteImage_HYTX(const std::string &sFilePath, HyImageInfo imageInfo, uint8 *pData);
	static bool WriteImage_DDS(const std::string &sFilePath, HyImageInfo imageInfo, uint8 *pData);
};

#endif /* HyFileIO_h__ */
