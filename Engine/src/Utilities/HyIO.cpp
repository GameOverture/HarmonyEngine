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

#include "soil2/SOIL2.h"

#include <fstream>
#include <iomanip>

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

	szStr = &sStrRef.at(uiNumByteOffset);
	uiUtf8Count = 0;
	size_t uiNumByteCount = 0;
	while(*szStr != '\0')
	{
		if((*szStr & 0xC0) != 0x80)
		{
			if(uiUtf8Count == uiOffset)
				break;

			++uiUtf8Count;
		}
		++uiNumByteCount;
		++szStr;
	}

	sStrRef = sStrRef.erase(uiNumByteOffset, uiNumByteCount);
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

	// TODO: Fix
	sStrRef = sStrRef.insert(uiOffset, sUtf8Str);
}

/*static*/ std::string HyIO::CleanPath(const char *szPath, const char *szExtension, bool bMakeLowercase)
{
	std::string sPath(szPath ? szPath : "");
	std::replace(sPath.begin(), sPath.end(), '\\', '/');

	if(szExtension)
	{
		std::string sExtension(szExtension);
		//if(sExtension[0] != '.')
		//	sExtension = "." + sExtension;	// <-- Don't prepend '.' to szExtension, because GUI tool ItemDir's use just '/' as an extension

		if(sPath.empty() || 0 != strcmp(&sPath[sPath.length() - sExtension.size()], sExtension.c_str()))
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


	if(bMakeLowercase)
		transform(sPath.begin(), sPath.end(), sPath.begin(), ::tolower);

	return sPath;
}

/*static*/ bool HyIO::FileExists(const std::string &sFilePath)
{
	std::ifstream infile(sFilePath.c_str());
	return infile.good();
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

	std::ifstream infile(szFilePath, std::ios::binary | std::ios::ate);
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
