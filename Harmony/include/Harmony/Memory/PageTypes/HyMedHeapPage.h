/**************************************************************************
 *	HyHeapPage.medium.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef __HyHeapPageMedium_h__
#define __HyHeapPageMedium_h__

#include "Afx/HyStdAfx.h"

template <uint32 TPageSize, uint32 TGranularity>
class HyMedHeapPage
{
	// This struct situated as the very last few bytes in the allocated page.
	struct Tail
	{
		uint32                mBigBlockIndex; // index in blocks, not bytes
		uint32                mBigBlockSize;  // counted in blocks, not bytes
		uint32                mFreeBits;
		HyMedHeapPage *mNext;
		HyMedHeapPage *mPrev;
	};

	enum
	{
		kIdealNumGranules = (TPageSize+TGranularity-1)/TGranularity,  // this is only used as an interim for computation
		kTotalTailSize = (kIdealNumGranules+31)/32*4 * 2 + sizeof(Tail),  // there are TWO bitmasks (rounded to an even 32-bits), so count it twice.  Also only used as an interim.

		kNumGranules = (TPageSize - kTotalTailSize) / TGranularity,
		kBitmaskSize = (kNumGranules+31)/32*4,  // size in bytes.  round up to an even 32 bits for each mask, for performance	
		kTailOffset = TPageSize - sizeof(Tail),		
		kStartFreeOffset = kTailOffset - kBitmaskSize,		
		kStartAllocFreeOffset = kTailOffset - kBitmaskSize - kBitmaskSize,
	};

	// shortcuts to clean up a bunch of messy code
	Tail *GetPageTail     (void) const { return (Tail*)((char *)this + kTailOffset); }	
	void *GetStartFree    (void) const { return (char *)this + kStartFreeOffset; }
	void *GetStartAllocEnd(void) const { return (char *)this + kStartAllocFreeOffset; }	

	// This updates the tail so it has correct information about the big block size.
	// We have to do this after every alloc and free.
	void UpdateBigBlock(void);

public:
	void Initialize(void);

	// When an allocation occurs, we push the page it allocated from to the head. 
	// Generally this will cause allocations to be close together in time and space,
	// but also open space tends to flock together and get used together with fewer traversals.
	void *Alloc(uint32 sz);

	// allocSize is filled out with the number of bytes the allocation ACTUALLY took.
	// This returns true if the entire page is freed now.  That indicates the system should release the page back to the OSAPI.
	bool Free (void *ptr, uint32 *allocSize);

	// When a new page is added by the system, this function manages the pointers.
	void InsertNewHeadPage(HyMedHeapPage **headPtr);

	// When a page needs to be alloc/freed from, we remove it from the list first since it often will change lists
	void RemoveFromList(HyMedHeapPage **headPtr);

	// fast accessor for the number of bits in the big block
	uint32 GetBigBlockSize(void) { return GetPageTail()->mBigBlockSize; }

	// This makes sure everything checks out internally.
	void SanityCheck(void);
};

//-------------------

template <uint32 TPageSize, uint32 TGranularity>
void HyMedHeapPage<TPageSize, TGranularity>::Initialize(void)
{
	Tail *tail = GetPageTail();

	// mark no bits in the end-of-allocation mask
	SetBitRangeTo0(GetStartAllocEnd(), 0, kNumGranules-1);

	// mark everything as free
	SetBitRangeTo1(GetStartFree(), 0, kNumGranules-1);

	// init the tail
	tail->mBigBlockIndex = 0;            // big block in a new page is at the start of the page	
	tail->mBigBlockSize = kNumGranules;
	tail->mFreeBits = kNumGranules;      // tracks exactly how many granules are free within the page.
	tail->mNext = NULL;
	tail->mPrev = NULL;
}

//-------------------

template <uint32 TPageSize, uint32 TGranularity>
void *HyMedHeapPage<TPageSize, TGranularity>::Alloc(uint32 sz)
{
	// The algorithm for allocation is pretty straightforward.
	// 1. Compute how many blocks this allocation requires.
	// 2. Scan forward through pages until we find one with at least that many free.
	// 3. Scan the bits in that page and see if there's a block of bits that is long enough.
	// 4. If so, mark those bits as zero, reduce the free bits count, move that page to the head of the list, and return the pointer.
	// 5. If not, go to #2 and continue the scan for good pages.
	uint32 const allocBitsRequired = (sz + TGranularity - 1) / TGranularity;

	Tail *tail = GetPageTail();
	HyAssert(tail->mFreeBits >= allocBitsRequired, "Not enough free bits for allocation");
	HyAssert(tail->mBigBlockSize >= allocBitsRequired, "Not enough free bits for allocation");

	void *bitmaskStartFree     = GetStartFree();	
	void *bitmaskStartAllocEnd = GetStartAllocEnd();

	// since we are always maintaining the location of the big block, and we know that the FMM internally manages it so that
	// we always are allocating from a page whose big block is as close-fitting to the allocation as possible, we will simply
	// allocate from the big block directly and then recompute it.
	void *ptr = (char *)this + tail->mBigBlockIndex * TGranularity;

	// mark the blocks as being ALLOCATED now
	SetBitRangeTo0(bitmaskStartFree, tail->mBigBlockIndex, tail->mBigBlockIndex+allocBitsRequired-1);

	// mark the end of the allocation
	SetBitTo1(bitmaskStartAllocEnd, tail->mBigBlockIndex+allocBitsRequired-1);

	// now, figure out how big the bigblock on this page is now that we've taken a bite out of it
	UpdateBigBlock();
	tail->mFreeBits -= allocBitsRequired;

	return ptr;
}

//-------------------

template <uint32 TPageSize, uint32 TGranularity>
bool HyMedHeapPage<TPageSize, TGranularity>::Free(void *ptr, uint32 *allocSize)
{
	HyAssert((((uint32)ptr - (uint32)this) % TGranularity) == 0, "if this is false, it means we're trying to delete some random address in a page.");

	// Freeing an object is as simple as walking a bit mask and marking allocated granules as free, until we find the end of the alloc in the AllocEnd mask.
	// We can also tell if there is corruption of some sort by asserting that bits are marked allocated when they should be.
	uint32 const startingBit = ((uint32)ptr - (uint32)this) / TGranularity;

	Tail *tail                 = GetPageTail();
	void *bitmaskStartFree     = GetStartFree();	
	void *bitmaskStartAllocEnd = GetStartAllocEnd();

	// find the end of the allocation by scanning the bitmask for a 1 bit
	uint32 const endOfAlloc = FindFirst1BitInRange(bitmaskStartAllocEnd, startingBit, kNumGranules);
	HyAssert(endOfAlloc<kNumGranules, "never found the end of the allocation.  That means something is busted.");

	// mark the region of memory as free
	SetBitRangeTo1(bitmaskStartFree, startingBit, endOfAlloc);

	// mark the end-of-allocation to false again
	SetBitTo0(bitmaskStartAllocEnd, endOfAlloc);

	// update the free bit count
	tail->mFreeBits += endOfAlloc - startingBit + 1;
	HyAssert(tail->mFreeBits <= kNumGranules, "if this exceeds the expected number of granules, we've got an accounting problem");

	// return the size that this allocation took up
	*allocSize = (endOfAlloc - startingBit + 1) * TGranularity;

	// now that we've added some memory back to the page, maybe the bigblock is larger now?
	UpdateBigBlock();

	return (tail->mFreeBits == kNumGranules);
}

//-------------------

template <uint32 TPageSize, uint32 TGranularity>
void HyMedHeapPage<TPageSize, TGranularity>::UpdateBigBlock(void)
{
	Tail *tail             = GetPageTail();
	void *bitmaskStartFree = GetStartFree();

	// note, as biggest block gets bigger, it becomes less important to check every last 
	// bit because beyond some point, no block CAN be bigger than what we already have.
	uint32 biggestBlock = 0;
	uint32 biggestBlockIndex = 0;
	for (uint32 bit=0; bit<kNumGranules - biggestBlock; )  
	{
		// scan forward to find the first free bit, then scan to find where the next allocated bit is
		uint32 const oneBitIndex = FindFirst1BitInRange(bitmaskStartFree, bit, kNumGranules-1);	
		if (oneBitIndex==kNumGranules)  // no more free space
		{
			break;
		}

		// found an allocated region, so let's scan for the end of it
		uint32 const zeroBitIndex = FindFirst0BitInRange(bitmaskStartFree, oneBitIndex+1, kNumGranules-1);
		uint32 const currentBlockLength = zeroBitIndex - oneBitIndex;
		if (currentBlockLength > biggestBlock)
		{
			biggestBlock      = currentBlockLength;
			biggestBlockIndex = oneBitIndex;
		}

		bit = zeroBitIndex + 1;
	}

	tail->mBigBlockIndex = biggestBlockIndex;  // update our records
	tail->mBigBlockSize  = biggestBlock;
}

//-------------------

template <uint32 TPageSize, uint32 TGranularity>
void HyMedHeapPage<TPageSize, TGranularity>::RemoveFromList(HyMedHeapPage **headPtr)
{
	// very simply, insert this ahead of the head page.
	Tail *tail = GetPageTail();
	if (*headPtr == this)
	{
		*headPtr = tail->mNext;  // fixup the head of the list, in case it was us
	}	
	if (tail->mPrev)
	{
		Tail *prevTail = tail->mPrev->GetPageTail();
		prevTail->mNext = tail->mNext;
	}
	if (tail->mNext)
	{
		Tail *nextTail = tail->mNext->GetPageTail();
		nextTail->mPrev = tail->mPrev;	
	}
	tail->mNext = NULL;
	tail->mPrev = NULL;
}

//-------------------

template <uint32 TPageSize, uint32 TGranularity>
void HyMedHeapPage<TPageSize, TGranularity>::InsertNewHeadPage(HyMedHeapPage **headPtr)
{
	// insert this ahead of the head page.
	Tail *tail = GetPageTail();
	HyAssert(tail->mNext == NULL && tail->mPrev == NULL, "should only be doing this on a page not in a list already");
	if (*headPtr)
	{
		Tail *nextTail = (*headPtr)->GetPageTail();
		nextTail->mPrev = this;
	}
	tail->mNext = *headPtr;
	*headPtr = this;
}

//-------------------

template <uint32 TPageSize, uint32 TGranularity>
void HyMedHeapPage<TPageSize, TGranularity>::SanityCheck(void)
{
	HyMedHeapPage *currentPage = this;
	while (currentPage)
	{
		Tail *tail = currentPage->GetPageTail();	
		HyAssert(tail->mPrev==NULL || tail->mPrev->GetPageTail()->mNext==currentPage, "Previous page linked-list isn't pointing to current page.");
		HyAssert(tail->mNext==NULL || tail->mNext->GetPageTail()->mPrev==currentPage, "Next page linked-list isn't pointing to current page.");
		HyAssert(tail->mFreeBits >= tail->mBigBlockSize, "The free bit count should always be at least as high or higher than the big block size");

		// we're going to walk ALL the free bits and see if the count matches the tail's count.
		void *bitmaskStartFree = GetStartFree();
		uint32 bitsSet = 0;
		for (uint32 i=0; i<kNumGranules; i++)
		{
			if (FindFirst1BitInRange(bitmaskStartFree, i, i)==i)
				bitsSet++;
		}
		HyAssert(bitsSet==tail->mFreeBits, "bitsSet is beyound mFreeBits");

		// now, if we know how many bits are FREE, we can also tell what the upper limit of end-of-allocations there should be (numGranules - freeBits).
		void *bitmaskStartAllocEnd = GetStartAllocEnd();
		uint32 allocBits = 0;
		for (uint32 i=0; i<kNumGranules; i++)
		{
			if (FindFirst1BitInRange(bitmaskStartAllocEnd, i, i)==i)
			{
				allocBits++;
			}
		}
		HyAssert(allocBits <=kNumGranules - tail->mFreeBits, "");
		currentPage = tail->mNext;  // iterate to the next page in the list and check it
	}
}

//-------------------

#endif /* __HyHeapPageMedium_h__ */
