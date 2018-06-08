/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.270 12-18-03 (barracuda-o04)" */
/*
 *  ======== mem.h ========
 *
 */

#ifndef MEM_
#define MEM_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _62_
extern far Fxn MEM_D_lockaddr;
extern far Fxn MEM_D_unlockaddr;
#else
extern Fxn MEM_D_lockaddr;
extern Fxn MEM_D_unlockaddr;
#endif

#if (defined(_55l_) || defined(_28_))	/* If 55x or 28x processor */
typedef  unsigned long  MEM_sizep;
#else
typedef  unsigned int   MEM_sizep;
#endif

#ifdef _54_
extern void MEM_register_lock(Fxn fxn);
extern void MEM_register_unlock(Fxn fxn);
#else
#define MEM_register_lock(fxn)          MEM_D_lockaddr = (fxn)
#define MEM_register_unlock(fxn)        MEM_D_unlockaddr = (fxn)
#endif

#define	MEM_ILLEGAL ((Ptr)NULL)	/* illegal memory address */

typedef struct MEM_Attrs {
    Int		space;		/* x, y, or p memory */
} MEM_Attrs;

typedef struct MEM_Config {
    Int     MALLOCSEG;		/* segid used by malloc() and free() */
} MEM_Config;

typedef struct MEM_Segment {
    Ptr		base;		/* Base of the segment */
    MEM_sizep	length;		/* Size of the segment */
    Int		space;		/* x, y or p memory */
} MEM_Segment;

typedef	struct MEM_Stat {
    Uns	    size;		/* original size of segment */
    Uns	    used;		/* number of bytes used in segment */
    Uns	    length;		/* lengthof largest contiguous block */
    Int	    space;		/* Indicate x, y or p memory */
} MEM_Stat;

/*
 *  ======== MEM_Header ========
 *  This header is used to maintain a list of free memory blocks.
 *  The memory allocation/free routines REQUIRE that the size of this
 *  header is a power of two; this simplifies several computations in
 *  these routines by eliminating the need for the C % operator.
 *  
 */
typedef struct MEM_Header {
    struct MEM_Header	*next;	/* form a free memory link list */
    MEM_sizep		size;	/* size of the free memory */
} MEM_Header;

#define MEM_headersize(space)	sizeof(MEM_Header)

#define MEM_headermask(space)	(MEM_headersize(space) - 1)

extern MEM_Config	*MEM;
extern MEM_Segment	MEM_memtab[];

#define MEM_calloc(segid, size, align)	MEM_valloc(segid, size, align, 0)

/* The default space must be defined to be 1 for all targets. */

#define MEM_32BIT	1
#define MEM_16BIT	2
#define MEM_8BIT	3

extern Void	MEM_init(void);
extern Ptr	MEM_alloc(Int segid, Uns size, Uns align);
extern Int	MEM_define(Ptr base, Uns length, MEM_Attrs *attrs);
extern Void	MEM_redefine(Int segid, Ptr base, Uns length);
extern Bool	MEM_free(Int segid, Ptr addr, Uns size);
extern Void 	MEM_print(Int segid);
extern Bool	MEM_stat(Int segid, MEM_Stat *statbuf);
extern Ptr	MEM_valloc(Int segid, Uns size, Uns align, Char val);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* MEM_ */
