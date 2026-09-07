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

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <cwctype>
#include <regex>

#pragma pack(push, 1)
struct HY_HYTX_HEADER
{
	uint32_t uiReserve32;	// Pads the magic number to 8 bytes incase it needs to increase
	uint32_t uiWidth;
	uint32_t uiHeight;
	uint32_t uiNumChannels;
	uint32_t uiFormatCode;
	uint64_t uiReserve64;	// Incase more information should be added
};

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
/*	The dwFlags member of the original DDSURFACEDESC2 structure
	can be set to one or more of the following values.	*/
#define DDSD_CAPS	0x00000001
#define DDSD_HEIGHT	0x00000002
#define DDSD_WIDTH	0x00000004
#define DDSD_PITCH	0x00000008
#define DDSD_PIXELFORMAT	0x00001000
#define DDSD_MIPMAPCOUNT	0x00020000
#define DDSD_LINEARSIZE	0x00080000
#define DDSD_DEPTH	0x00800000

/*	DirectDraw Pixel Format	*/
#define DDPF_ALPHAPIXELS	0x00000001
#define DDPF_FOURCC	0x00000004
#define DDPF_RGB	0x00000040
#define DDPF_LUMINANCE 0x20000

/*	The dwCaps1 member of the DDSCAPS2 structure can be
	set to one or more of the following values.	*/
#define DDSCAPS_COMPLEX	0x00000008
#define DDSCAPS_TEXTURE	0x00001000
#define DDSCAPS_MIPMAP	0x00400000

/*	The dwCaps2 member of the DDSCAPS2 structure can be
	set to one or more of the following values.		*/
#define DDSCAPS2_CUBEMAP	0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX	0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX	0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY	0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY	0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ	0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ	0x00008000
#define DDSCAPS2_VOLUME	0x00200000

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

/*static*/ uint8 *HyIO::ReadImage(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut)
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
				return ReadImage(sFilePath, loadHintsInOut, uiDataSizeOut);
			}
		}
		break; }

	case HYIMAGE_PNG:
		pData = ReadImage_PNG(sFilePath, loadHintsInOut, uiDataSizeOut);
		break;

	case HYIMAGE_HYTX:
		pData = ReadImage_HYTX(sFilePath, loadHintsInOut, uiDataSizeOut);
		break;

	case HYIMAGE_DDS:
		pData = ReadImage_DDS(sFilePath, loadHintsInOut, uiDataSizeOut);
		break;

	case HYIMAGE_ASTC:
		pData = ReadImage_ASTC(sFilePath, loadHintsInOut, uiDataSizeOut);
		break;

	default:
		HyError("HyIO::ReadImage - Invalid image type specified");
		break;
	}

	return pData;
}

/*static*/ void HyIO::DeleteImage(uint8 *pImageData)
{
	stbi_image_free(pImageData);
}

/*static*/ bool HyIO::WriteImage(const std::string &sFilePath, HyImageInfo imageInfo, uint8 *pData)
{
	if(imageInfo.GetWidth() <= 0 || imageInfo.GetHeight() <= 0 || imageInfo.GetNumChannels() <= 0 || pData == nullptr)
	{
		HyLogWarning("HyIO::WriteImage - passed invalid parameters");
		return false;
	}

	bool bResult = false;
	switch(imageInfo.GetType())
	{
	case HYIMAGE_Unknown: {
		std::string sExt = GetExtensionFromPath(sFilePath);
		std::transform(sExt.begin(), sExt.end(), sExt.begin(), ::tolower);
		for(int i = 0; i < HYNUM_IMAGETYPES; ++i)
		{
			if(sExt == HyImageInfo::GetExt(static_cast<HyImageType>(i)))
				return WriteImage(sFilePath, imageInfo, pData);
		}
		break; }

	case HYIMAGE_PNG:
		stbi_flip_vertically_on_write(imageInfo.IsVerticalFlip() ? 1 : 0);
		bResult = stbi_write_png(sFilePath.c_str(), imageInfo.GetWidth(), imageInfo.GetHeight(), imageInfo.GetNumChannels(), pData, 0) != 0;
		break;

	case HYIMAGE_HYTX:
		bResult = WriteImage_HYTX(sFilePath, imageInfo, pData);
		break;

	case HYIMAGE_DDS:
		bResult = WriteImage_DDS(sFilePath, imageInfo, pData);
		break;

	case HYIMAGE_ASTC:
		HyLogWarning("HyIO::WriteImage - Writing ASTC images is not supported via HyEngine, you can only export ASTC images using the editor");
		break;
	}

	return bResult;
}

