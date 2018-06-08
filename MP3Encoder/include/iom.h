/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.270 12-18-03 (barracuda-o04)" */
/*
 *  ======== iom.h ========
 *
 *  Input/Output Module's public header file.
 *
 *
 */

#ifndef IOM_
#define IOM_

#include <dev.h>
#include <que.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Create modes.  IOM_create() and mini-driver create functions take
 *  a mode parameter.
 */
#define IOM_INPUT    0x0001
#define IOM_OUTPUT   0x0002
#define IOM_INOUT    (IOM_INPUT | IOM_OUTPUT)

/*
 *  IOM Status Codes.
 */
#define  IOM_COMPLETED     0       /* I/O completed successfully */
#define  IOM_PENDING       1       /* I/O queued and pending */

/* 
 * I/O request flushed. Queued writes will be completed w/ IOM_COMPLETED.
 *   Queued read requests return w/ IOM_FLUSHED.
 */
#define  IOM_FLUSHED      2 

/*
 * I/O aborted. Non-completed read or write requests return w/ IOM_ABORTED.
 */
#define  IOM_ABORTED      3      


/*
 * IOM Error Codes
 */
#define  IOM_EBADIO        -1      /* Generic failure condition */
#define  IOM_ETIMEOUT      -2      /* Timeout occurred */
#define  IOM_ENOPACKETS    -3      /* No packets available for I/O */
#define  IOM_EFREE         -4      /* Unable to free resources */
#define  IOM_EALLOC        -5      /* Unable to alloc resource */
#define  IOM_EABORT        -6      /* I/O was aborted before completed */
#define  IOM_EBADMODE      -7      /* Illegal device mode */
#define  IOM_EOF           -8      /* End-of-File was encountered */
#define  IOM_ENOTIMPL      -9      /* Operation not implemented or supported */
#define  IOM_EBADARGS      -10     /* Illegal arguments specified */
#define  IOM_ETIMEOUTUNREC -11     /* Unrecoverable timeout occurred */
#define  IOM_EINUSE        -12     /* Device already in use */

/*
 *  IOM_Packet structures are managed by the IOM module.  IOM packets are the
 *  basis for all I/O operations. 'cmd' field contains the command id for the
 *  mini-driver.  'status' is filled in by the mini-driver and contains the
 *  status of the commmand.
 */
typedef DEV_Frame IOM_Packet;

/*
 *  This is the mini-driver's callback function.  The mini-driver will call
 *  a function of this type whenever an I/O operation completes.
 */
typedef Void	(*IOM_TiomCallback)(Ptr arg, IOM_Packet *packet);

/*
 *  The following function prototypes define the mini-driver functions.
 */
typedef Int  (*IOM_TmdBindDev)(Ptr *devp, Int devid, Ptr devParams);
typedef Int  (*IOM_TmdUnBindDev)(Ptr devp);
typedef Int  (*IOM_TmdControlChan)(Ptr chanp, Uns cmd, Ptr args);
typedef Int  (*IOM_TmdCreateChan)(Ptr *chanp, Ptr devp, String name, Int mode,
                      Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg);
typedef Int  (*IOM_TmdDeleteChan)(Ptr chanp);
typedef Int  (*IOM_TmdSubmitChan)(Ptr chanp, IOM_Packet *packet);


/*
 *  Function table structure used for actual linkage between the
 *  I/O module and each mini-driver.
 */
typedef struct IOM_Fxns
{
    IOM_TmdBindDev       mdBindDev;
    IOM_TmdUnBindDev     mdUnBindDev;
    IOM_TmdControlChan   mdControlChan;
    IOM_TmdCreateChan    mdCreateChan;
    IOM_TmdDeleteChan    mdDeleteChan;
    IOM_TmdSubmitChan    mdSubmitChan;

} IOM_Fxns;

/*
 * Use this mini-driver stub fxn definition if a fxn is not implemented.
 * This fxn always returns status of IOM_ENOTIMPL.
 */
extern Int IOM_mdNotImpl(Void);

#define IOM_BINDDEVNOTIMPL     (IOM_TmdBindDev)IOM_mdNotImpl
#define IOM_UNBINDDEVNOTIMPL   (IOM_TmdUnBindDev)IOM_mdNotImpl
#define IOM_CONTROLCHANNOTIMPL (IOM_TmdControlChan)IOM_mdNotImpl
#define IOM_CREATECHANNOTIMPL  (IOM_TmdCreateChan)IOM_mdNotImpl
#define IOM_DELETECHANNOTIMPL  (IOM_TmdDeleteChan)IOM_mdNotImpl
#define IOM_SUBMITCHANNOTIMPL  (IOM_TmdSubmitChan)IOM_mdNotImpl


/*
 *  -------- command codes for IOM_Packet --------
 */
#define IOM_READ	0
#define IOM_WRITE	1
#define IOM_ABORT	2
#define IOM_FLUSH	3

#define IOM_USER	128  /* 0-127 are reserved for system */

/*
 *   -------- Command codes reserved for control --------
 */
#define IOM_CHAN_RESET		0 /* reset channel only */
#define IOM_CHAN_TIMEDOUT	1 /* channel timeout occured */
#define IOM_DEVICE_RESET	2 /* reset entire device */

#define IOM_CNTL_USER   128  /* 0-127 are reserved for system */


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* IOM_ */
