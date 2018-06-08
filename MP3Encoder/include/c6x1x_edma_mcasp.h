/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.11.00.00 11-04-03 (ddk-b13)" */
/* 
 *  ======== c6x1x_edma_mcasp.h ========
 */

#ifndef C6X1X_EDMA_MCASP_
#define C6X1X_EDMA_MCASP_

#include <iom.h>

#include <csl.h>
#include <csl_edma.h>
#include <csl_mcasp.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Version number definition */
#define C6X1X_EDMA_MCASP_VERSION_1   0xAB01

/* McASP event bit definition */
#define C6X1X_EDMA_MCASP_EVT_RBASE      0x00000001
#define C6X1X_EDMA_MCASP_EVT_ROVRN      (C6X1X_EDMA_MCASP_EVT_RBASE << 0)
#define C6X1X_EDMA_MCASP_EVT_RSYNCERR   (C6X1X_EDMA_MCASP_EVT_RBASE << 1)
#define C6X1X_EDMA_MCASP_EVT_RCKFAIL    (C6X1X_EDMA_MCASP_EVT_RBASE << 2)
#define C6X1X_EDMA_MCASP_EVT_RDMAERR    (C6X1X_EDMA_MCASP_EVT_RBASE << 3) 

#define C6X1X_EDMA_MCASP_EVT_XBASE      0x00000100
#define C6X1X_EDMA_MCASP_EVT_XUNDRN     (C6X1X_EDMA_MCASP_EVT_XBASE << 0)
#define C6X1X_EDMA_MCASP_EVT_XSYNCERR   (C6X1X_EDMA_MCASP_EVT_XBASE << 1)
#define C6X1X_EDMA_MCASP_EVT_XCKFAIL    (C6X1X_EDMA_MCASP_EVT_XBASE << 2)
#define C6X1X_EDMA_MCASP_EVT_XDMAERR    (C6X1X_EDMA_MCASP_EVT_XBASE << 3) 
         
/* McASP Clock, High Clock, Frame Sync Source definition */
#define C6X1X_EDMA_MCASP_EXTERNAL       0
#define C6X1X_EDMA_MCASP_XMT            1
#define C6X1X_EDMA_MCASP_RCV            2
#define C6X1X_EDMA_MCASP_RCVXMT         3
#define C6X1X_EDMA_MCASP_XMTRCV         3



/* Driver function table to be used by applications. */
extern far IOM_Fxns C6X1X_EDMA_MCASP_FXNS;

/* McASP Event Handler */
typedef Void (*C6X1X_EDMA_MCASP_TevtCallback)(Uns evtMask, Uns devId);
 
typedef struct C6X1X_EDMA_MCASP_EvtCallback {
    C6X1X_EDMA_MCASP_TevtCallback evtFxn;
    Uns evtMask; /* ored events */
} C6X1X_EDMA_MCASP_EvtCallback;

/* Device setup parameters */
/* Definition for the following constant VALUE 
 * 0: External; 1(C6X1X_EDMA_MCASP_XMT): transmitter only; 
 * 2(C6X1X_EDMA_MCASP_RCV) receiver only;
 * 3(C6X1X_EDMA_MCASP_RCVXMT/C6X1X_EDMA_MCASP_XMTRCV) both
 */
typedef struct C6X1X_EDMA_MCASP_DevParams {
    Int versionId;   /* Set to the version number used by the application */
    Bool cacheCalls; /* Set to TRUE if buffers are in external memory */
    Uns enableClkg; /* Set VALUE for internal clock generator */
    Uns enableHclkg; /* Set VALUE for internal high frequency clock generator */
    Uns enableFsyncg;  /* Set VALUE for internal framesync generator */
    Int irqId;       /* IRQ number used for EDMA interrupt */
    MCASP_Config *mcaspCfgPtr; /* Configuration to use for the McASP */
    Int inEvtIrqId;    /* IRQ number used for McASP Event interrupt */
    Int outEvtIrqId;    /* IRQ number used for McASP Event interrupt */
    C6X1X_EDMA_MCASP_EvtCallback *evtCallback; /* Register events callback */
    Uns inEvtIntrMask; /* Interrupt mask, set while executing input ISR */
    Uns outEvtIntrMask; /* Interrupt mask, set while executing output ISR */
    Uns edmaIntrMask; /* Interrupt mask, set while executing EDMA ISR */
} C6X1X_EDMA_MCASP_DevParams;

/* Channel setup parameters */
typedef struct C6X1X_EDMA_MCASP_ChanParams {
    Uns tdmChans;    /* Number of TDM channels used by the McASP */
    Uns edmaChan;  /* McASP Transmit/Receive Event Mapped EDMA Channel Num */
    EDMA_Config *edmaCfgPtr; /* Configuration to use for the EDMA channel */
} C6X1X_EDMA_MCASP_ChanParams;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* C6X1X_EDMA_MCASP_ */