/*static*/ uint8 *HyIO::ReadImage_PNG(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut)
{
	uint8 *pData = nullptr;
	int iWidthOut, iHeightOut, iNumChannelsOut;
	if(loadHintsInOut.GetFormat() == HYTEXFORMAT_UINT8 || loadHintsInOut.GetFormat() == HYTEXFORMAT_Unknown)
	{
		stbi_set_flip_vertically_on_load(loadHintsInOut.IsVerticalFlip() ? 0 : 1); // Is image already flipped?
		pData = stbi_load(sFilePath.c_str(), &iWidthOut, &iHeightOut, &iNumChannelsOut, loadHintsInOut.GetNumChannels());
		if(pData == nullptr)
		{
			HyLogError("HyIO::ReadImage - " << stbi_failure_reason());
			return nullptr;
		}
		uiDataSizeOut = iWidthOut * iHeightOut * iNumChannelsOut;
	}
	else if(loadHintsInOut.GetFormat() == HYTEXFORMAT_UINT16)
	{
		stbi_set_flip_vertically_on_load(loadHintsInOut.IsVerticalFlip() ? 0 : 1); // Is image already flipped?
		pData = reinterpret_cast<uint8 *>(stbi_load_16(sFilePath.c_str(), &iWidthOut, &iHeightOut, &iNumChannelsOut, loadHintsInOut.GetNumChannels()));
		if(pData == nullptr)
		{
			HyLogError("HyIO::ReadImage - " << stbi_failure_reason());
			return nullptr;
		}
		uiDataSizeOut = iWidthOut * iHeightOut * (iNumChannelsOut * 2);
	}
	else
	{
		HyLogError("HyIO::ReadImage - Unhandled PNG format type");
		return nullptr;
	}

	loadHintsInOut.SetWidth(iWidthOut);
	loadHintsInOut.SetHeight(iHeightOut);
	loadHintsInOut.SetNumChannels(iNumChannelsOut);

	return pData;
}

/*static*/ uint8 *HyIO::ReadImage_HYTX(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut)
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

	HY_HYTX_HEADER header;
	infile.read(reinterpret_cast<char *>(&header), sizeof(HY_HYTX_HEADER));
	loadHintsInOut.SetWidth(header.uiWidth);
	loadHintsInOut.SetHeight(header.uiHeight);
	loadHintsInOut.SetNumChannels(header.uiNumChannels);
	switch(header.uiFormatCode)
	{
	case 0x55493038: // "UI08"
		loadHintsInOut.SetFormat(HYTEXFORMAT_UINT8);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * loadHintsInOut.GetNumChannels();
		break;
	case 0x53493038: // "SI08"
		loadHintsInOut.SetFormat(HYTEXFORMAT_INT8);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * loadHintsInOut.GetNumChannels();
		break;
	case 0x55493136: // "UI16"
		loadHintsInOut.SetFormat(HYTEXFORMAT_UINT16);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * (loadHintsInOut.GetNumChannels() * 2);
		break;
	case 0x53493136: // "SI16"
		loadHintsInOut.SetFormat(HYTEXFORMAT_INT16);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * (loadHintsInOut.GetNumChannels() * 2);
		break;
	case 0x55493332: // "UI32"
		loadHintsInOut.SetFormat(HYTEXFORMAT_UINT32);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * (loadHintsInOut.GetNumChannels() * 4);
		break;
	case 0x53493332: // "SI32"
		loadHintsInOut.SetFormat(HYTEXFORMAT_INT32);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * (loadHintsInOut.GetNumChannels() * 4);
		break;
	case 0x53463136: // "SF16"
		loadHintsInOut.SetFormat(HYTEXFORMAT_FLOAT16);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * (loadHintsInOut.GetNumChannels() * 2);
		break;
	case 0x53463332: // "SF32"
		loadHintsInOut.SetFormat(HYTEXFORMAT_FLOAT32);
		uiDataSizeOut = loadHintsInOut.GetWidth() * loadHintsInOut.GetHeight() * (loadHintsInOut.GetNumChannels() * 4);
		break;
	default:
		HyLogError("HyIO::ReadImage_HYTX - Invalid format read");
		return nullptr;
	}

	uint8 *pData = reinterpret_cast<uint8 *>(malloc(uiDataSizeOut));
	infile.read(reinterpret_cast<char *>(pData), uiDataSizeOut);
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage_HYTX - reading HYTX file - " << ec.message() << ": " << sFilePath);
		free(pData);
		return nullptr;
	}

	infile.close();
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage_HYTX - closing HYTX file - " << ec.message() << ": " << sFilePath);
		free(pData);
		return nullptr;
	}

	return pData;
}

