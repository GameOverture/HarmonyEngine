/**************************************************************************
 *	HyMemAPI_Windows.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyMemAPI_Windows_h__
#define HyMemAPI_Windows_h__

#include "Afx/HyStdAfx.h"

#define PAGE_SIZE 
#define NUM_CHUNKS 256

class HyMemoryAPI_Windows
{
	void *		m_pChunks[NUM_CHUNKS];
	uint32		m_UsedChunkFlags[NUM_CHUNKS/32];


	// Track the PAGES of small and medium allocation
	// Individual large allocs, are stored separately in that they're hopefully used sparingly
	std::set<void *>				m_SmallPages;	// this is a dumb implementation.  Do something smarter.
	std::set<void *>				m_MediumPages;
	std::map<void *, uint32>		m_mapLrgAllocs;	// pair of pointers and sizes

	uint32							m_uiUsedChunkFlags;
	uint32							m_uiUsedPagesFlags[NUM_CHUNKS];
								

	// We fetch this from the OS, and make sure it's what our page sizes are set to.
	uint32							m_uiNaturalPageSize;

public:
	HyMemoryAPI_Windows(void);

	// These enums are used in HyMemoryHeap
	enum
	{
		kNumSmlPages = 100,
		kMaxSmlBlockSize = 128,

		// Both must be a power of 2 for this code to work.  Although the page size
		// is generally 4k, meaning 4k of physical memory can be mapped at a time,
		// the VirtualAlloc function can only dole out memory ADDRESSES in 64k chunks,
		// so if we allocate 4k at a time, we're using up 60k of address space that we
		// will never be able to use.  32 bit programs will run out of address space 
		// before memory in that case.  Very bad.  So we allocate in the optimal chunk
		// sizes... 64k.
		kSmlPageSize  = 64*1024,
		kMedPageSize = 64*1024
	};

	// This is very important to be implemented in a fast way, so free is quick.
	void   *IsSmallBlock (void *ptr) const;  // returns the address of the PAGE this pointer is on
	void   *IsMediumBlock(void *ptr) const;
	uint32  IsLargeBlock (void *ptr) const;

	// these functions need to be overridden to change where and how 
	// the memory manager gets small pages.
	void   *AllocSmallPage(void);
	void    FreeSmallPage (void *ptr);

	// these functions need to be overridden to change where and how
	// the memory manager gets medium pages.
	void   *AllocMediumPage(void);
	void    FreeMediumPage (void *ptr);

	// these functions are called whenever the FMM needs a very large
	// single allocation.  Generally, in Windows this is better to just
	// map to VirtualAlloc, which guarantees no fragmentation but may
	// waste some memory between the end of the allocation and the end
	// of the physical memory page mapped to the address space.  Consoles
	// may direct this to a traditional linked list allocator or manually 
	// handle VirtualAlloc-like behavior by mapping memory blocks.
	void *AllocLarge(uint32 sz);

	// AllocSize is returned so the system can track how much memory is out in each type.
	void  FreeLarge (void *ptr, uint32 *allocSize);	
};

inline HyMemoryAPI_Windows::HyMemoryAPI_Windows(void)
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	m_uiNaturalPageSize = info.dwPageSize;

	// If this triggers, you are running in an inoptimal way and should change your 
	// size to match or be a multiple of natural page size of the OS.
	HyAssert(kSmlPageSize % m_uiNaturalPageSize == 0 && kMedPageSize % m_uiNaturalPageSize == 0, "kSmallPageSize is inoptimal and should be a multiple of natural page size of the OS");
	HyAssert(kSmlPageSize == kMedPageSize, "Page sizes must be identical to use this allocation policy");


}

inline void *HyMemoryAPI_Windows::IsSmallBlock(void *ptr) const
{
	void *pageStart = (void*)(reinterpret_cast<uint64_t>(ptr) & ~(kSmlPageSize-1));  // assume all small pages are aligned to their own size addresses, or better

	if (m_SmallPages.find(pageStart) != m_SmallPages.end())
		return pageStart;

	return NULL;
}

inline void *HyMemoryAPI_Windows::IsMediumBlock(void *ptr) const
{
	void *pageStart = (void*)(reinterpret_cast<uint64_t>(ptr) & ~(kMedPageSize-1));  // assume all medium pages are aligned to their own size addresses, or better
	
	if(m_MediumPages.find(pageStart) != m_MediumPages.end())
		return pageStart;

	return NULL;
}

//-------------------

inline uint32 HyMemoryAPI_Windows::IsLargeBlock(void *ptr) const
{
	return m_mapLrgAllocs.find(ptr) != m_mapLrgAllocs.end();
}

//-------------------

inline void *HyMemoryAPI_Windows::AllocSmallPage(void)
{
	void *pageAddress = VirtualAlloc(NULL, kSmlPageSize, MEM_COMMIT, PAGE_READWRITE);
	// TODO: assert(pageAddress);	
	m_SmallPages.insert(pageAddress);
	return pageAddress;
}

//-------------------

inline void  HyMemoryAPI_Windows::FreeSmallPage(void *ptr)
{
	VirtualFree(ptr, 0, MEM_RELEASE);
	m_SmallPages.erase(ptr);
}

//-------------------

inline void *HyMemoryAPI_Windows::AllocMediumPage(void)
{
	void *pageAddress = VirtualAlloc(NULL, kMedPageSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	// TODO: assert(pageAddress);
	m_MediumPages.insert(pageAddress);
	return pageAddress;
}

//-------------------

inline void  HyMemoryAPI_Windows::FreeMediumPage (void *ptr)
{
	VirtualFree(ptr, 0, MEM_RELEASE);
	m_MediumPages.erase(ptr);
}

//-------------------

inline void *HyMemoryAPI_Windows::AllocLarge(uint32 sz)
{
	void *addr = VirtualAlloc(NULL, sz, MEM_COMMIT, PAGE_READWRITE);
	// TODO: assert(addr);	
	m_mapLrgAllocs.insert(std::make_pair(addr, sz));
	return addr;
}

//-------------------

inline void  HyMemoryAPI_Windows::FreeLarge (void *ptr, uint32 *allocSize)
{
	std::map<void *, uint32>::iterator i = m_mapLrgAllocs.find(ptr);
	// TODO: assert(i!=mLargeAllocs.end());  // if this triggers, someone tried to delete a pointer we know nothing about
	if (allocSize)
		*allocSize = i->second;
	VirtualFree(ptr, 0, MEM_RELEASE);
	m_mapLrgAllocs.erase(i);
}

//-------------------

#endif /* HyMemAPI_Windows_h__ */
