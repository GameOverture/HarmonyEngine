/**************************************************************************
 *	HyStrManip.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyStrManip.h"

namespace HyStr
{
	// Takes the dst char ptr and dynamically allocates the size of src and copies its contents.
	void DynamicStringCopy(char *&dst, const char *src)
	{
		dst = nullptr;
		size_t iStrSize = strlen(src) + 1; // +1 for NULL terminator
		dst = HY_NEW char[iStrSize];
		strcpy(dst, src);
	}

	std::string MakeStringProperPath(const char *szPath, const char *szExtension, bool bMakeLowercase)
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

	bool Compare(const std::string &sFirst, const std::string &sSecond, bool bCaseSensitive /*= false*/)
	{
		if(bCaseSensitive)
			return std::equal(sFirst.begin(), sFirst.end(), sSecond.begin(), sSecond.end(), [](char a, char b) { return tolower(a) == tolower(b); });

		return sFirst == sSecond;
	}

	// Trim whitespace from start (in place)
	void TrimLeft(std::string &sStrOut)
	{
		sStrOut.erase(sStrOut.begin(), std::find_if(sStrOut.begin(), sStrOut.end(),	[](int ch)
																					{
																						return !std::isspace(ch);
																					}));
	}

	// Trim whitespace from end (in place)
	void TrimRight(std::string &sStrOut)
	{
		sStrOut.erase(std::find_if(sStrOut.rbegin(), sStrOut.rend(),[](int ch)
																	{
																		return !std::isspace(ch);
																	}).base(), sStrOut.end());
	}

	// Trim whitespace from both ends (in place)
	void Trim(std::string &sStrOut)
	{
		TrimLeft(sStrOut);
		TrimRight(sStrOut);
	}

	// code from http://www.c-plusplus.de/forum/viewtopic-var-t-is-168607.html
	std::wstring StringToWString(const std::string& p_Str)
	{
		if(p_Str.empty())
			return std::wstring();

		const std::ctype<wchar_t>& CType = std::use_facet<std::ctype<wchar_t> >(std::locale());
		std::vector<wchar_t> wideStringBuffer(p_Str.length());
		CType.widen(p_Str.data(), p_Str.data() + p_Str.length(), &wideStringBuffer[0]);
		return std::wstring(&wideStringBuffer[0], wideStringBuffer.size()); 
	}

	std::string WStringToString(const std::wstring& p_Str)
	{
		if(p_Str.empty())
			return std::string();

		const std::ctype<wchar_t>& CType = std::use_facet<std::ctype<wchar_t> >(std::locale());
		std::vector<char> narrowStringBuffer(p_Str.length());
		CType.narrow(p_Str.data(), p_Str.data() + p_Str.length(), ' ', &narrowStringBuffer[0]);
		return std::string(&narrowStringBuffer[0], narrowStringBuffer.size()); 
	}

	uint32 StringToHash(const unsigned char *szStr)
	{
		uint32 uiHash = 5381;
		int c;

		while(c = *szStr++)
			uiHash = ((uiHash << 5) + uiHash) + c; /* hash * 33 + c */

		return uiHash;
	}

	// Converts a given UTF-8 encoded character (array) to its UTF-32 LE equivalent
	uint32 HyUtf8_to_Utf32(const char *pChar, uint32 &uiNumBytesUsedRef)
	{
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
} // namespace HyStr
