/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.270 12-18-03 (barracuda-o04)" */
/*
 *  ======== std.h ========
 *
 */

#ifndef STD_
#define STD_

#ifdef _TMS320C28X
#define _28_ 1
#ifdef LARGE_MODEL
#define _28l_ 1
#endif
#endif
#ifdef _TMS320C2XX
#define _29_ 1
#endif
#ifdef _TMS320C30
#define _30_ 1
#endif
#ifdef _TMS320C40
#define _40_ 1
#endif
#ifdef _TMS320C50
#define _50_ 1
#endif
#ifdef _TMS320C5XX
#define _54_ 1
#endif
#ifdef __TMS320C55X__ 
#define _55_ 1
#ifdef __LARGE_MODEL__
#define _55l_ 1
#endif
#endif
#ifdef _TMS320C6200
#define _62_ 1
#define _6x_ 1
#endif
#ifdef _TMS320C6400
#define _64_ 1
#define _6x_ 1
#endif
#ifdef _TMS320C6700
#define _67_ 1
#define _6x_ 1
#endif
#ifdef M_I86
#define _86_ 1
#endif
#ifdef _MVP_MP
#define _80_ 1
#endif
#ifdef _MVP_PP
#define _80_ 1
#endif
#ifdef _WIN32
#define _W32_ 1
#endif

/*
 *  ======== _TI_ ========
 *  _TI_ is defined for all TI targets
 */
#if defined(_29_) || defined(_30_) || defined(_40_) || defined(_50_) || defined(_54_) || defined(_55_) || defined (_6x_) || defined(_80_) || defined (_28_) || defined(_24_)
#define _TI_	1
#endif

/*
 *  ======== _FLOAT_ ========
 *  _FLOAT_ is defined for all targets that natively support floating point
 */
#if defined(_SUN_) || defined(_30_) || defined(_40_) || defined(_67_) || defined(_80_)
#define _FLOAT_	1
#endif

/*
 *  ======== _FIXED_ ========
 *  _FIXED_ is defined for all fixed point target architectures
 */
#if defined(_29_) || defined(_50_) || defined(_54_) || defined(_55_) || defined (_62_) || defined(_64_) || defined (_28_)
#define _FIXED_	1
#endif

/*
 *  ======== _TARGET_ ========
 *  _TARGET_ is defined for all target architectures (as opposed to
 *  host-side software)
 */
#if defined(_FIXED_) || defined(_FLOAT_)
#define _TARGET_ 1
#endif

/*
 *  8, 16, 32-bit type definitions
 *
 *  Sm*	- 8-bit type
 *  Md* - 16-bit type
 *  Lg* - 32-bit type
 *
 *  *Int - signed type
 *  *Uns - unsigned type
 *  *Bits - unsigned type (bit-maps)
 */
typedef char SmInt;		/* SMSIZE-bit signed integer */
typedef short MdInt;		/* MDSIZE-bit signed integer */
#if defined(_6x_)
typedef int LgInt;		/* LGSIZE-bit signed integer */
#else
typedef long LgInt;		/* LGSIZE-bit signed integer */
#endif

typedef unsigned char SmUns;	/* SMSIZE-bit unsigned integer */
typedef unsigned short MdUns;	/* MDSIZE-bit unsigned integer */
#if defined(_6x_)
typedef unsigned LgUns;		/* LGSIZE-bit unsigned integer */
#else
typedef unsigned long LgUns;	/* LGSIZE-bit unsigned integer */
#endif

typedef unsigned char SmBits;	/* SMSIZE-bit bit string */
typedef unsigned short MdBits;	/* MDSIZE-bit bit string */
#if defined(_6x_)
typedef unsigned LgBits;	/* LGSIZE-bit bit string */
#else
typedef unsigned long LgBits;	/* LGSIZE-bit bit string */
#endif

/*
 *  Aliases for standard C types
 */
typedef int Int;		/* for those rare occasions */
typedef long int Long;
typedef short int Short;
typedef char Char;
#define Void void

typedef char * String;		/* pointer to null-terminated character
				 * sequence
				 */

#if defined(_28_) || defined(_29_) || defined(_50_) || defined(_54_) || defined(_55_) || defined(_6x_)
typedef unsigned Uns;
#else
typedef unsigned long Uns;
#endif

#if defined(_80_)
typedef int Bool;		/* boolean */
#elif defined(_W32_)
typedef long Bool;		/* boolean to match Windows boolean def */
#else
typedef MdUns Bool;		/* boolean */
#endif

typedef SmBits Byte;		/* smallest unit of addressable store */
typedef void *Ptr;		/* pointer to arbitrary type */

/* Arg should be size of Ptr */
#if defined(M_I86SM) || defined(_29_) || defined(_50_) || defined(_54_) || defined(_6x_)
typedef Int Arg;
#elif defined(_55_) || defined(_28_)
typedef void *Arg;
#else
typedef LgInt Arg;		/* uninterpreted LGSIZE-bit word */
#endif

typedef Int (*Fxn)();		/* generic function type */

#if defined(_80_) || defined(_SUN_) || defined(_67_)
typedef float Float;
#else
typedef double Float;
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define FALSE ((Bool)0)
#define TRUE  ((Bool)1)
#endif

/*
 * These macros are used to cast 'Arg' types to 'Int' or 'Ptr'.
 * These macros were added for the 55x since Arg is not the same
 * size as Int and Ptr in 55x large model.
 */
#if defined(_28l_) || defined(_55l_)
#define ArgToInt(A)	((Int)((long)(A) & 0xffff))
#define ArgToPtr(A)	((Ptr)(A))
#else
#define ArgToInt(A)	((Int)(A))
#define ArgToPtr(A)	((Ptr)(A))
#endif

/*
 *  ======== __inline ========
 *  The following definitions define the macro __inline for those
 *  C compilers that do not use __inline to indicate inline
 *  expansion of functions.
 *
 *  The TI C compilers support the "inline" keyword (ala C++).  Both 
 *  Microsoft and GNU C compilers support the "__inline" keyword.  The
 *  native SUN OS 4.x C compiler doesn't understand either.
 */
#ifdef _TI_
#ifdef _LINT_
#define __inline
#else
#define __inline inline
#endif
#endif

#ifdef _SUN4_
#define __inline
#endif

/*
 *  ======== inline ========
 *  Define "inline" so that all C code can optionally use the "inline"
 *  keyword.
 */
#if !defined(inline) && !defined(__cplusplus) && !defined(_TI_)
#define inline	__inline
#endif

#endif /* STD_ */
