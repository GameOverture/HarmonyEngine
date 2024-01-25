/**************************************************************************
*	HyFileIO.cpp
*
*	Harmony Engine
*	Copyright (c) 2015 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyIO.h"
#include "Diagnostics/Console/IHyConsole.h"

#include "vendor/SOIL2/src/SOIL2/SOIL2.h"

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cwctype>

/*static*/ HyStorage HyIO::SessionStorage(true);
/*static*/ HyStorage HyIO::LocalStorage(false);

/*static*/ void HyIO::MakeLowercase(std::string &sStringOut)
{
	std::transform(sStringOut.begin(), sStringOut.end(), sStringOut.begin(), ::tolower);
}

/*static*/ void HyIO::TrimWhitespace(std::string &sStringOut)
{
	sStringOut.erase(0, sStringOut.find_first_not_of(" \t\r\n"));
	sStringOut.erase(sStringOut.find_last_not_of(" \t\r\n") + 1);
}

/*static*/ size_t HyIO::Utf8Length(const std::string &sStrRef)
{
	const char *szStr = sStrRef.c_str();
	size_t uiCount = 0;
	while(*szStr != '\0')
	{
		if((*szStr & 0xC0) != 0x80)
			++uiCount;
		++szStr;
	}

	return uiCount;
}

/*static*/ void HyIO::Utf8Erase(std::string &sStrRef, size_t uiOffset, size_t uiCount)
{
	//const char *szStr = sStrRef.c_str();
	//size_t uiUtf8Count = 0;
	//size_t uiNumByteOffset = 0;
	//while(*szStr != '\0')
	//{
	//	if((*szStr & 0xC0) != 0x80)
	//	{
	//		if(uiUtf8Count == uiOffset)
	//			break;
	//	
	//		++uiUtf8Count;
	//	}
	//	++uiNumByteOffset;
	//	++szStr;
	//}

	//szStr = &sStrRef.at(uiNumByteOffset);
	//uiUtf8Count = 0;
	//size_t uiNumByteCount = 0;
	//while(*szStr != '\0')
	//{
	//	if((*szStr & 0xC0) != 0x80)
	//	{
	//		if(uiUtf8Count == uiOffset)
	//			break;

	//		++uiUtf8Count;
	//	}
	//	++uiNumByteCount;
	//	++szStr;
	//}

	//sStrRef = sStrRef.erase(uiNumByteOffset, uiNumByteCount);


	// Convert UTF-8 string to wstring
	std::wstring_convert<std::remove_reference<decltype(std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale()))>::type, wchar_t> converter;
	std::wstring wstr = converter.from_bytes(sStrRef);

	// Erase characters in the wstring
	wstr.erase(uiOffset, uiCount);

	// Convert back to UTF-8
	sStrRef = converter.to_bytes(wstr);
}

/*static*/ void HyIO::Utf8Insert(std::string &sStrRef, size_t uiOffset, const std::string &sUtf8Str)
{
	const char *szStr = sStrRef.c_str();
	size_t uiUtf8Count = 0;
	size_t uiNumByteOffset = 0;
	while(*szStr != '\0')
	{
		if((*szStr & 0xC0) != 0x80)
		{
			if(uiUtf8Count == uiOffset)
				break;

			++uiUtf8Count;
		}
		++uiNumByteOffset;
		++szStr;
	}

	sStrRef = sStrRef.insert(uiNumByteOffset, sUtf8Str);
}

/*static*/ bool HyIO::Utf8IsAlnum(const std::string &sStrRef) // Returns true if all characters in sStrRef are alphanumeric
{
	const char *szStr = sStrRef.c_str();
	while(*szStr != '\0')
	{
		if((*szStr & 0xC0) != 0x80)
		{
			if(std::iswalnum(*szStr) == 0)
				return false;
		}
		++szStr;
	}
	return true;
}

