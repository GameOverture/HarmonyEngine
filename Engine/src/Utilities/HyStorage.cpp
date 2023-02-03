/**************************************************************************
*	HyStorage.cpp
*
*	Harmony Engine
*	Copyright (c) 2023 Jason Knobler
*
*	Harmony License:
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Afx/HyStdAfx.h"
#include "Utilities/HyStorage.h"

#ifdef HY_PLATFORM_BROWSER
EM_JS(int, HyStorage_Exists, (int iSessionOnly, const char *szKey),
	{
		var storageRef;
		if(iSessionOnly)
			storageRef = window.sessionStorage;
		else
			storageRef = window.localStorage;

		var jsKey = UTF8ToString(szKey);
		for(let i = 0; i < storageRef.length; i++)
		{
			if(jsKey === storageRef.key(i))
				return 1;
		}

		return 0;
	});

EM_JS(int, HyStorage_Length, (int iSessionOnly),
	{
		if(iSessionOnly)
			return window.sessionStorage.length;
		else
			return window.localStorage.length;
	});

EM_JS(char*, HyStorage_Key, (int iSessionOnly, int iIndex),
	{
		var jsKeyName;
		if(iSessionOnly)
			jsKeyName = window.sessionStorage.key(iIndex);
		else
			jsKeyName = window.localStorage.key(iIndex);

		var lengthBytes = lengthBytesUTF8(jsKeyName) + 1;
		var stringOnWasmHeap = _malloc(lengthBytes);
		stringToUTF8(jsKeyName, stringOnWasmHeap, lengthBytes);
		return stringOnWasmHeap;
	});

EM_JS(int, HyStorage_GetInt, (int iSessionOnly, const char* szKey),
	{
		if(iSessionOnly)
			return window.sessionStorage.getItem(UTF8ToString(szKey));
		else
			return window.localStorage.getItem(UTF8ToString(szKey));
	});

EM_JS(double, HyStorage_GetDbl, (int iSessionOnly, const char* szKey),
	{
		if(iSessionOnly)
			return window.sessionStorage.getItem(UTF8ToString(szKey));
		else
			return window.localStorage.getItem(UTF8ToString(szKey));
	});

EM_JS(char*, HyStorage_GetStr, (int iSessionOnly, const char* szKey),
	{
		var jsValueStr;
		if(iSessionOnly)
			jsValueStr = window.sessionStorage.getItem(UTF8ToString(szKey));
		else
			jsValueStr = window.localStorage.getItem(UTF8ToString(szKey));

		var lengthBytes = lengthBytesUTF8(jsValueStr) + 1;
		var stringOnWasmHeap = _malloc(lengthBytes);
		stringToUTF8(jsValueStr, stringOnWasmHeap, lengthBytes);
		return stringOnWasmHeap;
	});

EM_JS(void, HyStorage_SetInt, (int iSessionOnly, const char *szKey, int iValue),
	{
		if(iSessionOnly)
			window.sessionStorage.setItem(UTF8ToString(szKey), iValue);
		else
			window.localStorage.setItem(UTF8ToString(szKey), iValue);
	});

EM_JS(void, HyStorage_SetDbl, (int iSessionOnly, const char *szKey, double dValue),
	{
		if(iSessionOnly)
			window.sessionStorage.setItem(UTF8ToString(szKey), dValue);
		else
			window.localStorage.setItem(UTF8ToString(szKey), dValue);
	});

EM_JS(void, HyStorage_SetStr, (int iSessionOnly, const char *szKey, const char *szValue),
	{
		if(iSessionOnly)
			window.sessionStorage.setItem(UTF8ToString(szKey), UTF8ToString(szValue));
		else
			window.localStorage.setItem(UTF8ToString(szKey), UTF8ToString(szValue));
	});

EM_JS(void, HyStorage_RemoveItem, (int iSessionOnly, const char *szKey),
	{
		if(iSessionOnly)
			window.sessionStorage.removeItem(UTF8ToString(szKey));
		else
			window.localStorage.removeItem(UTF8ToString(szKey));
	});

EM_JS(void, HyStorage_Clear, (int iSessionOnly),
	{
		if(iSessionOnly)
			window.sessionStorage.clear();
		else
			window.localStorage.clear();
	});
#endif

HyStorage::HyStorage(bool bIsSessionOnly) :
	m_iSESSION_ONLY(bIsSessionOnly ? 1 : 0)
{
}

bool HyStorage::Exists(std::string sKey)
{
#ifdef HY_PLATFORM_BROWSER
	return HyStorage_Exists(m_iSESSION_ONLY, sKey.c_str());
#else
	return false;
#endif
}

uint32 HyStorage::Length()
{
#ifdef HY_PLATFORM_BROWSER
	return static_cast<uint32>(HyStorage_Length(m_iSESSION_ONLY));
#else
	return 0;
#endif
}

std::string HyStorage::Key(uint32 uiIndex)
{
#ifdef HY_PLATFORM_BROWSER
	std::string sKey;
	char *szKey = HyStorage_Key(m_iSESSION_ONLY, uiIndex);
	sKey = szKey;
	free(szKey);
	
	return sKey;
#else
	return std::string();
#endif
}

int32 HyStorage::GetInt(std::string sKey)
{
#ifdef HY_PLATFORM_BROWSER
	return HyStorage_GetInt(m_iSESSION_ONLY, sKey.c_str());
#else
	return 0;
#endif
}

double HyStorage::GetDouble(std::string sKey)
{
#ifdef HY_PLATFORM_BROWSER
	return HyStorage_GetDbl(m_iSESSION_ONLY, sKey.c_str());
#else
	return 0.0;
#endif
}

std::string HyStorage::GetString(std::string sKey)
{
#ifdef HY_PLATFORM_BROWSER
	std::string sValue;
	char *szValue = HyStorage_GetStr(m_iSESSION_ONLY, sKey.c_str());
	sValue = szValue;
	free(szValue);

	return sValue;
#else
	return std::string();
#endif
}

void HyStorage::SetItem(std::string sKey, int32 iValue)
{
#ifdef HY_PLATFORM_BROWSER
	HyStorage_SetInt(m_iSESSION_ONLY, sKey.c_str(), iValue);
#else
#endif
}

void HyStorage::SetItem(std::string sKey, double dValue)
{
#ifdef HY_PLATFORM_BROWSER
	HyStorage_SetDbl(m_iSESSION_ONLY, sKey.c_str(), dValue);
#else
#endif
}

void HyStorage::SetItem(std::string sKey, std::string sValue)
{
#ifdef HY_PLATFORM_BROWSER
	HyStorage_SetStr(m_iSESSION_ONLY, sKey.c_str(), sValue.c_str());
#else
#endif
}

void HyStorage::RemoveItem(std::string sKey)
{
#ifdef HY_PLATFORM_BROWSER
	HyStorage_RemoveItem(m_iSESSION_ONLY, sKey.c_str());
#else
#endif
}

void HyStorage::Clear()
{
#ifdef HY_PLATFORM_BROWSER
	HyStorage_Clear(m_iSESSION_ONLY);
#else
#endif
}