/*static*/ uint8 *HyIO::ReadImage_DDS(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut)
{
	std::error_code ec;
	std::ifstream infile(sFilePath, std::ifstream::in | std::ios::binary);
	if(!infile || infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage_DDS - opening file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	uint32 uiMagic;
	infile.read(reinterpret_cast<char *>(&uiMagic), sizeof(uint32));
	if(uiMagic != 0x20534444) // "DDS "
	{
		HyLogError("HyIO::ReadImage_DDS - Invalid magic number");
		return nullptr;
	}

	HY_DDS_HEADER header;
	infile.read(reinterpret_cast<char *>(&header), sizeof(HY_DDS_HEADER));
	loadHintsInOut.SetWidth(header.width);
	loadHintsInOut.SetHeight(header.height);
	int iNumMipMaps = header.mipMapCount;
	if(iNumMipMaps == 0)
		iNumMipMaps = 1;
	int iBlockSize;
	switch(header.pf.fourCC)
	{
	case 0x31545844:		// "DXT1"
		loadHintsInOut.SetNumChannels(3); // TODO: also check for 4 channels (RGBA w/ 1-bit alpha)
		loadHintsInOut.SetFormat(HYTEXFORMAT_BC1_DXT1);
		iBlockSize = 8;		// 8 bytes per 4x4 block
		break;
	case 0x33545844:		// "DXT3" (mostly obsolete)
		loadHintsInOut.SetNumChannels(4);
		loadHintsInOut.SetFormat(HYTEXFORMAT_BC2_DXT3);
		iBlockSize = 16;	// 16 bytes per 4x4 block
		break;
	case 0x35545844:		// "DXT5"
		loadHintsInOut.SetNumChannels(4);
		loadHintsInOut.SetFormat(HYTEXFORMAT_BC3_DXT5);
		iBlockSize = 16;	// 16 bytes per 4x4 block
		break;
	case 0x31495441:		// "ATI1"
	case 0x55344342:		// "BC4U"
		loadHintsInOut.SetNumChannels(1);
		loadHintsInOut.SetFormat(HYTEXFORMAT_BC4_RGTC1);
		iBlockSize = 8;		// 8 bytes per 4x4 block
		break;
	case 0x53344342:		// "BC4S"
		loadHintsInOut.SetNumChannels(1);
		loadHintsInOut.SetFormat(HYTEXFORMAT_BC4_SIGNED_RGTC1);
		iBlockSize = 8;		// 8 bytes per 4x4 block
		break;
	case 0x32495441:		// "ATI2"
	case 0x55354342:		// "BC5U"
		loadHintsInOut.SetNumChannels(2);
		loadHintsInOut.SetFormat(HYTEXFORMAT_BC5_RGTC2);
		iBlockSize = 16;	// 16 bytes per 4x4 block
		break;
	case 0x53354342:		// "BC5S"
		loadHintsInOut.SetNumChannels(2);
		loadHintsInOut.SetFormat(HYTEXFORMAT_BC5_SIGNED_RGTC2);
		iBlockSize = 16;	// 16 bytes per 4x4 block
		break;

	default:
		HyLogError("HyIO::ReadImage_DDS - Unhandled fourCC: " << std::hex << header.pf.fourCC << std::dec);
		return nullptr;
	}

	// Read pixel data - Compute data size using block sizes
	uiDataSizeOut = 0; //iDataSizeOut = (iNumMipMaps > 1) ? header.pitchOrLinearSize * 4 / 3 : header.pitchOrLinearSize;
	int w = header.width;
	int h = header.height;
	for(int i = 0; i < iNumMipMaps; i++)
	{
		int bw = (w + 3) / 4;
		int bh = (h + 3) / 4;
		uiDataSizeOut += bw * bh * iBlockSize;
		w = (w + 1) / 2;
		h = (h + 1) / 2;
		if(w == 0)
			w = 1;
		if(h == 0)
			h = 1;
	}
	HyAssert(uiDataSizeOut == header.pitchOrLinearSize, "HyIO::ReadImage_DDS - Data size did not match pitchOrLinearSize"); // TODO: Support MipMaps

	uint8 *pData = reinterpret_cast<uint8 *>(malloc(uiDataSizeOut));
	infile.read(reinterpret_cast<char *>(pData), uiDataSizeOut);
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage_DDS - reading DDS file - " << ec.message() << ": " << sFilePath);
		free(pData);
		return nullptr;
	}

	infile.close();
	if(infile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::ReadImage_DDS - closing DDS file - " << ec.message() << ": " << sFilePath);
		free(pData);
		return nullptr;
	}

	return pData;
}

