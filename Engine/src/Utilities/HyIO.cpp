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
#include "Assets/HyAssets.h"
#include "Diagnostics/Console/IHyConsole.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_DXT_IMPLEMENTATION
#include "vendor/stb/stb_image.h"
#include "vendor/stb/stb_image_write.h"
#include "vendor/stb/stb_dxt.h"
//#include "vendor/SOIL2/src/SOIL2/SOIL2.h"

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cwctype>
#include <regex>

#pragma pack(push, 1)
struct HY_DDS_PIXELFORMAT
{
	uint32_t size;
	uint32_t flags;
	uint32_t fourCC;
	uint32_t rgbBitCount;
	uint32_t rBitMask;
	uint32_t gBitMask;
	uint32_t bBitMask;
	uint32_t aBitMask;
};
struct HY_DDS_HEADER
{
	uint32_t size;
	uint32_t flags;
	uint32_t height;
	uint32_t width;
	uint32_t pitchOrLinearSize;
	uint32_t depth;
	uint32_t mipMapCount;
	uint32_t reserved1[11];
	HY_DDS_PIXELFORMAT pf;
	uint32_t caps;
	uint32_t caps2;
	uint32_t caps3;
	uint32_t caps4;
	uint32_t reserved2;
};
#pragma pack(pop)

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

