/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.270 12-18-03 (barracuda-o04)" */
/*
 *  ======== sio.h ========
 *  Stream I/O Manager
 *
 */

#ifndef SIO_
#define SIO_

#include <dev.h>
#include <que.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIO_INPUT	DEV_INPUT		/* input stream */
#define SIO_OUTPUT	DEV_OUTPUT		/* output stream */

#define SIO_STANDARD		0
#define SIO_ISSUERECLAIM	1

typedef struct SIO_Attrs {	/* stream attributes */
    Int		nbufs;		/* number of buffers */
    Int		segid;		/* buffer segment */
    Int		align;		/* buffer alignment */
    Bool	flush;		/* TRUE -> don't block in DEV_idle() */
    Uns		model;		/* SIO_STANDARD, SIO_ISSUERECLAIM */
    Uns		timeout;	/* passed to all calls to DEV_reclaim() */
    DEV_Callback *callback;	/* initializes callback in DEV_Obj */
} SIO_Attrs;

typedef struct SIO_Obj {	/* stream object */
    DEV_Obj	dobj;		/* device object */
    Bool	flush;		/* flush */
    QUE_Obj	framelist;	/* start of frame list */
    DEV_Frame	*standardFrame;	/* frame for use in SIO_STANDARD model only */
    Uns		model;		/* SIO_STANDARD or SIO_ISSUERECLAIM */
    DEV_Fxns	*pfxns;		/* pointer to DEV_Fxns for static stream init */
} SIO_Obj, *SIO_Handle;

extern SIO_Attrs SIO_ATTRS;	/* default attributes */

extern Int _SIO_idle(SIO_Handle stream, Bool flush);
#define SIO_idle(stream)		_SIO_idle(stream, 0)
#define SIO_flush(stream)		_SIO_idle(stream, 1)

#define SIO_ctrl(stream, cmd, arg)	DEV_ctrl((DEV_Handle)stream, cmd, arg)
#define SIO_ready(stream)		DEV_ready((DEV_Handle)stream, NULL)

#define SIO_segid(stream)		((stream)->dobj.segid)
#define SIO_bufsize(stream)		((stream)->dobj.bufsize)
#define SIO_nbufs(stream)		((stream)->dobj.nbufs)

extern SIO_Handle SIO_create(String name, Int mode, Uns size, SIO_Attrs *attrs);
extern Int SIO_delete(SIO_Handle stream);
extern Int SIO_get(SIO_Handle stream, Ptr *buf);
extern Int SIO_staticbuf(SIO_Handle stream, Ptr *buf);
extern Int SIO_put(SIO_Handle stream, Ptr *buf, Uns nbytes);
extern Uns SIO_select(SIO_Handle streamtab[], Int n, Uns timeout);
extern Int SIO_issue(SIO_Handle stream, Ptr pbuf, Uns nbytes, Arg arg);
extern Int SIO_reclaim(SIO_Handle stream, Ptr *ppbuf, Arg *parg);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* SIO_ */
