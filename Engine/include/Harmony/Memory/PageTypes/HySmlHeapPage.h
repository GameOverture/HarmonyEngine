/**************************************************************************
 *	HyHeapPage.small.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	Harmony License:
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyHeapPageSmall_h__
#define HyHeapPageSmall_h__

#include "Afx/HyStdAfx.h"

template <size_t TPageSize>
class HySmlHeapPage
{
protected:
	// This will be found at the end of each page.
	struct PageTail
	{
		// immediately following PRIOR TO this begins the allocation bitmask,
		// which are used as bitwise markers for which chunks in the page are allocated.
		// Note, a FREE block has a SET bit.	
		size_t             mFreeCount;  // improves alloc and free speed, both.
		size_t             mBlockSize;  // necessary for Free() to be fast
		HySmlHeapPage *	mPrev;		// makes restructuring the list faster
		HySmlHeapPage *	mNext;
	};

	enum
	{
		// we can't precompute how large the bitmask will be because that varies with the
		// size of the block being allocated.  But we can precompute the offset to the
		// start of the tail structure in each page.
		kTailOffset = TPageSize - sizeof(PageTail),
	};

	// Simple address translation to get to the tail
	PageTail *GetPageTail (void) const { return (PageTail *)((char *)this + kTailOffset); }

public:
	// This is called only when a new one is allocated.
	void Initialize(size_t blockSize);

	// The page literally has no place to store any data except a bitmask 
	// and a pointer at the end of the page, so we calculate where the tail
	// is based on the cache line length and size of the page and tell this class
	// how to work based on that.  The LAST thing we want is to require a cache
	// line fetch in each small block page just to read a few silly configuration
	// variables, anyway.
	// The isFull bool is filled out so that the system knows when the page should
	// be moved to the full pages bucket.
	void *Alloc(size_t blockSize, bool *isFull);

	// The hard part of figuring out what page this pointer came from is handled 
	// at a higher level, mainly because all that depends on the policy the user creates.
	// It could be a bitmask of addresses, a simple masking operation on the address
	// itself, or a binary search over a sorted array of addresses, etc.
	// Returns true if the whole page is free, meaning the system can release the whole page.
	// Also fills out whether the page was full before the free occurred, so the system
	// can know which kind of page it came from.
	bool Free(void *ptr, bool *wasFull);

	// During the free operation when a page is being removed, we need to figure out the
	// block size so we can pass in the head pointer of the page list, otherwise if a page
	// needs to be freed, it will unlink stuff without the system knowing about it and it
	// will lose track of the page list.
	size_t GetBlockSize(void) const;

	// When a new page is added by the system, it asks us to handle the pointer handling.
	void InsertNewHeadPage(HySmlHeapPage **headPtr);

	// Easy function to untangle the prev/next pointers from the current list.
	void RemoveFromList(HySmlHeapPage **headPtr);

	// Debugging aid to verify that pointers make sense and bits agree with counts.
	void SanityCheck(void);
};

//-------------------

template <size_t TPageSize>
inline size_t HySmlHeapPage<TPageSize>::GetBlockSize() const
{
	return GetPageTail()->mBlockSize;
}

//-------------------

template <size_t TPageSize>
void HySmlHeapPage<TPageSize>::Initialize(size_t blockSize)
{
	// get a pointer to the tail so we can initialize it
	PageTail *pTail = GetPageTail();

	// figure out how many allocatable blocks are actually in this page, 
	// taking into account the size of the tail structure (which includes 
	// a bitmask that depends on the number of blocks in this page!)
	const size_t uiMaxAllocsPerPage     = (TPageSize - sizeof(PageTail))/blockSize;
	const size_t uiBytesRequiredForTail = (uiMaxAllocsPerPage+31)/32*4 + sizeof(PageTail);  // in bytes.  Rounded to nearest 32-bit value for performance.
	const size_t uiNumBlocks            = (TPageSize - uiBytesRequiredForTail) / blockSize;
	const size_t uiNumBytesInBitmask    = (uiNumBlocks+31)/32*4;

	pTail->mBlockSize = (size_t)blockSize;
	pTail->mFreeCount = (size_t)uiNumBlocks;
	pTail->mNext = NULL;
	pTail->mPrev = NULL;

	unsigned char *bitmask = (unsigned char*)pTail - uiNumBytesInBitmask;
	SetBitRangeTo1(bitmask, 0, uiNumBlocks-1);  // mark all block alloc masks as FREE
}

//-------------------
// This function never fails.  It is assumed the system knows there is space available before calling.
template <size_t TPageSize>
void *HySmlHeapPage<TPageSize>::Alloc(size_t blockSize, bool *isFull)
{
	// treat this as a block of memory with a piece of data at the end that we want to access
	PageTail *pTail = GetPageTail();
	HyAssert(pTail->mFreeCount > 0, "No more free memory blocks!? Should never fail here.");

	// figure out how many allocatable blocks are actually in this page, 
	// taking into account the size of the tail structure (which includes 
	// a bitmask that depends on the number of blocks in this page!)
	const size_t maxAllocsPerPage     = (TPageSize - sizeof(PageTail))/blockSize;
	const size_t bytesRequiredForTail = (maxAllocsPerPage+31)/32*4 + sizeof(PageTail);  // in bytes.  Rounded to nearest 32-bit value for performance.
	const size_t numBlocks            = (TPageSize - bytesRequiredForTail) / blockSize;
	const size_t numBytesInBitmask    = (numBlocks+31)/32*4;

	// scan for a free bit, which absolutely should be there
	unsigned char *bitmask = (unsigned char*)pTail - numBytesInBitmask;
	size_t oneBitIndex = FindFirst1BitInRange(bitmask, 0, numBlocks-1);
	HyAssert(oneBitIndex!=numBlocks, "No free bit for allocation!? Should never fail here");

	// clear the free bit
	SetBitTo0(bitmask, oneBitIndex);

	void *ptr = (unsigned char *)this + oneBitIndex * blockSize;

	// move this page to the head of the small list if there's anything left to allocate in it.
	pTail->mFreeCount--;
	*isFull = (pTail->mFreeCount==0);
	return ptr;
}

//-------------------

template <size_t TPageSize>
bool HySmlHeapPage<TPageSize>::Free(void *ptr, bool *wasFull)
{
	// treat this as a block of memory with a piece of data at the end that we want to access
	PageTail *pTail = GetPageTail();

	const size_t blockSize		      = pTail->mBlockSize;  // MEMORY ACCESS at the end of the page.  Fetches an L2 cache line.
	const size_t maxAllocsPerPage     = (TPageSize - sizeof(PageTail))/blockSize;
	const size_t bytesRequiredForTail = (maxAllocsPerPage+31)/32*4 + sizeof(PageTail);  // in bytes.  Rounded to nearest 32-bit value for performance.
	const size_t numBlocks            = (TPageSize - bytesRequiredForTail) / blockSize;
	const size_t numBytesInBitmask    = (numBlocks+31)/32*4;

	HyAssert(pTail->mFreeCount<numBlocks, "Should be at least one allocated block in this page, or somethin's broke!");
	HyAssert((((size_t)ptr - (size_t)this) % blockSize) == 0, "The address of our pointer should line up evenly on a block start, or somethin's broke!");
	unsigned char *bitmask = (unsigned char*)pTail - numBytesInBitmask;  // the bitmask comes right BEFORE the tail structure	
	size_t const blockNumber = ((size_t)ptr - (size_t)this) / blockSize;

	HyAssert(FindFirst0BitInRange(bitmask, blockNumber, blockNumber)==blockNumber, "Make sure this block is currently free");
	SetBitTo1(bitmask, blockNumber);  // mark this block as used
	pTail->mFreeCount++;             // keep account

	*wasFull = (pTail->mFreeCount == 1);  // if there's only one free block, this used to be a full page

	// release any page that becomes completely freed
	return (pTail->mFreeCount == numBlocks);
}

//-------------------

template <size_t TPageSize>
void HySmlHeapPage<TPageSize>::InsertNewHeadPage(HySmlHeapPage **headPtr)
{
	// very simply, insert 'newPage' ahead of this, fixing up both sets of pointers, then assign newPage to *headPtr.
	PageTail *pTail = GetPageTail();
	HySmlHeapPage *headPage = *headPtr;
	if (headPage)
	{
		PageTail *pageTail = headPage->GetPageTail();
		pageTail->mPrev = this;
	}

	pTail->mNext = headPage;
	*headPtr = this;
}

//-------------------

template <size_t TPageSize>
void HySmlHeapPage<TPageSize>::SanityCheck(void)
{
	HySmlHeapPage *currentPage = this;
	while (currentPage)
	{
		PageTail *tail = currentPage->GetPageTail();
		HyAssert(tail->mPrev==NULL || tail->mPrev->GetPageTail()->mNext==currentPage, "Previous page linked-list isn't pointing to current page.");
		HyAssert(tail->mNext==NULL || tail->mNext->GetPageTail()->mPrev==currentPage, "Next page linked-list isn't pointing to current page.");

		// count the bits that are marked as FREE and make sure they match the count in the tail
		const size_t blockSize            = tail->mBlockSize;  // MEMORY ACCESS at the end of the page.  Fetches an L2 cache line.
		const size_t maxAllocsPerPage     = (TPageSize - sizeof(PageTail))/blockSize;
		const size_t bytesRequiredForTail = (maxAllocsPerPage+31)/32*4 + sizeof(PageTail);  // in bytes.  Rounded to nearest 32-bit value for performance.
		const size_t numBlocks            = (TPageSize - bytesRequiredForTail) / blockSize;
		const size_t numBytesInBitmask    = (numBlocks+31)/32*4;		

		unsigned char *bitmask = (unsigned char*)tail - numBytesInBitmask;  // the bitmask comes right BEFORE the tail structure
		size_t bitsSet = 0;
		for (size_t i=0; i<numBlocks; i++)
		{
			if (FindFirst1BitInRange(bitmask, i, i)==i)
			{
				bitsSet++;
			}
		}
		HyAssert(bitsSet==tail->mFreeCount, "Bits don't match counter");

		currentPage = tail->mNext;  // check the next page in this list
	}
}

//-------------------

template <size_t TPageSize>
void HySmlHeapPage<TPageSize>::RemoveFromList(HySmlHeapPage **headPtr)
{
	// very simply, insert this ahead of the head page.
	PageTail *tail = GetPageTail();
	if (*headPtr == this)
	{
		*headPtr = tail->mNext;  // fixup the head of the list, in case it was us
	}	
	if (tail->mPrev)
	{
		PageTail *prevTail = tail->mPrev->GetPageTail();
		prevTail->mNext = tail->mNext;
	}
	if (tail->mNext)
	{
		PageTail *nextTail = tail->mNext->GetPageTail();
		nextTail->mPrev = tail->mPrev;	
	}
	tail->mNext = NULL;
	tail->mPrev = NULL;
}

//-------------------

#endif /* HyHeapPageSmall_h__ */
