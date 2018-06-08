/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.270 12-18-03 (barracuda-o04)" */
/*
 *  ======== log.h ========
 *
 */

#ifndef LOG_
#define LOG_

#include <_log.h>
#include <trc.h>


#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== LOG_Obj structure ========
 */
typedef struct LOG_Event {
#if (defined(_55_) && defined(__LARGE_MODEL__)) 
    Uns pad;
#endif
#if defined(_28_)
    Long seqnum;
#else
    Uns seqnum;
#endif
    Arg val1;
    Arg val2;
    Arg val3;
}  LOG_Event;

typedef struct LOG_Obj {
    Arg* bufend;
    Uns flag;
    Uns seqnum;
    LOG_Event* curptr;
    Uns lenmask;
    LOG_Event* bufbeg;
} LOG_Obj;

typedef struct LOG_Obj *LOG_Handle; 

extern LOG_Handle LOG_D_system;  /* distinguished system log */


/*
 *  ======== LOG_disable ========
 *  set the flag field of the LOG object to 1 to disable the LOG
 */
#define LOG_disable(log) ((log)->flag = 1)

/*
 *  ======== LOG_enable ========
 *  set the flag field of the LOG object to 0 to enable the LOG
 */
#define LOG_enable(log) ((log)->flag = 0)

/*
 *  ======== LOG_error ========
 *  Write a "user" error event into the system log.  This operation is
 *  not affected by any TRC trace bits; an error event is *always* written
 *  to the system log.
 */
#define LOG_error(format, arg0) \
    LOG_event(LOG_D_system, (Arg)(arg0), (Arg)(format), (Arg)_LOG_USRERR) 

/*
 *  ======== LOG_event ========
 *  Write an uninterpreted event into the specified log.
 */
extern Void LOG_event(LOG_Handle log, Arg arg0, Arg arg1, Arg arg2);

/*
 *  ======== LOG_message ========
 *  Write a "user" message event into the system log provided that both
 *  TRC_GBLHOST and TRC_GBLTARG trace is enabled.
 */
#define LOG_message(format, arg0)                \
     if (!TRC_query(TRC_GBLHOST | TRC_GBLTARG))  \
	LOG_event(LOG_D_system,(Arg)(arg0),(Arg)(format),(Arg)_LOG_USRMSG) 

/*
 *  ======== LOG_printf ========
 *  Write an "printf" event into the specified log.
 */
extern Void LOG_printf(LOG_Handle log, String format, ...);

/*
 *  ======== LOG_reset ========
 *  reset the LOG by doing the following:
 *  log->flag = 0;
 *  log->seqnum = 0;
 *  log->curptr = log->bufbeg;
 *  set each word in the log buf to 0xffffffff;
 */
extern Void LOG_reset(LOG_Handle log);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif	/* LOG_ */




