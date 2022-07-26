/**************************************************************************
 *	HyMemoryHeap.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyMemoryHeap_h__
#define HyMemoryHeap_h__

#include "Afx/HyStdAfx.h"

#include "PageTypes/HySmlHeapPage.h"
#include "PageTypes/HyMedHeapPage.h"

#include "Utilities/HyBitManip.h"

#ifdef HY_PLATFORM_WINDOWS
	#include "Memory/Interop/HyMemAPI_Windows.h"
#endif

class HyMemoryHeap
{
	//enum
	//{
	//	kNumSmlPageBuckets = HY_MEM_API::kNumSmlPages,
	//	kSmlPageSize = HY_MEM_API::kSmlPageSize,
	//};

	// SMALL PAGES //
	//-------------//
	// A 
	//HySmlHeapPage<kSmlPageSize> *		m_pSmlPages[kNumSmlPageBuckets];
	//HySmlHeapPage<kSmlPageSize> *		m_pSmlPagesFull[kNumSmlPageBuckets];

	//HyMedHeapPage< *		m_pMedPages[kNumSmallPages];

	//HY_MEM_API		m_MemAPI;

public:
	HyMemoryHeap();

	~HyMemoryHeap();
	
	void *Alloc(size_t sz);
	bool  Free (void *ptr);
};

#endif /* HyMemoryHeap_h__ */
