//*****************************************************************************
// Description:  Library global macros
//*****************************************************************************

#ifndef TREADINGFX_BASEMACRO_H
#define TREADINGFX_BASEMACRO_H

//*****************************************************************************
//** common definitions
//*****************************************************************************

// wide character text
#define __WT(x)   L ## x
#define _WT(x)    __WT(x)
#define _WTEXT(x) __WT(x)

// ansi character text
#define __AT(x)   x
#define _AT(x)    __AT(x)
#define _ATEXT(x) __AT(x)

// unicode definitions
#define __WFILE__       _WT(__FILE__)
#define __WTIME__       _WT(__TIME__)
#define __WTIMESTAMP__  _WT(__TIMESTAMP__)

#if defined(UNICODE)
#define TFILE__ __WFILE__
#else
#define TFILE__ __FILE__
#endif

// asser expression macro for debug time verification of parameters/results
#define ASSERT_EXPR(expr) ASSERT_SYSTEM(expr)
#define ASSERT_FAILED(errmsg) ASSERT_EXPR(false)

#endif // #ifndef TREADINGFX_BASEMACRO_H
