/**************************************************************************
 *	HyStrManip.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#include "Utilities/HyStrManip.h"

#include <algorithm>
#include <vector>

// Takes the dst char ptr and dynamically allocates the size of src and copies its contents.
void DynamicStringCopy(char *&dst, const char *src)
{
	dst = NULL;
	size_t iStrSize = strlen(src) + 1; // +1 for NULL terminator
	dst = new char[iStrSize];
	strcpy(dst, src);
}

std::string MakeStringProperPath(const char *szPath, const char *szExtension)
{
	std::string sPath(szPath ? szPath : "");

	std::replace(sPath.begin(), sPath.end(), '\\', '/');

	if(sPath.empty() || 0 != strcmp(&sPath[sPath.length() - strlen(szExtension) - 1], szExtension))
		sPath.append(szExtension ? szExtension : "");
	
	transform(sPath.begin(), sPath.end(), sPath.begin(), ::tolower);

	return sPath;
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
