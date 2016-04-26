// //////////////////////////////////////////////////////////
// Crc32.h
// Copyright (c) 2011-2015 Stephan Brumme. All rights reserved.
// Slicing-by-16 contributed by Bulat Ziganshin
// see http://create.stephan-brumme.com/disclaimer.html
//

#ifndef __Crc32_h__
#define __Crc32_h__

// g++ -o Crc32 Crc32.cpp -O3 -lrt -march=native -mtune=native

// if running on an embedded system, you might consider shrinking the
// big Crc32Lookup table:
// - crc32_bitwise doesn't need it at all
// - crc32_halfbyte has its own small lookup table
// - crc32_1byte    needs only Crc32Lookup[0]
// - crc32_4bytes   needs only Crc32Lookup[0..3]
// - crc32_8bytes   needs only Crc32Lookup[0..7]
// - crc32_4x8bytes needs only Crc32Lookup[0..7]
// - crc32_16bytes  needs all of Crc32Lookup


#include <stdlib.h>

// define endianess and some integer data types
#if defined(_MSC_VER) || defined(__MINGW32__)
typedef unsigned __int8  uint8_t;
typedef unsigned __int32 uint32_t;
typedef   signed __int32  int32_t;

#define __LITTLE_ENDIAN 1234
#define __BIG_ENDIAN    4321
#define __BYTE_ORDER    __LITTLE_ENDIAN

#include <xmmintrin.h>
#ifdef __MINGW32__
#define PREFETCH(location) __builtin_prefetch(location)
#else
#define PREFETCH(location) _mm_prefetch(location, _MM_HINT_T0)
#endif
#else
// uint8_t, uint32_t, in32_t
#include <stdint.h>
// defines __BYTE_ORDER as __LITTLE_ENDIAN or __BIG_ENDIAN
#include <sys/param.h>

#ifdef __GNUC__
#define PREFETCH(location) __builtin_prefetch(location)
#else
#define PREFETCH(location) ;
#endif
#endif

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
