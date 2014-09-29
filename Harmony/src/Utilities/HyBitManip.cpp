/**************************************************************************
 *	HyBitManip.cpp
 *	
 *	Harmony Engine
 *	Copyright (c) 2012 Jason Knobler
 *
 *	The zlib License (zlib)
 *	https://github.com/OvertureGames/HarmonyEngine/blob/master/LICENSE
 *************************************************************************/

#include "Utilities/HyBitManip.h"

#if 0
//-------------------
// This assumes a bit will be set, so don't call it unless there is.
inline tUINT32 FindFirstSetBitInByte(tUINT32 v)
{
	assert(v<256);  // don't call this with more than an 8 bit value
	tUINT32 firstBitSet[16] = { 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 };
	
	// the 0th entry will never be used...
	if (v & 0x0f)  // lower nibble
	{
		return firstBitSet[v & 0x0f];
	}
	else  // upper nibble
	{
		return firstBitSet[v >> 4] + 4;
	}
}

// This assumes a bit will be set, so don't call it unless there is.

inline tUINT32 FindFirstSetBit(tUINT32 v)
{
	if (v & 0xffff)
	{
		if (v & 0xff)  // first byte
		{
			return FindFirstSetBitInByte(v & 0xff);
		}
		else  // second byte
		{
			return FindFirstSetBitInByte((v >> 8) & 0xff) + 8;
		}
	}
	else if (v & 0xff0000)  // third byte
	{
		return FindFirstSetBitInByte((v >> 16) & 0xff) + 16;	
	}
	else  // fourth byte
	{
		return FindFirstSetBitInByte((v >> 24) & 0xff) + 24;	
	}
}
#else
//-------------------
// This returns an index to an 'on' bit in data.  Presumably the lowest on bit.
inline uint32 FindFirstSetBit(uint32 data)
{
	uint32 bitIndex = 0xffffffff;
	
#if defined(_M_IX86)
	// use a little assembly to get the index of the first one bit trivially
	_asm
	{
		mov eax, data
		bsf ecx, eax
		jz  nobits
		mov bitIndex, ecx
nobits:
	}
#else
	// this does a Log(N) search through 32 bits for the first 'on' bit we can find
	if (data)
	{
		bitIndex = 0;
		uint32 mask = 0x0000ffff;
		for (uint32 shiftCount=16; shiftCount; )
		{
			if (data & mask)  // if this is true, leave the data where it is
			{
			}
			else  // the on bit is above the bottom half, so shift it down, and increase the bit index by that much
			{
				data     >>= shiftCount;
				bitIndex  += shiftCount;
			}

			// slide the mask down so it's half as big
			shiftCount >>= 1;
			mask       >>= shiftCount;
		}
	}
#endif
	return bitIndex;
}
#endif
//-------------------
// This scans looking for any set bits, but changes nothing.
uint32 FindFirst1BitInRange(void const *root, uint32 bitStart, uint32 bitEnd)
{
	// we require root to be 4-byte address aligned, for memory performance reasons.
	// TODO: assert(((tUINT32)root & 3)==0);

	// Now, we operate entirely in 32-bit space. (which means Endian-ness matters)
	unsigned      *currentWord = (unsigned*)root + bitStart/32;

	uint32 bitsToSet = bitEnd - bitStart + 1;	
	uint32 bitsLeftInWord = (32 - (bitStart & 31));

	// deal with special case where start AND end are in this first word.
	// Otherwise, the mask extends to the end of the first word, possibly over many others, and terminates partway along the last word
	if (bitsToSet < bitsLeftInWord)
	{
		uint32 value = *currentWord;
		value >>= bitStart & 31;  // shift the bits we care about down until they are the lowest order bits
		uint32 mask = ((1 << bitsToSet)-1);  // these are in the low order
		uint32 overlap = value & mask;
		if (overlap)
		{
			return FindFirstSetBit(overlap) + bitStart;
		}
		else  // didn't find the bit
		{
			return bitEnd + 1;
		}
	}
	else  // this mask extends to the end of the word
	{
		uint32 value = *currentWord;
		value >>= bitStart & 31;
		if (value)  // found our bit already
		{
			return FindFirstSetBit(value) + bitStart;
		}
		currentWord++;		
		bitsToSet -= bitsLeftInWord;		
		bitStart += bitsLeftInWord;
	}
	
	// now, as long as there are more than 32 bits to set, simply force-set the whole word w/o reading it from memory.  No need.
	while (bitsToSet >= 32)
	{
		uint32 value = *currentWord++;
		if (value)
		{
			return FindFirstSetBit(value) + bitStart;
		}
		bitsToSet -= 32;
		bitStart  += 32;
	}

	// finally, figure out how many bits we want to actually set in this final word and set only those
	if (bitsToSet>0)
	{
		uint32 value = *currentWord;
		value &= ((1 << bitsToSet)-1);  // keep all bits up to the count remaining
		if (value)
		{
			return FindFirstSetBit(value) + bitStart;
		}
	}
	return bitEnd + 1;
}

//-------------------

