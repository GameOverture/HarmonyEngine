/**************************************************************************
*	HyDiagnostics.cpp
*
*	Harmony Engine
*	Copyright (c) 2017 Jason Knobler
*
*	The zlib License (zlib)
*	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
*************************************************************************/
#include "Diagnostics/HyDiagnostics.h"

HyDiagnostics::HyDiagnostics()
{
#if defined(HY_PLATFORM_WINDOWS)
	m_sPlatform = "Windows";

	// Get number of CPUs
	SYSTEM_INFO sysinfo;
#if defined(HY_ENV_32)
	GetSystemInfo(&sysinfo);
#else
	GetNativeSystemInfo(&sysinfo);
#endif

	m_uiNumCPUs = sysinfo.dwNumberOfProcessors;

	// Get total memory size
	MEMORYSTATUSEX meminfo;
	GlobalMemoryStatusEx(&meminfo);
	m_uiRamSize = static_cast<uint64>(meminfo.ullTotalPhys); // as opposed to dwTotalPhys

#elif defined(HY_PLATFORM_OSX)
	// Set info
	m_sPlatform = "Mac OSX";
#endif

	// Confirm endian-ness with what is defined
	union
	{
		uint32 i;
		char c[4];
	} bint = { 0x01020304 };

#if defined(HY_ENDIAN_LITTLE)
	if(bint.c[0] == 1) {
		HyError("HY_ENDIAN_LITTLE was defined but did not pass calculation test");
	}
#else
	if(bint.c[0] != 1) {
		HyError("HY_ENDIAN_BIG was defined but did not pass calculation test");
}
#endif
}

HyDiagnostics::~HyDiagnostics()
{
}
