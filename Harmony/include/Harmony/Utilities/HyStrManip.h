/**************************************************************************
 *	HyStrManip.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyStrManip_h__
#define __HyStrManip_h__

#include "Afx/HyStdAfx.h"

#include <string>

// Takes the dst char ptr and dynamically allocates the size of src and copies its contents.
void DynamicStringCopy(char *&dst, const char *src);

std::string MakeStringProperPath(const char *szPath, const char *szExtension, bool bMakeLowercase);

// converts a string into a wstring
std::wstring StringToWString(const std::string& p_Str);

// converts a wstring into a string
std::string WStringToString(const std::wstring& p_Str);

uint32 StringToHash(const unsigned char *szStr);

#endif /* __HyStrManip_h__ */

