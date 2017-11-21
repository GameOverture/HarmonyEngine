/**************************************************************************
 *	HyBitManip.h
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/
#ifndef HyBitManip_h__
#define HyBitManip_h__

#include "Afx/HyStdAfx.h"

#include <stddef.h>
#include <memory.h>

// FindSetBitInRange will return the first bit in the specified range that is on, or will return bitEnd+1.
uint32 FindFirst1BitInRange(void const *root, uint32 bitStart, uint32 bitEnd);
uint32 FindFirst0BitInRange(void const *root, uint32 bitStart, uint32 bitEnd);

// These functions are specially optimized to work in 32-bits at all times, with good memory alignment performance.
// NOT endian-safe at the moment.  However, if you only use these functions to access bits rather than calculating
// them directly, it's not important.
void SetBitRangeTo1(void *root, uint32 bitStart, uint32 bitEnd);
void SetBitRangeTo0(void *root, uint32 bitStart, uint32 bitEnd);

// These always work in 32-bit aligned memory, specifically so we don't have to worry about endianness when porting.
// If all memory is 32-bit aligned, and you access it as 32-bits always, the byte order doesn't really matter.
// It does matter if you treat the memory as bytes sometimes and 32-bit other times.
inline void SetBitTo1(void *root, uint32 bitStart)
{
	// we require root to be 4-byte address aligned, for memory performance reasons.
	// TODO: assert(((tUINT32)root & 3)==0);

	// Now, we operate entirely in 32-bit space. (which means Endian-ness matters)
	unsigned      *currentWord = (unsigned*)root + bitStart/32;
	*currentWord |= 1<<(bitStart & 31);
}

inline void SetBitTo0(void *root, uint32 bitStart)
{
	// we require root to be 4-byte address aligned, for memory performance reasons.
	// TODO: assert(((tUINT32)root & 3)==0);

	// Now, we operate entirely in 32-bit space. (which means Endian-ness matters)
	unsigned      *currentWord = (unsigned*)root + bitStart/32;
	*currentWord &= ~(1<<(bitStart & 31));
}

#endif /* HyBitManip_h__ */