uint32 FindFirst0BitInRange(void const *root, uint32 bitStart, uint32 bitEnd)
{
	// we require root to be 4-byte address aligned, for memory performance reasons.
	// TODO: assert(((tUINT32)root & 3)==0);

	// Now, we operate entirely in 32-bit space. (which means Endian-ness matters)
	unsigned      *currentWord = (unsigned*)root + bitStart/32;

	uint32 bitsToSet = bitEnd - bitStart + 1;	
	uint32 bitsLeftInWord = (32 - (bitStart & 31));

	// deal with special case where start AND end are in this first word.
	// Otherwise, the mask extends to the end of the first word, possibly over many others, and terminates partway along the last word
	if (bitsToSet < bitsLeftInWord)
	{
		uint32 value = *currentWord;
		value >>= bitStart & 31;  // shift the bits we care about down until they are the lowest order bits
		uint32 mask = ((1 << bitsToSet)-1);  // these are in the low order
		uint32 overlap = (value & mask) ^ mask;  // flip all 0 bits to 1 bits in the range we care about
		if (overlap)
		{
			return FindFirstSetBit(overlap) + bitStart;
		}
		else  // didn't find the bit
		{
			return bitEnd + 1;
		}
	}
	else  // this mask extends to the end of the word
	{
		uint32 value = *currentWord;
		value >>= bitStart & 31;
		uint32 mask = 0xffffffff >> (bitStart & 31);  // lowest bits mask
		value = (value & mask) ^ mask;
		if (value)  // found our bit already
		{
			return FindFirstSetBit(value) + bitStart;
		}
		currentWord++;
		bitsToSet -= bitsLeftInWord;		
		bitStart += bitsLeftInWord;
	}
	
	// now, as long as there are more than 32 bits to set, simply force-set the whole word w/o reading it from memory.  No need.
	while (bitsToSet >= 32)
	{
		uint32 value = ~(*currentWord++);
		if (value)
		{
			return FindFirstSetBit(value) + bitStart;
		}
		bitsToSet -= 32;
		bitStart  += 32;
	}

	// finally, figure out how many bits we want to actually set in this final word and set only those
	if (bitsToSet>0)
	{
		uint32 value = *currentWord;
		uint32 mask = ((1 << bitsToSet)-1);
		value = (value & mask) ^ mask;  // flip 0 bits to 1 so we can check for them
		if (value)
		{
			return FindFirstSetBit(value) + bitStart;
		}
	}
	return bitEnd + 1;
}

//-------------------
// This is a little helper that can set a range of bits for us.
// Note: We CANNOT assume that root is 32-bit aligned.  We have 
// to handle that ourselves by adjusting bitStart and bitEnd to be 
// relative to the proper address.
void SetBitRangeTo1(void *root, uint32 bitStart, uint32 bitEnd)
{
	// we require root to be 4-byte address aligned, for memory performance reasons.
	// TODO: assert(((tUINT32)root & 3)==0);

	// Now, we operate entirely in 32-bit space. (which means Endian-ness matters)
	unsigned      *currentWord = (unsigned*)root + bitStart/32;

	uint32 bitsToSet = bitEnd - bitStart + 1;	
	uint32 bitsLeftInWord = (32 - (bitStart & 31));

	// deal with special case where start AND end are in this first word.
	// Otherwise, the mask extends to the end of the first word, possibly over many others, and terminates partway along the last word
	if (bitsToSet < bitsLeftInWord)
	{
		uint32 value = *currentWord;
		value |= ((1 << bitsToSet)-1) << (bitStart & 31);  // adjust the mask upward
		*currentWord = value;
		return;
	}
	else  // this mask extends to the end of the word
	{
		uint32 value = *currentWord;
		value |= 0xffffffff << (bitStart & 31);  // set all bits at or above the start
		*currentWord++ = value;
		bitsToSet -= bitsLeftInWord;		
	}
	
	// now, as long as there are more than 32 bits to set, simply force-set the whole word w/o reading it from memory.  No need.
	while (bitsToSet >= 32)
	{
		*currentWord++ = 0xffffffff;
		bitsToSet -= 32;
	}

	// finally, figure out how many bits we want to actually set in this final word and set only those
	if (bitsToSet>0)
	{
		uint32 value = *currentWord;
		value |= ((1 << bitsToSet)-1);  // set all bits up to the count remaining
		*currentWord = value;
	}
}

//-------------------

void SetBitRangeTo0(void *root, uint32 bitStart, uint32 bitEnd)
{
	// we require root to be 4-byte address aligned, for memory performance reasons.
	// TODO: assert(((tUINT32)root & 3)==0);

	// Now, we operate entirely in 32-bit space. (which means Endian-ness matters)
	unsigned      *currentWord = (unsigned*)root + bitStart/32;

	uint32 bitsToSet = bitEnd - bitStart + 1;	
	uint32 bitsLeftInWord = (32 - (bitStart & 31));

	// deal with special case where start AND end are in this first word.
	// Otherwise, the mask extends to the end of the first word, possibly over many others, and terminates partway along the last word
	if (bitsToSet < bitsLeftInWord)
	{
		uint32 value = *currentWord;
		value &= ~(((1 << bitsToSet)-1) << (bitStart & 31));  // adjust the mask upward
		*currentWord = value;
		return;
	}
	else  // this mask extends to the end of the word
	{
		uint32 value = *currentWord;
		value &= ~(0xffffffff << (bitStart & 31));  // clear all bits at or above the start
		*currentWord++ = value;
		bitsToSet -= bitsLeftInWord;
	}
	
	// now, as long as there are more than 32 bits to set, simply force-set the whole word w/o reading it from memory.  No need.
	while (bitsToSet >= 32)
	{
		*currentWord++ = 0;
		bitsToSet -= 32;
	}

	// finally, figure out how many bits we want to actually set in this final word and set only those
	if (bitsToSet>0)
	{
		uint32 value = *currentWord;
		value &= ~((1 << bitsToSet)-1);  // clear all bits up to the count remaining
		*currentWord = value;
	}
}

//-------------------
