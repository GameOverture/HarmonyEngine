/**************************************************************************
 *	HyStrManip.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyStrManip_h__
#define HyStrManip_h__

#include "Afx/HyStdAfx.h"

namespace HyStr
{
	// Takes the dst char ptr and dynamically allocates the size of src and copies its contents.
	void DynamicStringCopy(char *&dst, const char *src);

	std::string MakeStringProperPath(const char *szPath, const char *szExtension, bool bMakeLowercase);

	// Trim whitespace from start (in place)
	void TrimLeft(std::string &sStrOut);

	// Trim whitespace from end (in place)
	void TrimRight(std::string &sStrOut);

	// Trim whitespace from both ends (in place)
	void Trim(std::string &sStrOut);

	// converts a string into a wstring
	std::wstring StringToWString(const std::string& p_Str);

	// converts a wstring into a string
	std::string WStringToString(const std::wstring& p_Str);

	uint32 StringToHash(const unsigned char *szStr);

	// Converts a given UTF-8 encoded character (array) to its UTF-32 LE equivalent
	uint32 HyUtf8_to_Utf32(const char *pChar, uint32 &uiNumBytesUsedRef);
}

#endif /* HyStrManip_h__ */

