// //////////////////////////////////////////////////////////
// Crc32.h
// Copyright (c) 2011-2015 Stephan Brumme. All rights reserved.
// Slicing-by-16 contributed by Bulat Ziganshin
// see http://create.stephan-brumme.com/disclaimer.html
//

#ifndef __Crc32_h__
#define __Crc32_h__

/// compute CRC32 (bitwise algorithm)
uint32_t crc32_bitwise(const void* data, size_t length, uint32_t previousCrc32 = 0);

/// compute CRC32 (half-byte algoritm)
uint32_t crc32_halfbyte(const void* data, size_t length, uint32_t previousCrc32 = 0);

/// compute CRC32 (standard algorithm)
uint32_t crc32_1byte(const void* data, size_t length, uint32_t previousCrc32 = 0);

/// compute CRC32 (Slicing-by-4 algorithm)
uint32_t crc32_4bytes(const void* data, size_t length, uint32_t previousCrc32 = 0);

/// compute CRC32 (Slicing-by-8 algorithm)
uint32_t crc32_8bytes(const void* data, size_t length, uint32_t previousCrc32 = 0);

/// compute CRC32 (Slicing-by-8 algorithm), unroll inner loop 4 times
uint32_t crc32_4x8bytes(const void* data, size_t length, uint32_t previousCrc32 = 0);

/// compute CRC32 (Slicing-by-16 algorithm)
uint32_t crc32_16bytes(const void* data, size_t length, uint32_t previousCrc32 = 0);

/// compute CRC32 (Slicing-by-16 algorithm, prefetch upcoming data blocks)
uint32_t crc32_16bytes_prefetch(const void* data, size_t length, uint32_t previousCrc32 = 0, size_t prefetchAhead = 256);

/// compute CRC32 using the fastest algorithm for large datasets on modern CPUs
uint32_t crc32_fast(const void* data, size_t length, uint32_t previousCrc32 = 0);

#endif
