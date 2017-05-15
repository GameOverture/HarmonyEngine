///**************************************************************************
// *	HyHardwareInfo.cpp
// *	
// *	Harmony Engine
// *	Copyright (c) 2012 Jason Knobler
// *
// *	The zlib License (zlib)
// *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
// *************************************************************************/
//#include "Utilities/HyHardwareInfo.h"
//
//HyHardwareInfo *	HyHardwareInfo::sm_pInstance = NULL;
//
//HyHardwareInfo::HyHardwareInfo()
//{
//
//
//}
//
//HyHardwareInfo *HyHardwareInfo::Get()
//{
//	if(sm_pInstance == NULL)
//		sm_pInstance = HY_NEW HyHardwareInfo();
//
//	return sm_pInstance;
//}
//
//const char *HyHardwareInfo::GetPlatform()		{ return m_szPlatform; }
//const char *HyHardwareInfo::GetDevelopmentURL()	{ return m_szDevURL; }
//const char *HyHardwareInfo::GetAssetDir()		{ return m_szAssetDir; }
//float HyHardwareInfo::GetDiscAccessSpeed()	{ return 0.0f; } // in seconds
//float HyHardwareInfo::GetDiscReadSpeed()		{ return 0.0f; } // in bytes/second
//HyHardwareInfo::eEndian HyHardwareInfo::GetEndian()	{ return m_eEndian; }
//uint32 HyHardwareInfo::GetNumCPUs()			{ return m_uiNumCPUs; }
//uint32 HyHardwareInfo::GetCPUSpeed()			{ return 0; }
//uint32 HyHardwareInfo::GetNumMemoryBanks()		{ return m_uiNumMemBanks; }
//size_t HyHardwareInfo::GetMemorySize()			{ return m_uiRAMsize; }
//
//void HyHardwareInfo::DumpStats(void)
//{
//	// TODO: print to output
//
//	//sgxTrace("SGX Engine - %s", GetPlatform());
//	//sgxTrace(" Local docs : %s", GetDocsURL());
//	//sgxTrace(" Remote site : %s", GetDevelopmentURL());
//
//	//sgxTrace("Disc Response:"); 
//	//sgxTrace(" Seek time (in ms) : %.2f", SGX_KILOBYTES*GetDiscAccessSpeed());
//	//sgxTrace(" Read time (in Mps) : %2.2f", GetDiscReadSpeed()/SGX_MEGABYTES);
//
//	//sgxTrace("CPUs:"); 
//	//sgxTrace(" Number : %d", GetNumCPUs());
//	//tUINT32 i;
//	//for(i=0; i<GetNumCPUs(); ++i) {
//	//	sgxTrace("  CPU %d speed : %d", i+1, GetCPUSpeed(i));
//	//}
//
//	//sgxTrace("Memory banks:"); 
//	//sgxTrace(" Number : %d", GetNumMemoryBanks());
//	//for(i=0; i<GetNumMemoryBanks(); ++i) {
//	//	sgxTrace("  Bank %d size : %d", i+1, GetMemorySize(i));
//	//}
//
//	// We can retrieve platform-specific data like this:
//	//     GetCPUSpeed(HyHardwareInfoPS2::tPS2CPU::eEE);
//}
//
////
//// Overload the type so we can save the enum as integers
////
////CSGXFile& operator>>(CSGXFile&File, HyHardwareInfo::tPlatform &v)
////{
////	tUINT32 as_int;
////	File >> as_int;
////	v = (HyHardwareInfo::tPlatform)as_int;
////
////	return File;
////}
////
////
////CSGXFile& operator<<(CSGXFile&File, HyHardwareInfo::tPlatform &v)
////{
////	tUINT32 as_int = (tUINT32)v;
////	File << as_int;
////
////	return File;
////}