/*static*/ bool HyIO::Utf8IsWhitespace(const std::string &sStrRef) // Returns true if all characters in sStrRef are whitespace
{
	const char *szStr = sStrRef.c_str();
	while(*szStr != '\0')
	{
		if((*szStr & 0xC0) != 0x80)
		{
			if(std::iswspace(*szStr) == 0)
				return false;
		}
		++szStr;
	}
	return true;
}

/*static*/ uint32 HyIO::Utf8_to_Utf32(const char *pChar, uint32 &uiNumBytesUsedRef)
{
#ifndef HY_ENDIAN_LITTLE
	#error "HyIO::Utf8_to_Utf32 does not support big endian"
#endif

	uint32 uiResult = -1;
	uiNumBytesUsedRef = 0;

	if(!pChar)
		return uiResult;

	if((pChar[0] & 0x80) == 0x0)
	{
		uiNumBytesUsedRef = 1;
		uiResult = pChar[0];
	}

	if((pChar[0] & 0xC0) == 0xC0)
	{
		uiNumBytesUsedRef = 2;
		uiResult = ((pChar[0] & 0x3F) << 6) | (pChar[1] & 0x3F);
	}

	if((pChar[0] & 0xE0) == 0xE0)
	{
		uiNumBytesUsedRef = 3;
		uiResult = ((pChar[0] & 0x1F) << (6 + 6)) | ((pChar[1] & 0x3F) << 6) | (pChar[2] & 0x3F);
	}

	if((pChar[0] & 0xF0) == 0xF0)
	{
		uiNumBytesUsedRef = 4;
		uiResult = ((pChar[0] & 0x0F) << (6 + 6 + 6)) | ((pChar[1] & 0x3F) << (6 + 6)) | ((pChar[2] & 0x3F) << 6) | (pChar[3] & 0x3F);
	}

	if((pChar[0] & 0xF8) == 0xF8)
	{
		uiNumBytesUsedRef = 5;
		uiResult = ((pChar[0] & 0x07) << (6 + 6 + 6 + 6)) | ((pChar[1] & 0x3F) << (6 + 6 + 6)) | ((pChar[2] & 0x3F) << (6 + 6)) | ((pChar[3] & 0x3F) << 6) | (pChar[4] & 0x3F);
	}

	return uiResult;
}

/*static*/ std::string HyIO::Utf32_to_Utf8(uint32 uiChar)
{
	std::string sResult;
	if(uiChar <= 0x7F)
	{
		sResult.resize(1);
		sResult[0] = uiChar;
	}
	else if(uiChar <= 0x7FF)
	{
		sResult.resize(2);
		sResult[0] = 0xC0 | ((uiChar >> 6) & 0x1F);
		sResult[1] = 0x80 | (uiChar & 0x3F);
	}
	else if(uiChar <= 0xFFFF)
	{
		sResult.resize(3);
		sResult[0] = 0xE0 | ((uiChar >> 12) & 0x0F);
		sResult[1] = 0x80 | ((uiChar >> 6) & 0x3F);
		sResult[2] = 0x80 | (uiChar & 0x3F);
	}
	else if(uiChar <= 0x1FFFFF)
	{
		sResult.resize(4);
		sResult[0] = 0xF0 | ((uiChar >> 18) & 0x07);
		sResult[1] = 0x80 | ((uiChar >> 12) & 0x3F);
		sResult[2] = 0x80 | ((uiChar >> 6) & 0x3F);
		sResult[3] = 0x80 | (uiChar & 0x3F);
	}
	else
		HyLogWarning("HyIO::Utf32_to_Utf8 - Unhandled UTF32 character: " << uiChar);

	return sResult;
}

