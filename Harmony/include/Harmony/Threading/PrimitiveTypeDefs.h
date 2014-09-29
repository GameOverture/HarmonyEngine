//*****************************************************************************
// Description:  primitive library type definitions
//*****************************************************************************

#ifndef TREADINGFX_PRIMITIVETYPEDEFS_H
#define TREADINGFX_PRIMITIVETYPEDEFS_H



//*****************************************************************************
//** data type definitions
//*****************************************************************************

// define to have the C99 limit macros available when compiling C++ code
#ifndef __STDC_LIMIT_MACROS
	#define __STDC_LIMIT_MACROS
#endif

#include <stdint.h>
#endif



// uint8 type
typedef uint8 uint8;


// short named unsigned types
typedef uint8 uchar;
typedef uint16 ushort;
typedef uint32 uint;
typedef uint64 ulonglong;


// 32/64 bit platform dependent types
#if defined(API_32BIT)
	typedef int32 intx;
	typedef uint32 uintx;
	#define sizeof_intx (4)
	#define digits_intx (8)
#else
	typedef int64 intx;
	typedef uint64 uintx;
	#define sizeof_intx (8)
	#define digits_intx (16)
#endif


// special index type, used to convert 64 bit indices to access managed arrays without compiler warning
#if defined(PLATFORM_NATIVE)
	typedef intx size_idx;
#else
	typedef int size_idx;
#endif


#endif // #ifndef TREADINGFX_PRIMITIVETYPEDEFS_H