/*static*/ uint8 *HyIO::ReadImage_ASTC(const std::string &sFilePath, HyImageInfo &loadHintsInOut, uint32_t &uiDataSizeOut)
{
	// TODO: Read the image file in-place instead of needing a memcpy below
	std::vector<uint8> astcData;
	HyIO::ReadBinaryFile(sFilePath, astcData);
	if(astcData.empty())
	{
		HyLogError("HyFileAtlas::ReadImage_ASTC - failed to read binary file: " << sFilePath);
		return nullptr;
	}

	// ASTC header declaration.
	struct AstcHeader
	{
		unsigned char  magic[4];
		unsigned char  blockdim_x;
		unsigned char  blockdim_y;
		unsigned char  blockdim_z;
		unsigned char  xsize[3];   /* x-size = xsize[0] + xsize[1] + xsize[2] */
		unsigned char  ysize[3];   /* x-size, y-size and z-size are given in texels */
		unsigned char  zsize[3];   /* block count is inferred */
	};

	// Traverse the file structure
	AstcHeader *pAstcHeader = reinterpret_cast<AstcHeader *>(astcData.data());

	// Store number of bytes for each dimension
	// Merge x,y,z-sizes from 3 chars into one integer value
	int32 xsize = pAstcHeader->xsize[0] + (pAstcHeader->xsize[1] << 8) + (pAstcHeader->xsize[2] << 16);
	int32 ysize = pAstcHeader->ysize[0] + (pAstcHeader->ysize[1] << 8) + (pAstcHeader->ysize[2] << 16);
	int32 zsize = pAstcHeader->zsize[0] + (pAstcHeader->zsize[1] << 8) + (pAstcHeader->zsize[2] << 16);

	// Number of blocks in the x, y and z direction
	int32 xblocks = (xsize + pAstcHeader->blockdim_x - 1) / pAstcHeader->blockdim_x;
	int32 yblocks = (ysize + pAstcHeader->blockdim_y - 1) / pAstcHeader->blockdim_y;
	int32 zblocks = (zsize + pAstcHeader->blockdim_z - 1) / pAstcHeader->blockdim_z;

	// Each block is encoded on 16 bytes, so calculate total compressed image data size
	uiDataSizeOut = xblocks * yblocks * zblocks << 4;

	uint8 *pData = reinterpret_cast<uint8 *>(malloc(uiDataSizeOut));
	memcpy(pData, &pAstcHeader[1], uiDataSizeOut);

	return pData;
}

