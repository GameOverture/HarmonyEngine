/**************************************************************************
 *	HyMemoryHeap.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2013 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyMemoryHeap_h__
#define __HyMemoryHeap_h__

#include "Afx/HyStdAfx.h"

#include "PageTypes/HySmlHeapPage.h"
#include "PageTypes/HyMedHeapPage.h"

#include "Utilities/HyBitManip.h"

#ifdef HY_PLATFORM_WINDOWS
	#include "Memory/Interop/HyMemAPI_Windows.h"
#endif

class HyMemoryHeap
{
	enum
	{
		kNumSmlPageBuckets = HY_MEMPLAT::kNumSmlPages,
		kSmlPageSize = HY_MEMPLAT::kSmlPageSize,
	};

	// SMALL PAGES //
	//-------------//
	// A 
	HySmlHeapPage<kSmlPageSize> *		m_pSmlPages[kNumSmlPageBuckets];
	HySmlHeapPage<kSmlPageSize> *		m_pSmlPagesFull[kNumSmlPageBuckets];

	//HyMedHeapPage< *		m_pMedPages[kNumSmallPages];

	HY_MEMPLAT		m_MemAPI;

public:
	HyMemoryHeap();

	~HyMemoryHeap();
	
	void *Alloc(uint32 sz);
	bool  Free (void *ptr);
};

#endif /* __HyMemoryHeap_h__ */