/*static*/ std::string HyIO::CleanPath(const std::string &sDirtyPath, const std::string &sExtension /*= ""*/)
{
	std::string sPath(sDirtyPath);

	// If sPath starts with "\\", set 'bIsNetworkPath' to true
	bool bIsNetworkPath = false;
	if(sPath.length() >= 2 && sPath[0] == '\\' && sPath[1] == '\\')
		bIsNetworkPath = true;

	// If this is a newtwork path, get rid of the leading '\\'
	if(bIsNetworkPath)
	{
		sPath.erase(0, 2);
		if(sPath.empty())
			return sPath;
	}

	std::replace(sPath.begin(), sPath.end(), '\\', '/');

	// Prepend the "\\" back onto the path if it was a network path
	if(bIsNetworkPath)
		sPath.insert(0, "\\\\");

	TrimWhitespace(sPath);

	if(sExtension.empty() == false)
	{
		// Don't enforce '.' on sExtension (directories use '/' as an extension)

		std::string sTestExistingExtension = sPath.substr(sPath.length() - sExtension.length());
		std::string sTestNewExtension = sExtension;
#if defined(HY_PLATFORM_WINDOWS)
		// Test for case insensitive file extension
		std::transform(sTestExistingExtension.begin(), sTestExistingExtension.end(), sTestExistingExtension.begin(), ::tolower);
		std::transform(sTestNewExtension.begin(), sTestNewExtension.end(), sTestNewExtension.begin(), ::tolower);
#endif
		if(sPath.empty() || sTestNewExtension != sTestExistingExtension)
			sPath += sExtension;
	}

	// Get rid of any double slashes
	size_t uiIndex = 0;
	while(true)
	{
		uiIndex = sPath.find("//", uiIndex);
		if(uiIndex == std::string::npos)
			break;

		sPath.replace(uiIndex, 2, "/");
	}

	// Resolve any "."
	// If path begins with "./", get rid of it
	if(0 == sPath.compare(0, 2, "./"))
		sPath.replace(0, 2, "");

	// Get rid of any other "/./"
	while(true)
	{
		uiIndex = 0;
		uiIndex = sPath.find("/./", uiIndex);
		if(uiIndex == std::string::npos)
			break;

		sPath.replace(uiIndex, 3, "/");
	}

	// Resolve any ".." (as far as possible)
	while(true)
	{
		uiIndex = 0;
		uiIndex = sPath.find("/..", uiIndex);
		if(uiIndex == std::string::npos || uiIndex == 0)
			break;

		size_t uiStartIndex = sPath.rfind("/", uiIndex-1);
		if(uiStartIndex == std::string::npos)
			break;

		sPath.erase(uiStartIndex, (uiIndex+3) - uiStartIndex);
	}

	return sPath;
}

/*static*/ std::string HyIO::GetWorkingDirectory()
{
	std::string sWorkingDir;
#if defined(HY_PLATFORM_WINDOWS)
	char szBuffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, szBuffer);
	sWorkingDir = szBuffer;
#elif defined(HY_PLATFORM_LINUX)
	char szBuffer[PATH_MAX];
	getcwd(szBuffer, PATH_MAX);
	sWorkingDir = szBuffer;
#endif

	return HyIO::CleanPath(sWorkingDir, "/");
}

/*static*/ std::string HyIO::GetFileNameFromPath(const std::string &sPath)
{
	std::string sTmpPath = HyIO::CleanPath(sPath.c_str());
	size_t uiStartIndex = sTmpPath.rfind("/", std::string::npos) + 1;

	return sTmpPath.substr(uiStartIndex);
}

/*static*/ std::string HyIO::GetDirectoryFromPath(const std::string &sPath)
{
	std::string sTmpPath = HyIO::CleanPath(sPath.c_str());
	if(sTmpPath.empty())
		return "/";

	// If sTmpPath's last character is a '/', then remove it
	if(sTmpPath[sTmpPath.length() - 1] == '/')
		sTmpPath = sTmpPath.substr(0, sTmpPath.length() - 1);

	size_t uiEndIndex = sTmpPath.rfind("/", std::string::npos);

	return HyIO::CleanPath(sTmpPath.substr(0, uiEndIndex), "/");
}