/*static*/ bool HyIO::WriteImage_HYTX(const std::string &sFilePath, HyImageInfo imageInfo, uint8 *pData)
{
	std::streamsize uiDataSize;

	HY_HYTX_HEADER header;
	header.uiReserve32 = 0;
	header.uiWidth = imageInfo.GetWidth();
	header.uiHeight = imageInfo.GetHeight();
	header.uiNumChannels = imageInfo.GetNumChannels();
	switch(imageInfo.GetFormat())
	{
	case HYTEXFORMAT_Unknown:
		[[fallthrough]];
	case HYTEXFORMAT_UINT8:
		header.uiFormatCode = 0x55493038; // "UI08"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * imageInfo.GetNumChannels();
		break;
	case HYTEXFORMAT_INT8:
		header.uiFormatCode = 0x53493038; // "SI08"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * imageInfo.GetNumChannels();
		break;
	case HYTEXFORMAT_UINT16:
		header.uiFormatCode = 0x55493136; // "UI16"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * (imageInfo.GetNumChannels() * 2);
		break;
	case HYTEXFORMAT_INT16:
		header.uiFormatCode = 0x53493136; // "SI16"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * (imageInfo.GetNumChannels() * 2);
		break;
	case HYTEXFORMAT_UINT32:
		header.uiFormatCode = 0x55493332; // "UI32"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * (imageInfo.GetNumChannels() * 4);
		break;
	case HYTEXFORMAT_INT32:
		header.uiFormatCode = 0x53493332; // "SI32"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * (imageInfo.GetNumChannels() * 4);
		break;
	case HYTEXFORMAT_FLOAT16:
		header.uiFormatCode = 0x53463136; // "SF16"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * (imageInfo.GetNumChannels() * 2);
		break;
	case HYTEXFORMAT_FLOAT32:
		header.uiFormatCode = 0x53463332; // "SF32"
		uiDataSize = imageInfo.GetWidth() * imageInfo.GetHeight() * (imageInfo.GetNumChannels() * 4);
		break;
	default:
		HyLogError("HyIO::WriteImage_HYTX - Invalid format specified");
		return false;
	}
	header.uiReserve64 = 0;

	// Write file
	std::error_code ec;
	std::ofstream outfile(sFilePath, std::ios::binary);
	if(!outfile || outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_HYTX - " << ec.message() << ": " << sFilePath);
		return false;
	}

	uint32_t uiMagicNumber = 0x58545948; // "HYTX"
	outfile.write(reinterpret_cast<const char *>(&uiMagicNumber), sizeof(uint32_t));
	outfile.write(reinterpret_cast<const char *>(&header), sizeof(HY_HYTX_HEADER));
	outfile.write(reinterpret_cast<const char *>(pData), uiDataSize);
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_HYTX - writing file - " << ec.message() << ": " << sFilePath);
		return false;
	}
	if(!outfile.flush())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_HYTX - flushing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	outfile.close();
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_HYTX - closing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	return true;
}

//static inline uint8_t getChannel(const uint8_t *src, int width, int height, int x, int y, int channel, int numChannels)
//{
//	int cx = (x < width) ? x : width - 1;
//	int cy = (y < height) ? y : height - 1;
//	return src[(cy * width + cx) * numChannels + channel];
//}

