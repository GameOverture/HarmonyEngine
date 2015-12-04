/**************************************************************************
 *	HyHardwareInfo.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyHardwareInfo_h__
#define __HyHardwareInfo_h__

#include "Afx/HyStdAfx.h"

class HyHardwareInfo 
{
public:

	typedef enum 
	{ 
		eBigEndian = 0,
		eLittleEndian = 1,

		ePadding = HY_ENUM_PADDING,
	} eEndian;

private:

	static HyHardwareInfo *	sm_pInstance;

	const char *		m_szPlatform;
	const char *		m_szAssetDir;
	const char *		m_szDevURL;
	eEndian				m_eEndian;
	uint32				m_uiNumCPUs;

	uint32				m_uiNumMemBanks;
	size_t				m_uiRAMsize;

public:

	static HyHardwareInfo  *Get();
	
	const char *GetPlatform();
	const char *GetDevelopmentURL();
	const char *GetAssetDir();
	float		GetDiscAccessSpeed();     // in seconds
	float		GetDiscReadSpeed();       // in bytes/second
	eEndian		GetEndian();
	uint32		GetNumCPUs();
	uint32		GetCPUSpeed();
	uint32		GetNumMemoryBanks();
	size_t		GetMemorySize();

	void        DumpStats();

protected:
	HyHardwareInfo();
};

	
//CSGXFile& operator>>(CSGXFile&File, HyHardwareInfo::ePlatform &v);
//CSGXFile& operator<<(CSGXFile&File, HyHardwareInfo::ePlatform &v);

#endif /* __HyHardwareInfo_h__ */