/*static*/ std::string HyIO::GetExtensionFromPath(const std::string &sPath)
{
	std::string sTmpPath = HyIO::CleanPath(sPath.c_str());
	size_t uiStartIndex = sTmpPath.rfind(".", std::string::npos);

	return sTmpPath.substr(uiStartIndex);
}

/*static*/ std::string HyIO::GetFileNameWithoutExtension(const std::string &sPath)
{
	std::string sTmpPath = HyIO::CleanPath(sPath.c_str());
	size_t uiStartIndex = sTmpPath.rfind("/", std::string::npos) + 1;
	size_t uiEndIndex = sTmpPath.rfind(".", std::string::npos);

	return sTmpPath.substr(uiStartIndex, uiEndIndex - uiStartIndex);
}

/*static*/ bool HyIO::FileExists(const std::string &sFilePath)
{
	return std::filesystem::exists(sFilePath) && std::filesystem::is_directory(sFilePath) == false;
}

/*static*/ bool HyIO::DirectoryExists(const std::string &sDirPath)
{
	return std::filesystem::exists(sDirPath) && std::filesystem::is_directory(sDirPath);
}

/*static*/ std::vector<std::string> HyIO::GetFileList(const std::string &sDirPath, const std::string &sFilterExtension, bool bRecursively)
{
	std::vector<std::string> fileList;
	if(DirectoryExists(sDirPath) == false)
		return fileList;

	std::filesystem::recursive_directory_iterator iter(sDirPath);
	std::filesystem::recursive_directory_iterator end;
	while(iter != end)
	{
		if(std::filesystem::is_regular_file(iter->path()))
		{
			if(sFilterExtension.empty() || iter->path().extension() == sFilterExtension)
				fileList.push_back(iter->path().string());
		}
		if(bRecursively == false)
			iter.disable_recursion_pending();
		
		std::error_code ec;
		iter.increment(ec);
		if(ec)
			break;
	}

	return fileList;
}

/*static*/ void HyIO::ReadTextFile(const char *szFilePath, std::vector<char> &contentsOut)
{
	//SDL_RWops *pBankFile = SDL_RWFromFile(sFilePath.c_str(), "rb");
	//if(pBankFile == nullptr)
	//	return false;

	//Sint64 iBankSize = SDL_RWsize(pBankFile);
	//m_pBankData = HY_NEW unsigned char[iBankSize + 1];

	//Sint64 iTotalReadObjs = 0, iCurReadObjs = 1;
	//unsigned char *pFilePtr = m_pBankData;
	//while(iTotalReadObjs < iBankSize && iCurReadObjs != 0)
	//{
	//	iCurReadObjs = SDL_RWread(pBankFile, pFilePtr, 1, (iBankSize - iTotalReadObjs));
	//	iTotalReadObjs += iCurReadObjs;
	//	pFilePtr += iCurReadObjs;
	//}
	//SDL_RWclose(pBankFile);

	//if(iTotalReadObjs != iBankSize)
	//{
	//	HyLogError("Failed to read " << sFilePath);
	//	delete[] m_pBankData;
	//	return false;
	//}

	//m_pBankData[iTotalReadObjs] = '\0';
	//return true;

	if(szFilePath == nullptr)
		return;

	std::ifstream infile(szFilePath, std::ifstream::in | std::ios::binary | std::ios::ate);
	HyAssert(infile, "HyReadTextFile invalid file: " << szFilePath);

	std::streamsize size = infile.tellg();
	infile.seekg(0, std::ios::beg);

	contentsOut.resize(static_cast<uint32>(size) + 1);
	if(infile.read(contentsOut.data(), size))
		contentsOut[static_cast<uint32>(size)] = '\0';
	else {
		HyLogError("HyIO::ReadTextFile - only " << infile.gcount() << " bytes was read");
	}

	infile.close();
}

