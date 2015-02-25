/**************************************************************************
 *	HyMemoryHeap.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/

#ifndef HY_PLATFORM_TOOL

#include "Memory/HyMemoryHeap.h"


HyMemoryHeap::HyMemoryHeap(void)
{

}


HyMemoryHeap::~HyMemoryHeap()
{
}


void *HyMemoryHeap::Alloc(size_t sz)
{
	//if(sz <= kSmlBlockSize)
	//{
	//	HySmlHeapPage *pPage
	//}

	return malloc(sz);
}


bool HyMemoryHeap::Free(void *ptr)
{
	free(ptr);
	return true;
}

#endif