/*static*/ std::string HyIO::HtmlDecode(std::string sString)
{
	// Remove all HTML tags
	sString = std::regex_replace(sString, std::regex("<[^>]*>"), "");

	// Replace all HTML entities
	sString = std::regex_replace(sString, std::regex("&quot;"), "\"");
	sString = std::regex_replace(sString, std::regex("&amp;"), "&");
	sString = std::regex_replace(sString, std::regex("&lt;"), "<");
	sString = std::regex_replace(sString, std::regex("&gt;"), ">");
	sString = std::regex_replace(sString, std::regex("&nbsp;"), " ");
	sString = std::regex_replace(sString, std::regex("&copy;"), "(c)");
	sString = std::regex_replace(sString, std::regex("&reg;"), "(r)");
	sString = std::regex_replace(sString, std::regex("&trade;"), "(tm)");

	return sString;
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

/*static*/ bool HyIO::FileExists(const std::string &sFilePath, bool bRegularFile /*= true*/)
{
	std::error_code ec;
	bool bExists = std::filesystem::exists(sFilePath, ec);
	if(ec)
	{
		HyLogError("HyIO::FileExists - " << ec.message());
		return false;
	}

	bool bIsFile = false;
	if(bRegularFile)
		bIsFile = std::filesystem::is_regular_file(sFilePath, ec);
	else
		bIsFile = (std::filesystem::is_directory(sFilePath, ec) == false);
	if(ec)
	{
		HyLogError("HyIO::FileExists - " << ec.message());
		return false;
	}

	return bExists && bIsFile;
}

/*static*/ bool HyIO::DirectoryExists(const std::string &sDirPath)
{
	std::error_code ec;
	bool bExists = std::filesystem::exists(sDirPath, ec);
	if(ec)
	{
		HyLogError("HyIO::DirectoryExists - " << ec.message());
		return false;
	}
	bool bIsDir = std::filesystem::is_directory(sDirPath, ec);
	if(ec)
	{
		HyLogError("HyIO::DirectoryExists - " << ec.message());
		return false;
	}

	return bExists && bIsDir;
}

/*static*/ std::vector<std::string> HyIO::GetFileList(const std::string &sDirPath, const std::string &sFilterExtension, bool bRecursively)
{
	std::vector<std::string> fileList;
	if(DirectoryExists(sDirPath) == false)
		return fileList;

	std::error_code ec;
	std::filesystem::recursive_directory_iterator iter(sDirPath, ec);
	if(ec)
	{
		HyLogError("HyIO::GetFileList - " << ec.message());
		return fileList;
	}

	std::filesystem::recursive_directory_iterator end;
	while(iter != end)
	{
		if(std::filesystem::is_regular_file(iter->path(), ec))
		{
			if(sFilterExtension.empty() || iter->path().extension() == sFilterExtension)
				fileList.push_back(iter->path().string());
		}
		if(ec)
		{
			HyLogError("HyIO::GetFileList - " << ec.message());
			break;
		}

		if(bRecursively == false)
			iter.disable_recursion_pending();
		
		iter.increment(ec);
		if(ec)
		{
			HyLogError("HyIO::GetFileList - " << ec.message());
			break;
		}
	}

	return fileList;
}

/*static*/ bool HyIO::ReadTextFile(const std::string &sFilePath, std::vector<char> &contentsOut)
{
	if(sFilePath.empty())
		return false;

	std::error_code ec;
	std::ifstream infile(sFilePath, std::ifstream::in | std::ios::binary | std::ios::ate);
	if(!infile || infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadTextFile - opening file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	std::streamsize size = infile.tellg();
	infile.seekg(0, std::ios::beg);

	contentsOut.resize(static_cast<uint32>(size) + 1);
	if(infile.read(contentsOut.data(), size))
		contentsOut[static_cast<uint32>(size)] = '\0';
	else
	{
		HyLogError("HyIO::ReadTextFile - reading file - only " << infile.gcount() << " bytes were read");
		return false;
	}

	infile.close();
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadTextFile - closing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	return true;
}

/*static*/ bool HyIO::WriteTextFile(const std::string &sFilePath, const char *szContentBuffer)
{
	if(sFilePath.empty())
		return false;

	std::error_code ec;
	std::ofstream outfile(sFilePath);
	if(!outfile || outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteTextFile - opening file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	outfile.write(szContentBuffer, strlen(szContentBuffer));
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteTextFile - writing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	outfile.close();
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteTextFile - closing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	HyLog("HyIO::WriteTextFile - Wrote " << strlen(szContentBuffer) << " bytes to " << sFilePath);
	return true;
}

/*static*/ bool HyIO::ReadBinaryFile(const std::string &sFilePath, std::vector<uint8> &contentsOut)
{
	if(sFilePath.empty())
		return false;

	std::error_code ec;
	std::uintmax_t uiSize = std::filesystem::file_size(sFilePath, ec);
	if(ec)
	{
		HyLogError("HyIO::ReadBinaryFile - " << ec.message());
		return false;
	}
	if(uiSize == 0)
		return false;

	contentsOut.resize(uiSize);

	std::ifstream infile(sFilePath, std::ifstream::in | std::ios::binary);
	if(!infile || infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadBinaryFile - opening file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	infile.read(reinterpret_cast<char *>(contentsOut.data()), uiSize);
	if(!infile || infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadBinaryFile - reading file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	infile.close();
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadBinaryFile - closing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	return true;
}

/*static*/ bool HyIO::WriteBinaryFile(const std::string &sFilePath, const std::vector<uint8> &data)
{
	if(sFilePath.empty())
		return false;

	std::error_code ec;
	std::ofstream outfile(sFilePath, std::ios::binary);
	if(!outfile || outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteBinaryFile - opening file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	outfile.write(reinterpret_cast<const char *>(data.data()), static_cast<std::streamsize>(data.size()));
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteBinaryFile - writing file - " << ec.message() << ": " << sFilePath);
		return false;
	}
	if(!outfile.flush())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteBinaryFile - flushing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	outfile.close();
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteBinaryFile - closing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	return true;
}

/*static*/ uint8 *HyIO::ReadImage(const std::string &sFilePath, HyImageInfo &loadHintsInOut, int &iWidthOut, int &iHeightOut, int &iNumChannelsOut, int &iDataSizeOut)
{
	uint8 *pData = nullptr;
	switch(loadHintsInOut.GetType())
	{
	case HYIMAGE_Unknown: {
		std::string sExt = GetExtensionFromPath(sFilePath);
		std::transform(sExt.begin(), sExt.end(), sExt.begin(), ::tolower);
		for(int i = 0; i < HYNUM_IMAGETYPES; ++i)
		{
			if(sExt == HyImageInfo::GetExt(static_cast<HyImageType>(i)))
			{
				loadHintsInOut.SetType(static_cast<HyImageType>(i));
				return ReadImage(sFilePath, loadHintsInOut, iWidthOut, iHeightOut, iNumChannelsOut, iDataSizeOut);
			}
		}
		break; }

	case HYIMAGE_PNG:
		pData = ReadImage_PNG(sFilePath, loadHintsInOut, iWidthOut, iHeightOut, iNumChannelsOut, iDataSizeOut);
		break;

	case HYIMAGE_HYTX:
		pData = ReadImage_HYTX(sFilePath, iWidthOut, iHeightOut, iNumChannelsOut, iDataSizeOut);
		break;

	case HYIMAGE_DDS:
		pData = ReadImage_DDS(sFilePath, iWidthOut, iHeightOut, iNumChannelsOut, iDataSizeOut);
		break;

	case HYIMAGE_ASTC:
		break;

	default:
		HyError("HyIO::ReadImage - Invalid image type specified");
		break;
	}

	loadHintsInOut.SetNumChannels(iNumChannelsOut);
	return pData;
}

/*static*/ void HyIO::DeleteImage(uint8 *pImageData)
{
	stbi_image_free(pImageData);
}

/*static*/ bool HyIO::WriteImage(const std::string &sFilePath, HyImageType eImageType, int iWidth, int iHeight, int iNumChannels, bool bFlipVertically, uint8 *pData)
{
	bool bResult = false;
	switch(eImageType)
	{
	case HYIMAGE_Unknown:
		break;
	case HYIMAGE_PNG:
		stbi_flip_vertically_on_write(bFlipVertically ? 1 : 0);
		bResult = stbi_write_png(sFilePath.c_str(), iWidth, iHeight, iNumChannels, pData, 0) != 0;
		break;
	case HYIMAGE_HYTX:
		break;
	case HYIMAGE_DDS:
		break;
	case HYIMAGE_ASTC:
		break;
	}

	return bResult;
}

/*static*/ uint8 *HyIO::ReadImage_PNG(const std::string &sFilePath, HyImageInfo &loadHintsInOut, int &iWidthOut, int &iHeightOut, int &iNumChannelsOut, int &iDataSizeOut)
{
	uint8 *pData = nullptr;
	if(loadHintsInOut.GetFormat() == HYTEXFORMAT_UINT8 || loadHintsInOut.GetFormat() == HYTEXFORMAT_Unknown)
	{
		pData = stbi_load(sFilePath.c_str(), &iWidthOut, &iHeightOut, &iNumChannelsOut, loadHintsInOut.GetNumChannels());
		iDataSizeOut = iWidthOut * iHeightOut * iNumChannelsOut;
	}
	else if(loadHintsInOut.GetFormat() == HYTEXFORMAT_UINT16)
	{
		pData = reinterpret_cast<uint8 *>(stbi_load_16(sFilePath.c_str(), &iWidthOut, &iHeightOut, &iNumChannelsOut, loadHintsInOut.GetNumChannels()));
		iDataSizeOut = iWidthOut * iHeightOut * (iNumChannelsOut * 2);
	}
	else
	{
		HyLogError("HyIO::ReadImage - Unhandled PNG format type");
		return nullptr;
	}

	if(pData == nullptr)
		HyLogError("HyIO::ReadImage - " << stbi_failure_reason());

	return pData;
}

/*static*/ uint8 *HyIO::ReadImage_HYTX(const std::string &sFilePath, int &iWidthOut, int &iHeightOut, int &iNumChannelsOut, int &iDataSizeOut)
{
	std::error_code ec;
	std::ifstream infile(sFilePath, std::ifstream::in | std::ios::binary);
	if(!infile || infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage - opening HYTX file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	uint32 uiMagic;
	infile.read(reinterpret_cast<char *>(&uiMagic), sizeof(uint32));
	if(uiMagic != 0x58545948) // "HYTX"
	{
		HyLogError("HyIO::ReadImage - Invalid HYTX magic number");
		return nullptr;
	}

	int iTexelSize = 0; // In bytes
	int iNumChannels = textureInfo.m_uiFormatParam1;
	HyTextureFormatType eDataFormat, eInternalFormat;
	textureInfo.GetUncompressedFormatTypes(eDataFormat, eInternalFormat);
	switch(eDataFormat)
	{
		case HYTEXTUREFORMAT_UINT8:
		case HYTEXTUREFORMAT_INT8:
			iTexelSize = iNumChannels;
			break;
		case HYTEXTUREFORMAT_UINT16:
		case HYTEXTUREFORMAT_INT16:
		case HYTEXTUREFORMAT_FLOAT16:
			iTexelSize = iNumChannels * 2;
			break;
		case HYTEXTUREFORMAT_UINT32:
		case HYTEXTUREFORMAT_INT32:
		case HYTEXTUREFORMAT_FLOAT32:
			iTexelSize = iNumChannels * 4;
			break;
		default:
			HyError("HyIO::ParseRawTextureFile() - Unhandled raw data format");
			return false;
	}

	int32_t iNumTexels = *reinterpret_cast<int32_t *>(&fileData[HYASSETS_MagicNumberHeaderSize]);
	size_t uiDataSize = iNumTexels * iTexelSize;
	if(uiDataSize == 0)
	{
		HyLogError("HyIO::ParseRawTextureFile() - Failed to load texel data: " << sFilePath);
		return false;
	}
	
	contentsOut.resize(uiDataSize);
	memcpy(contentsOut.data(), &fileData[HYASSETS_MagicNumberHeaderSize + sizeof(int32_t) + sizeof(int32_t)], uiDataSize);
}

/*static*/ uint8 *HyIO::ReadImage_DDS(const std::string &sFilePath, int &iWidthOut, int &iHeightOut, int &iNumChannelsOut, int &iDataSizeOut)
{
	std::error_code ec;
	std::ifstream infile(sFilePath, std::ifstream::in | std::ios::binary);
	if(!infile || infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage - opening DDS file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	uint32 uiMagic;
	infile.read(reinterpret_cast<char *>(&uiMagic), sizeof(uint32));
	if(uiMagic != 0x20534444) // "DDS "
	{
		HyLogError("HyIO::ReadImage - Invalid DDS magic number");
		return nullptr;
	}

	HY_DDS_HEADER header;
	infile.read(reinterpret_cast<char *>(&header), sizeof(HY_DDS_HEADER));
	iWidthOut = header.width;
	iHeightOut = header.height;
	int iNumMipMaps = header.mipMapCount;
	if(iNumMipMaps == 0)
		iNumMipMaps = 1;
	int iBlockSize;
	switch(header.pf.fourCC)
	{
	case 0x31545844:		// "DXT1"
		iNumChannelsOut = 3;// or 4 (RGBA w/ 1-bit alpha)
		iBlockSize = 8;		// 8 bytes per 4x4 block
		break;
	case 0x55344342:		// "BC4U"
	case 0x53344342:		// "BC4S"
		iNumChannelsOut = 1;
		iBlockSize = 8;		// 8 bytes per 4x4 block
		break;
	case 0x33545844:		// "DXT3" (mostly obsolete)
	case 0x35545844:		// "DXT5"
		iNumChannelsOut = 4;
		iBlockSize = 16;	// 16 bytes per 4x4 block
		break;
	case 0x55354342:		// "BC5U"
	case 0x53354342:		// "BC5S"
		iNumChannelsOut = 2;
		iBlockSize = 16;	// 16 bytes per 4x4 block
		break;
	default:
		HyLogError("HyIO::ReadImage - DDS file - Unhandled fourCC: " << std::hex << header.pf.fourCC << std::dec);
		return nullptr;
	}

	// Read pixel data - Compute data size using block sizes
	iDataSizeOut = 0; //iDataSizeOut = (iNumMipMaps > 1) ? header.pitchOrLinearSize * 4 / 3 : header.pitchOrLinearSize;
	int w = iWidthOut;
	int h = iHeightOut;
	for(int i = 0; i < iNumMipMaps; i++)
	{
		int bw = (w + 3) / 4;
		int bh = (h + 3) / 4;
		iDataSizeOut += bw * bh * iBlockSize;
		w = (w + 1) / 2;
		h = (h + 1) / 2;
		if(w == 0)
			w = 1;
		if(h == 0)
			h = 1;
	}

	uint8 *pData = reinterpret_cast<uint8 *>(malloc(iDataSizeOut));
	infile.read(reinterpret_cast<char *>(pData), iDataSizeOut);
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage - reading DDS file - " << ec.message() << ": " << sFilePath);
		free(pData);
		return nullptr;
	}

	infile.close();
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage - closing DDS file - " << ec.message() << ": " << sFilePath);
		free(pData);
		return nullptr;
	}

	return pData;
}