/*static*/ bool HyIO::WriteImage_DDS(const std::string &sFilePath, HyImageInfo imageInfo, uint8 *pData)
{
	int iWidth = imageInfo.GetWidth();
	int iHeight = imageInfo.GetHeight();
	int iNumChannels = imageInfo.GetNumChannels();

	int iNumBlocksX = (imageInfo.GetWidth() + 3) / 4;
	int iNumBlocksY = (imageInfo.GetHeight() + 3) / 4;
	int iNumTotalBlocks = iNumBlocksX * iNumBlocksY;

	// Build DDS header
	HY_DDS_HEADER header;
	memset(&header, 0, sizeof(HY_DDS_HEADER));
	header.size = sizeof(HY_DDS_HEADER);
	header.flags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_LINEARSIZE | DDSD_MIPMAPCOUNT;
	header.width = iWidth;
	header.height = iHeight;
	header.mipMapCount = 1; // TODO: Support mipmaps
	header.caps = DDSCAPS_TEXTURE;
	header.pf.size = sizeof(HY_DDS_PIXELFORMAT);
	header.pf.flags = DDPF_FOURCC;

	int iBlockSize;
	switch(imageInfo.GetNumChannels())
	{
	case 1:
		if(imageInfo.GetFormat() != HYTEXFORMAT_BC4_SIGNED_RGTC1)
			header.pf.fourCC = 0x55344342; // "BC4U"
		else
			header.pf.fourCC = 0x53344342; // "BC4S"
		iBlockSize = 8;		// 8 bytes per 4x4 block
		header.pitchOrLinearSize = iNumTotalBlocks * iBlockSize;
		break;

	case 2:
		if(imageInfo.GetFormat() != HYTEXFORMAT_BC5_SIGNED_RGTC2)
			header.pf.fourCC = 0x55354342; // "BC5U"
		else
			header.pf.fourCC = 0x53354342; // "BC5S"
		iBlockSize = 16;	// 16 bytes per 4x4 block
		header.pitchOrLinearSize = iNumTotalBlocks * iBlockSize;
		break;

	case 3:
		header.pf.fourCC = 0x31545844; // "DXT1"
		iBlockSize = 8;		// 8 bytes per 4x4 block
		header.pitchOrLinearSize = iNumTotalBlocks * iBlockSize;
		break;
	
	case 4:
		header.pf.fourCC = 0x35545844; // "DXT5"
		iBlockSize = 16;	// 16 bytes per 4x4 block
		header.pitchOrLinearSize = iNumTotalBlocks * iBlockSize;
		break;
	
	default:
		HyLogError("HyIO::WriteImage_DDS - Unhandled fourCC: " << std::hex << header.pf.fourCC << std::dec);
		return nullptr;
	}
	HyAssert(header.pf.fourCC != 0, "HyIO::WriteImage_DDS - Did not set header.pf.fourCC");
	HyAssert(header.pitchOrLinearSize != 0, "HyIO::WriteImage_DDS - Did not set header.pitchOrLinearSize");

	std::vector<uint8_t> compressed(header.pitchOrLinearSize);

	// Compress each 4x4 block
	for(int by = 0; by < iNumBlocksY; by++)
	{
		for(int bx = 0; bx < iNumBlocksX; bx++)
		{
			uint8_t *dest = &compressed[(by * iNumBlocksX + bx) * iBlockSize];

			// Vertical flip: read from bottom of source
			int srcBy = imageInfo.IsVerticalFlip() ? (iNumBlocksY - 1 - by) : by;

			switch(iNumChannels)
			{
			case 3:
			case 4: {
				// Input: 16 pixels × 4 bytes (RGBA) = 64 bytes
				uint8_t block[16 * 4];
				for(int y = 0; y < 4; y++)
				{
					for(int x = 0; x < 4; x++)
					{
						int sx = bx * 4 + x;
						int sy = srcBy * 4 + y;
						uint8_t *px = &block[(y * 4 + x) * 4];

						int cx = (sx < iWidth) ? sx : iWidth - 1;
						int cy = (sy < iHeight) ? sy : iHeight - 1;
						px[0] = pData[(cy * iWidth + cx) * iNumChannels + 0];
						px[1] = pData[(cy * iWidth + cx) * iNumChannels + 1];
						px[2] = pData[(cy * iWidth + cx) * iNumChannels + 2];
						if(iNumChannels == 4)
							px[3] = pData[(cy * iWidth + cx) * iNumChannels + 3];
						else
							px[3] = 255;

						//px[0] = getChannel(pData, iWidth, iHeight, sx, sy, 0, iNumChannels);
						//px[1] = getChannel(pData, iWidth, iHeight, sx, sy, 1, iNumChannels);
						//px[2] = getChannel(pData, iWidth, iHeight, sx, sy, 2, iNumChannels);
						//px[3] = (iNumChannels >= 4)
							//? getChannel(pData, iWidth, iHeight, sx, sy, 3, iNumChannels)
							//: 255; // opaque if no alpha
					}
				}
				int alpha = (iNumChannels == 4) ? 1 : 0;
				stb_compress_dxt_block(dest, block, alpha, 0);
				break; }

			case 1: {
				// Input: 16 pixels × 1 byte (R) = 16 bytes
				uint8_t block[16];
				for(int y = 0; y < 4; y++)
				{
					for(int x = 0; x < 4; x++)
					{
						int sx = bx * 4 + x;
						int sy = srcBy * 4 + y;

						int cx = (sx < iWidth) ? sx : iWidth - 1;
						int cy = (sy < iHeight) ? sy : iHeight - 1;
						block[y * 4 + x] = pData[(cy * iWidth + cx) * iNumChannels + 0];
						//block[y * 4 + x] = getChannel(pData, iWidth, iHeight, sx, sy, 0, iNumChannels);
					}
				}
				stb_compress_bc4_block(dest, block);
				break; }

			case 2: {
				// Input: 16 pixels × 2 bytes (RG interleaved) = 32 bytes
				uint8_t block[16 * 2];
				for(int y = 0; y < 4; y++)
				{
					for(int x = 0; x < 4; x++)
					{
						int sx = bx * 4 + x;
						int sy = srcBy * 4 + y;
						int idx = (y * 4 + x) * 2;

						int cx = (sx < iWidth) ? sx : iWidth - 1;
						int cy = (sy < iHeight) ? sy : iHeight - 1;
						block[idx] = pData[(cy * iWidth + cx) * iNumChannels + 0];
						block[idx + 1] = pData[(cy * iWidth + cx) * iNumChannels + 1];
						//block[idx] = getChannel(pData, iWidth, iHeight, sx, sy, 0, iNumChannels); // R
						//block[idx + 1] = getChannel(pData, iWidth, iHeight, sx, sy, 1, iNumChannels); // G
					}
				}
				stb_compress_bc5_block(dest, block);
				break; }
			}
		}
	}
	

	//for(int by = 0; by < blocksY; by++)
	//{
	//	for(int bx = 0; bx < blocksX; bx++)
	//	{
	//		// Build a 4x4 RGBA block (pad with black if near edge)
	//		uint8_t block[4 * 4 * 4];
	//		for(int y = 0; y < 4; y++)
	//		{
	//			for(int x = 0; x < 4; x++)
	//			{
	//				int px = bx * 4 + x;
	//				int py = by * 4 + y;
	//				uint8_t *dst = &block[(y * 4 + x) * 4];
	//				if(px < iWidth && py < iHeight)
	//				{
	//					const uint8_t *src = &rgba[(py * iWidth + px) * 4];
	//					memcpy(dst, src, 4);
	//				}
	//				else
	//				{
	//					dst[0] = dst[1] = dst[2] = 0;
	//					dst[3] = 255;
	//				}
	//			}
	//		}

	//		// alpha = 1 (use DXT5)
	//		// mode = 0 (standard quality)
	//		stb_compress_dxt_block(&compressed[(by * blocksX + bx) * iBlockSize], block,	1, 0);
	//	}
	//}

	
	// Write file
	std::error_code ec;
	std::ofstream outfile(sFilePath, std::ios::binary);
	if(!outfile || outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_DDS - " << ec.message() << ": " << sFilePath);
		return false;
	}

	uint32_t uiMagicNumber = 0x20534444; // "DDS "
	outfile.write(reinterpret_cast<const char *>(&uiMagicNumber), sizeof(uint32_t));
	outfile.write(reinterpret_cast<const char *>(&header), sizeof(HY_DDS_HEADER));
	outfile.write(reinterpret_cast<const char *>(compressed.data()), compressed.size());
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_DDS - writing file - " << ec.message() << ": " << sFilePath);
		return false;
	}
	if(!outfile.flush())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_DDS - flushing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	outfile.close();
	if(outfile.bad())
	{
		ec = std::error_code(errno, std::generic_category());
		HyLogError("HyIO::WriteImage_DDS - closing file - " << ec.message() << ": " << sFilePath);
		return false;
	}

	return true;
}