/*static*/ void HyIO::ReadBinaryFile(const char *szFilePath, std::vector<uint8> &contentsOut)
{
	if(szFilePath == nullptr)
		return;

	std::ifstream infile(szFilePath, std::ifstream::in | std::ios::binary);
	contentsOut = std::vector<uint8>(std::istreambuf_iterator<char>(infile), {});

	infile.close();
}

///*static*/ uint8 *HyIO::ReadBinaryFile(const char *szFilePath, uint32 &uiBufferSizeOut)
//{
//	if(szFilePath == nullptr)
//		return nullptr;
//
//	std::ifstream infile(szFilePath, std::ifstream::in | std::ios::binary | std::ios::ate);
//	HyAssert(infile, "ReadBinaryFile invalid file: " << szFilePath);
//
//	std::streamsize size = infile.tellg();
//	infile.seekg(0, std::ios::beg);
//
//	uiBufferSizeOut = static_cast<uint32>(size);
//	uint8 *pBuffer = HY_NEW uint8[uiBufferSizeOut];
//
//	if(!infile.read(reinterpret_cast<char *>(pBuffer), size))
//		HyLogError("HyIO::ReadBinaryFile - only " << infile.gcount() << " bytes was read");
//
//	infile.close();
//
//	return pBuffer;
//}

/*static*/ void HyIO::WriteTextFile(const char *szFilePath, const char *szContentBuffer)
{
	std::ofstream outfile(szFilePath);
	outfile.write(szContentBuffer, strlen(szContentBuffer));
	outfile.close();
}

/*static*/ std::string HyIO::UrlEncode(std::string sStr)
{
	std::ostringstream ssEncoded;
	ssEncoded.fill('0');
	ssEncoded << std::hex;

	for(std::string::const_iterator iter = sStr.begin(); iter != sStr.end(); ++iter)
	{
		std::string::value_type c = (*iter);

		// Keep alphanumeric and other accepted characters intact
		if(isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{
			ssEncoded << c;
			continue;
		}

		// Any other characters are percent-encoded
		ssEncoded << std::uppercase;
		ssEncoded << '%' << std::setw(2) << int((unsigned char)c);
		ssEncoded << std::nouppercase;
	}

	return ssEncoded.str();
}

/*static*/ std::string HyIO::UrlDecode(std::string sStr)
{
	std::ostringstream ssDecoded;

	for(std::string::const_iterator iter = sStr.begin(); iter != sStr.end(); ++iter)
	{
		std::string::value_type c0 = (*iter);
		
		if(c0 == '%')
		{
			++iter;
			if(iter == sStr.end())
			{
				HyLogWarning("HyIO::UrlDecode parsed invalid '%' encoding");

				ssDecoded << c0;
				return ssDecoded.str();
			}
			std::string::value_type c1 = (*iter);

			++iter;
			if(iter == sStr.end())
			{
				HyLogWarning("HyIO::UrlDecode parsed invalid '%' encoding");

				ssDecoded << c0;
				--iter; // process whatever 'c1' is at least
				continue;
			}
			std::string::value_type c2 = (*iter);

			std::string sHex;
			sHex += c1;
			sHex += c2;
			int32 iDec = 0;
			std::istringstream(sHex) >> std::hex >> iDec;

			ssDecoded << static_cast<char>(iDec);
		}
		else if(c0 == '+')
			ssDecoded << ' ';
		else
			ssDecoded << c0;
	}

	return ssDecoded.str();
}

/*static*/ bool HyIO::SaveImage_DTX5(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData)
{
	return 0 != SOIL_save_image_quality(szFilename, SOIL_SAVE_TYPE_DDS, iWidth, iHeight, 4, pUncompressedPixelData, 0);
}

/*static*/ bool HyIO::SaveImage_DTX1(const char *szFilename, int iWidth, int iHeight, const unsigned char *const pUncompressedPixelData)
{
	return 0 != SOIL_save_image_quality(szFilename, SOIL_SAVE_TYPE_DDS, iWidth, iHeight, 3, pUncompressedPixelData, 0);
}
