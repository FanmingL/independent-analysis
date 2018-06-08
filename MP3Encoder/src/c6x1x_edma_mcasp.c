/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.11.00.00 11-04-03 (ddk-b13)" */
/* 
 *  ======== c6x1x_edma_mcasp.c ========
 * 
 *  Generic McASP driver for the TMS320C6x1x series. Uses the EDMA.
 */

#include <std.h>
#include <atm.h>
#include <hwi.h>
#include <que.h>

#include <iom.h>

#include <csl.h>
#include <csl_mcasp.h>
#include <csl_irq.h>
#include <csl_edma.h>
#include <csl_cache.h>
#include <csl_chip.h>

#include <c6x1x_edma_mcasp.h>

/*
 *  Macro to start serializer & state machine
 *  if turned on, driver will start both transmit and receive serializer 
 *  and state machine synchronously  
 */
#define STARTSERIALIZERSYNC     0

/*
 *  Macro to enable edma loop job interrupt
 *  if turned on, each time edma loop job gets running, an ineterrupt will
 *  be generated. This is only used for debug purpose.
 */
#define ENABLELOOPINTR          0 

#if ENABLELOOPINTR
#include <log.h>
extern far LOG_Obj trace;
#endif

#define IRQEVTINPUT             5
#define IRQEVTOUTPUT            6
#define IRQEDMA                 8


/* Maximum number of EDMA jobs linked at a time (Must be 2). */
#define MAXLINKCNT 2

/* Used as index since IOM mode is a bit mask and not an index */
#define INPUT   0
#define OUTPUT  1

/* States for chanCleanUp() */
#define SETFALSE 1
#define FREETCC 2
#define FREETABLE 3
#define FREETABLEEX 4
#define DELCHAN 5

/* Macro to increment and return the indice that ranges over MAXLINKCNT */
#define nextIndex(index) ((index) ^ 1)

/* Number of ports available */
#define NUMPORTS _MCASP_PORT_CNT 

/* Number of channels per port (one input and one output channel) */
#define NUMCHANS 2

/* Structure containing channel specific variables */
typedef struct ChanObj {
    Uns inUse;                /* True if the channel is in use */
    Int mode;                 /* Input or output channel */
    struct PortObj *port;     /* Pointer to the port which owns this chan */
    EDMA_Handle xferPram;     /* Handle to transfer PaRAM */
    EDMA_Handle pramTbl[MAXLINKCNT]; /* Handles to link PaRAMs */
    EDMA_Handle prevPramPtr;  /* Points to the PaRAM last used */
    EDMA_Handle loophEdma;    /* Handle to the Loop job PaRAM */
    IOM_Packet *flushPacket;  /* Holds the flushpacket (if any) */
    IOM_Packet *abortPacket;  /* Holds the abortpacket (if any) */
    IOM_Packet *packetList[MAXLINKCNT]; /* Holds linked  packets */
    QUE_Obj packetQueue;      /* Holds submitted but not linked packets */
    Int submitCount;          /* Number of submit calls pending */
    Int writeIndex;           /* Index of next PaRAM to write to */
    Int readIndex;            /* Index of next PaRAM to read from */
    Int tcc;                  /* Channel transfer complete code */
    IOM_TiomCallback cbFxn;   /* Called when I/O complete */
    Ptr cbArg;                /* Argument to callback function */
} ChanObj, *ChanHandle;

/* Structure containing port specific variables */
typedef struct PortObj {
    Uns inUse;                /* True if the port is in use */
    Int devId;                /* The device id passed to mdBindDev() */
    Bool cacheCalls;          /* Submitted buffers are in cacheable memory */
    Uint32 enableHclkg;       /* Holds enable Hclk variable */
    Uint32 enableClkg;        /* Holds enable Clk variable */
    Uint32 enableFsyncg;      /* Holds enable Fsync variable */
    MCASP_Handle hMcasp;      /* CSL Device handle */
    ChanObj chans[NUMCHANS];  /* The channels associated with the port */
    Uns chanCreated;          /* One channel in this port has been Created */
    C6X1X_EDMA_MCASP_TevtCallback evtCallback; /* event callback */
    Uns evtMask;              /* registered events */
} PortObj, *PortHandle;

/* Declare the port and channel structures */
static PortObj ports[NUMPORTS];

/* Define EDMA Event Id's Array */
static Uns eventIds[NUMPORTS][2] = {
    { EDMA_CHA_AREVT0, EDMA_CHA_AXEVT0 },
#if NUMPORTS >= 2
    { EDMA_CHA_AREVT1, EDMA_CHA_AXEVT1 },
#endif
#if NUMPORTS == 3
    { EDMA_CHA_AREVT2, EDMA_CHA_AXEVT2 }
#endif
};

/*
 * Forward declaration of the IOM interface functions. They are only
 * exposed via the IOM function table to avoid namespace pollution.
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams);
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg);
static Int mdDeleteChan(Ptr chanp);
static Int mdSubmitChan(Ptr chanp, IOM_Packet *packet);
static Int mdUnBindDev(Ptr devp);

#if ENABLELOOPINTR
static Void isrLoop(Int tcc);
#endif

/* Public IOM interface table */
IOM_Fxns C6X1X_EDMA_MCASP_FXNS = {
    &mdBindDev,
    &mdUnBindDev,
    IOM_CONTROLCHANNOTIMPL,
    &mdCreateChan,
    &mdDeleteChan,
    &mdSubmitChan
};

/* Local function prototypes */
static Void chanCleanUp(ChanHandle chan, Uns state);
static Void isrCommon(ChanHandle chan);
static Void isrInput(Int tcc);
static Void isrOutput(Int tcc);
static Void isrEvent(Int mode);
static Void linkPacket(ChanHandle chan, IOM_Packet *packet);

/* Local driver variables. */
static Uint32 loopDstBuf;
static Uint32 loopSrcBuf;

#if ENABLELOOPINTR
static Void isrLoop(Int tcc)
{
    LOG_printf(&trace, "LOOP=%d", tcc);
}
#endif

/*
 * ======== chanCleanUp ========
 * Cleans up the channel resources.
 */
static Void chanCleanUp(ChanHandle chan, Uns state)
{
    switch(state) {
    case DELCHAN:
        /* Close the EDMA channel */
        EDMA_close(chan->xferPram);

        /* Disable transfer interrupts from the EDMA */
        EDMA_intDisable(chan->tcc);
        /* will fall through the next case */
    case FREETABLEEX:
        /* Free the EDMA link PaRAM tables */
        EDMA_freeTableEx(MAXLINKCNT, chan->pramTbl);
        /* will fall through the next case */
    case FREETABLE:
        /* Free the loop EDMA PaRAM table */
        EDMA_freeTable(chan->loophEdma);
        /* will fall through the next case */
    case FREETCC:
        /* Free the transfer complete code */
        EDMA_intFree(chan->tcc);
        /* will fall through the next case */
    case SETFALSE:
        /* Mark the channel as closed */
        chan->inUse = FALSE;
        break;
    }
}
/*
 * ======== isrCommon ========
 * Shared ISR code between input and output. Processes a normal EDMA job.
 */
static Void isrCommon(ChanHandle chan)
{
    IOM_Packet *packet;
    Int cnt;

    /* Check to see if this is a completed async abort call */
    if (chan->abortPacket) {
        /* Discard all packets in transmission or queued up */
        if (chan->submitCount > MAXLINKCNT) {
            cnt = MAXLINKCNT;
        }
        else {
            cnt = chan->submitCount;
        }

        while (cnt > 0) {
            packet = chan->packetList[chan->readIndex];
            packet->status = IOM_ABORTED;
            (*chan->cbFxn)(chan->cbArg, packet);

            chan->readIndex = nextIndex(chan->readIndex);
            cnt--;
        }

        while (!QUE_empty(&chan->packetQueue)) {
            packet = QUE_dequeue(&chan->packetQueue);
            packet->status = IOM_ABORTED;
            (*chan->cbFxn)(chan->cbArg, packet);
        }

        /* Reset the driver channel state */
        chan->writeIndex = 0;
        chan->readIndex = 0;
        chan->submitCount = 0;

        chan->abortPacket->status = IOM_COMPLETED;
        (*chan->cbFxn)(chan->cbArg, chan->abortPacket);
        chan->abortPacket = NULL;
        return;
    }

    /* Fetch the completed packet */
    packet = chan->packetList[chan->readIndex];

    chan->readIndex = nextIndex(chan->readIndex);

    /* Mark the packet as completed */
    packet->status = IOM_COMPLETED;

    /* Call the callback function */
    (*chan->cbFxn)(chan->cbArg, packet);

    chan->submitCount--;

    /*
     * See if there are any unlinked packets in the packetQueue
     * and if so link them.
     */
    if (chan->submitCount >= MAXLINKCNT) {
        packet = QUE_dequeue(&chan->packetQueue);
        linkPacket(chan, packet);
    }
}

/*
 * ======== isrInput ========
 * The input isr called from the EDMA dispatcher every time an input
 * EDMA job completes.
 */
static Void isrInput(Int tcc)
{
    ChanHandle chan;
    Int portNbr;

    /* Check which port was responsible for the interrupt */
    for (portNbr = 0; portNbr < NUMPORTS; portNbr++) {
        chan = &ports[portNbr].chans[INPUT];

        if (chan->tcc == tcc && chan->inUse) {

            if (EDMA_RGETH(chan->xferPram, DST) == (Uint32) &loopDstBuf &&
                chan->submitCount > 1 && !chan->abortPacket) {
                /*
                 * An emulation halt has occured with more than 1 job
                 * submitted. Link the currently executing job (the Loop job)
                 * to the first of the linked jobs which hadn't been called
                 * back. This way we still have the same number of submitted
                 * jobs after the execution continues as we had before the
                 * emulation halt (breakpoint) occured (this preserves double
                 * buffering if used).
                 */
                EDMA_disableChannel(chan->xferPram);
                EDMA_link(chan->xferPram, chan->pramTbl[chan->readIndex]);
                EDMA_enableChannel(chan->xferPram);
            }
            else {
                /* Call the common ISR code for a finished normal EDMA job */
                isrCommon(chan);
            }
        }
    }
}

/*
 * ======== isrOutput ========
 * The output isr called from the EDMA dispatcher every time an output
 * EDMA job completes.
 */
static Void isrOutput(Int tcc)
{
    ChanHandle chan;
    Int portNbr;

    /* Check which port was responsible for the interrupt */
    for (portNbr = 0; portNbr < NUMPORTS; portNbr++) {
        chan = &ports[portNbr].chans[OUTPUT];

        if (chan->tcc == tcc && chan->inUse) {

            if (EDMA_RGETH(chan->xferPram, SRC) == (Uint32)&loopSrcBuf &&
                chan->submitCount > 1 && !chan->abortPacket) {
                /*
                 * An emulation halt has occured with more than 1 job
                 * submitted. Link the currently executing job (the Loop job)
                 * to the first of the linked jobs which hadn't been called
                 * back. This way we still have the same number of submitted
                 * jobs after the execution continues as we had before the
                 * emulation halt (breakpoint) occured (this preserves double
                 * buffering if used).
                 */
                EDMA_disableChannel(chan->xferPram);
                EDMA_link(chan->xferPram, chan->pramTbl[chan->readIndex]);
                EDMA_enableChannel(chan->xferPram);
            }
            else {
                /* Call the common ISR code for a finished normal EDMA job */
                isrCommon(chan);

                /* Check to see if an async flush has completed */
                if (chan->submitCount == 0 && chan->flushPacket) {
                    chan->flushPacket->status = IOM_COMPLETED;
                    (*chan->cbFxn)(chan->cbArg,chan->flushPacket);
                    chan->flushPacket = NULL;
                }
            }
        }
    }
}

/*
 * ======== isrEvent ========
 * The event isr called when an McASP input/output event happens
 * This is used for exception event handling asserted by McASP. Normal data
 * hanlding is done by EDMA. 
 */
static Void isrEvent(Int mode)
{
    MCASP_Handle hMcasp;
    Uns portNbr;
    Uns events;
    Uns eventReturn;
    
    /* Check which port was responsible for the interrupt */
    for (portNbr = 0; portNbr < NUMPORTS; portNbr++) {
        if (ports[portNbr].inUse) {
            hMcasp = ports[portNbr].hMcasp;
            events = 0;
            if (mode == INPUT) {          
                if (MCASP_FGETH(hMcasp, RSTAT, ROVRN)) {
                    MCASP_FSETSH(hMcasp, RSTAT, ROVRN, YES);
                    events |= C6X1X_EDMA_MCASP_EVT_ROVRN;
                } 
                if (MCASP_FGETH(hMcasp, RSTAT, RCKFAIL)) {
                    MCASP_FSETSH(hMcasp, RSTAT, RCKFAIL, YES);
                    events |= C6X1X_EDMA_MCASP_EVT_RCKFAIL;
                }
                if (MCASP_FGETH(hMcasp, RSTAT, RSYNCERR)) {
                    MCASP_FSETSH(hMcasp, RSTAT, RSYNCERR, YES);
                    events |= C6X1X_EDMA_MCASP_EVT_RSYNCERR;
                }   
                if (MCASP_FGETH(hMcasp, RSTAT, RDMAERR)) {
                    MCASP_FSETH(hMcasp, RSTAT, RDMAERR, 1);
                    events |= C6X1X_EDMA_MCASP_EVT_RDMAERR;
                }   
                /* Clear not registered events here */
                MCASP_RSETH(hMcasp, RSTAT, 0xFFFFFFFF);
            } 
            else {
                if (MCASP_FGETH(hMcasp, XSTAT, XUNDRN)) {
                    MCASP_FSETSH(hMcasp, XSTAT, XUNDRN, YES);
                    events |= C6X1X_EDMA_MCASP_EVT_XUNDRN;
                } 
                if (MCASP_FGETH(hMcasp, XSTAT, XCKFAIL)) {
                    MCASP_FSETSH(hMcasp, XSTAT, XCKFAIL, YES);
                    events |= C6X1X_EDMA_MCASP_EVT_XCKFAIL;
                }
                if (MCASP_FGETH(hMcasp, XSTAT, XSYNCERR)) {
                    MCASP_FSETSH(hMcasp, XSTAT, XSYNCERR, YES);
                    events |= C6X1X_EDMA_MCASP_EVT_XSYNCERR;
                }   
                if (MCASP_FGETH(hMcasp, XSTAT, XDMAERR)) {
                    MCASP_FSETH(hMcasp, XSTAT, XDMAERR, 1);
                    events |= C6X1X_EDMA_MCASP_EVT_XDMAERR;
                }   
                /* Clear not registered events here */
                MCASP_RSETH(hMcasp, XSTAT, 0xFFFFFFFF);
            }
            eventReturn = ports[portNbr].evtMask & events;
 
            if (eventReturn) {
                (*ports[portNbr].evtCallback)(eventReturn, portNbr);
            } 
        }   
    }    
}

/*
 * ======== linkPacket ========
 * Links a packet with the EDMA. When called by mdSubmitChan() it is called
 * with all interrupts disabled, but when called by an ISR only the EDMA IRQ
 * is disabled.
 */
static Void linkPacket(ChanHandle chan, IOM_Packet *packet)
{
    EDMA_Handle pramPtr;
    Uns edmaCnt;

    /* Store the packet in the packetList */
    chan->packetList[chan->writeIndex] = packet;

    /* Set up pointer to link PaRAM to write submit job info to */
    pramPtr = chan->pramTbl[chan->writeIndex];
    chan->writeIndex = nextIndex(chan->writeIndex);

    /* Load the buffer pointer into the EDMA */
    if (chan->mode == INPUT) {
        EDMA_RSETH(pramPtr, DST, (Uint32) packet->addr);
    }
    else {
        EDMA_RSETH(pramPtr, SRC, (Uint32) packet->addr);
    }

    /*
     * Load the transfer count (in samples) into the EDMA. Use the ESIZE
     * field of the EDMA job to calculate number of samples.
     */
    edmaCnt = (Uint32)packet->size >> (2 - EDMA_FGETH(pramPtr, OPT, ESIZE));
    EDMA_FSETH(pramPtr, CNT, FRMCNT, (edmaCnt / 
            EDMA_FGETH(pramPtr, CNT, ELECNT) - 1)); 

    /*
     * Link to loop EDMA job upon termination. This way we won't
     * loose the frame sync if the channel is starved.
     */
    EDMA_link(pramPtr, chan->loophEdma);

    /* Disable the EDMA channel to make sure current job doesn't complete */
    EDMA_disableChannel(chan->xferPram);

    /*
     * Link the currently executing job to the new job. This can be
     * either the loop EDMA job or a real data EDMA job.
     */
    EDMA_link(chan->xferPram, pramPtr);

    if (chan->submitCount > 0) {
        /*
         * We need to link the parameter space corresponding to the running
         * job so that if a breakpoint occurs, we know how to recover.
         */
        EDMA_link(chan->prevPramPtr, pramPtr);
    }

    /* Reenable the EDMA channel */
    EDMA_enableChannel(chan->xferPram);

    /* Save the new job for the loop above for next time */
    chan->prevPramPtr = pramPtr;
}

/*
 * ======== mdBindDev ========
 * This function allocates and configures the McASP port specified by devId.
 */
static Int mdBindDev(Ptr *devp, Int devid, Ptr devParams)
{
    Uns old;
    Int inIrqId;
    int outIrqId;
    int irqId;
    PortHandle port;
    HWI_Attrs hwiAttrs;
    C6X1X_EDMA_MCASP_DevParams *params =
        (C6X1X_EDMA_MCASP_DevParams *) devParams;

    /* This driver must receive a valid devparams */
    if (params == NULL) {
        return (IOM_EBADARGS);
    }

    /* Check the version number */
    if (params->versionId != C6X1X_EDMA_MCASP_VERSION_1){
        /* Unsupported version */
        return(IOM_EBADARGS);
    }

    /* Get the device parameters of the specified port */
    port = &ports[devid];

    /* Mark the port as in use */
    old = ATM_setu(&(port->inUse), TRUE);

    /* Check if the port was already bound */
    if (old) {
        return (IOM_EALLOC);
    }

    /* Map the supplied IRQ to the EDMA event */
    if (params->irqId > 0 ) {
        irqId = params->irqId;
    }
    else {
        irqId = IRQEDMA;
    }
    IRQ_map(IRQ_EVT_EDMAINT, irqId);

    hwiAttrs.intrMask = params->edmaIntrMask;
    hwiAttrs.ccMask = IRQ_CCMASK_NONE; /* the default value */
    hwiAttrs.arg = NULL;

    /* Plug the EDMA dispatcher into the HWI dispatcher */
    HWI_dispatchPlug(irqId, (Fxn)EDMA_intDispatcher, -1, &hwiAttrs);
   
    /* Set the McASP high frequency sample rate generator */
    /* Set the McASP sample rate generator */
    /* Set the McASP frame sync generator */
    port->enableHclkg = params->enableHclkg;
    port->enableClkg = params->enableClkg;
    port->enableFsyncg = params->enableFsyncg;

    /* True if buffers are in external memory */
    port->cacheCalls = params->cacheCalls;

    /* Store the devid */
    port->devId = devid;

    /* No channel create yet */
    port->chanCreated = 0;

    /* Open and reset the McASP */
    port->hMcasp = MCASP_open(devid, MCASP_OPEN_RESET);

    if (port->hMcasp == INV) {
        return (IOM_EALLOC);
    }

    /* Configure the McASP with the supplied configuration */
    MCASP_config(port->hMcasp, params->mcaspCfgPtr);

    if (params->evtCallback != NULL) {
        /* register the events */
        port->evtCallback = params->evtCallback->evtFxn;
        port->evtMask = params->evtCallback->evtMask;
 
        if (params->inEvtIrqId > 0) {
            inIrqId = params->inEvtIrqId;
        }
        else {
            inIrqId = IRQEVTINPUT;
        } 

        if (params->outEvtIrqId > 0) {
            outIrqId = params->outEvtIrqId;
        }
        else {
            outIrqId = IRQEVTOUTPUT;
        } 

        IRQ_map(MCASP_getXmtEventId(port->hMcasp), outIrqId);
        IRQ_map(MCASP_getRcvEventId(port->hMcasp), inIrqId); 

        hwiAttrs.intrMask = params->inEvtIntrMask;
        hwiAttrs.ccMask = IRQ_CCMASK_NONE; /* the default value */
        hwiAttrs.arg = INPUT;
        HWI_dispatchPlug(inIrqId, (Fxn)isrEvent, -1, &hwiAttrs);

        hwiAttrs.intrMask = params->outEvtIntrMask;
        hwiAttrs.ccMask = IRQ_CCMASK_NONE; /* the default value */
        hwiAttrs.arg = OUTPUT;
        HWI_dispatchPlug(outIrqId, (Fxn)isrEvent, -1, &hwiAttrs);
    } 
    else {
        port->evtCallback = NULL;
    }

    /* set 0xe for debug, real value should be 0 */
    loopSrcBuf = 0;
    loopDstBuf = 0; 

    /* Return the device handle and a status code for success */
    *devp = port;
    return (IOM_COMPLETED);
}

/*
 * ======== mdCreateChan ========
 * This function creates and configures a device channel.
 */
static Int mdCreateChan(Ptr *chanp, Ptr devp, String name, Int mode,
                        Ptr chanParams, IOM_TiomCallback cbFxn, Ptr cbArg)
{
    PortHandle port = (PortHandle) devp;
    C6X1X_EDMA_MCASP_ChanParams *params =
        (C6X1X_EDMA_MCASP_ChanParams *) chanParams;
    ChanHandle chan;
    Uns old;
    Uns esize;
    Uns cnt;
    Uns fs;
    Uns elerld;
    Uns mcaspAddr;
    Int edmaChanEvent;
    Int i;
#if ENABLELOOPINTR
    Int loopTcc;
#endif

    /*
     * Configuration structure for the loop EDMA job. If the McASP
     * is left to free running, this loop job is running when there is
     * no data to transfer. This is useful if the McASP is externally
     * clocked.  If the driver is starved or if an emulation halt
     * (breakpoint) occurs, the frame sync will still be correct when
     * the driver continues transmitting data.
     */
    EDMA_Config loopEdmaCfg = {
        EDMA_FMKS(OPT, PRI, HIGH)           |
        EDMA_FMKS(OPT, ESIZE, 32BIT)        |
        EDMA_FMKS(OPT, 2DS, NO)             |
        EDMA_FMKS(OPT, SUM, NONE)           |
        EDMA_FMKS(OPT, 2DD, NO)             |
        EDMA_FMKS(OPT, DUM, NONE)           |
        EDMA_FMKS(OPT, TCINT, NO)           |
        EDMA_FMKS(OPT, TCC, DEFAULT)        |
        EDMA_FMKS(OPT, LINK, YES)           |
        EDMA_FMKS(OPT, FS, NO),

        EDMA_FMK (SRC, SRC, NULL),

        EDMA_FMK (CNT, FRMCNT, 0)           |
        EDMA_FMK (CNT, ELECNT, 0),

        EDMA_FMKS(IDX, FRMIDX, OF(0))       |
        EDMA_FMKS(IDX, ELEIDX, OF(0)),

        EDMA_FMK (DST, DST, NULL),

        EDMA_FMK (RLD, ELERLD, 0)           |
        EDMA_FMK (RLD, LINK, NULL)
    };


    /* This driver needs a valid channel parameter structure passed */
    if (params == NULL) {
        return (IOM_EBADARGS);
    }

    /* Use own indexes since IOM mode is a bit mask and not an index */
    if (mode == IOM_INPUT) {
        chan = &port->chans[INPUT];
        chan->mode = INPUT;
    }
    else {
        chan = &port->chans[OUTPUT];
        chan->mode = OUTPUT;
    }

    /* Mark the channel as used */
    old = ATM_setu(&(chan->inUse), TRUE);

    /* Make sure the channel wasn't already created */
    if (old) {
        return (IOM_EALLOC);
    }

    /* Initialise the channel structure */
    chan->cbFxn = cbFxn;
    chan->cbArg = cbArg;
    chan->port = port;
    chan->writeIndex = 0;
    chan->readIndex = 0;
    chan->submitCount = 0;
    chan->flushPacket = NULL;
    chan->abortPacket = NULL;

    /* Initialize the packet queue */
    QUE_new(&chan->packetQueue);

    /*
     * Set the number of elements (corresponding to number of McASP TDM
     * channels) used in the Loop job to preserve the frame sync.
     * FRMCNT should be (num of TDM slots - 1);
     */
    cnt = params->tdmChans - 1;
    EDMA_FSETA(&loopEdmaCfg.cnt, CNT, FRMCNT, cnt);
    EDMA_FSETA(&loopEdmaCfg.cnt, CNT, ELECNT, params->edmaCfgPtr->cnt);

    /* Use the same sample size in the Loop job as in normal jobs */
    esize = EDMA_FGETA(&params->edmaCfgPtr->opt, OPT, ESIZE);
    EDMA_FSETA(&loopEdmaCfg.opt, OPT, ESIZE, esize);

    /* Use  the same FS info as in the normal job */
    fs = EDMA_FGETA(&params->edmaCfgPtr->opt, OPT, FS); 
    EDMA_FSETA(&loopEdmaCfg.opt, OPT, FS, fs);

    /* Use  the same ELERLD info as in the normal job */
    elerld = EDMA_FGETA(&params->edmaCfgPtr->rld, RLD, ELERLD); 
    EDMA_FSETA(&loopEdmaCfg.rld, RLD, ELERLD, elerld); 

    /* Allocate a TCC for the EDMA */
    chan->tcc = EDMA_intAlloc(-1);
    /* If tcc > 15 we abort. */
    if (chan->tcc == -1 || chan->tcc > 15) {
        chanCleanUp(chan, SETFALSE);
        return (IOM_EALLOC);
    }

#if ENABLELOOPINTR
    loopTcc = EDMA_intAlloc(-1);
    /* If tcc > 15 we abort. */
    if (loopTcc == -1 || loopTcc > 15) {
        EDMA_intFree(loopTcc);
        chanCleanUp(chan, FREETCC);
        return (IOM_EALLOC);
    }
#endif

    /* Allocate an EDMA PaRAM for the Loop EDMA job. */
    chan->loophEdma = EDMA_allocTable(-1);

    if (chan->loophEdma == EDMA_HINV) {
        chanCleanUp(chan, FREETCC);
        return (IOM_EALLOC);
    }

    /*
     * Allocate EDMA PaRAM link area based on max number of
     * submits possible.
     */
    if (EDMA_allocTableEx(MAXLINKCNT, chan->pramTbl) != MAXLINKCNT) {
        chanCleanUp(chan, FREETABLE);
        return (IOM_EALLOC);
    }

    /* workaround for big endian problem in McASP */
    if (chan->mode == INPUT) {
        mcaspAddr = MCASP_getRbufAddr(port->hMcasp);
    }
    else {
        mcaspAddr = MCASP_getXbufAddr(port->hMcasp);
    }

#ifdef _BIG_ENDIAN
    if (esize == EDMA_OPT_ESIZE_8BIT) {
        mcaspAddr += 3;
    }
    else if (esize == EDMA_OPT_ESIZE_16BIT) {
        mcaspAddr += 2;
    }
#endif

    if (chan->mode == INPUT) {
        
        /* Put input specific parameters in the Loop EDMA config */
        EDMA_RSETA(&loopEdmaCfg.src, SRC, mcaspAddr);
        EDMA_RSETA(&loopEdmaCfg.dst, DST, (Uint32) &loopDstBuf);

        EDMA_RSETA(&params->edmaCfgPtr->src, SRC, mcaspAddr);
        /* Register our isrInput with the EDMA dispatcher */
        EDMA_intHook(chan->tcc, &isrInput);
    }
    else {     
        
        /* Put output specific parameters in the Loop EDMA config */
        EDMA_RSETA(&loopEdmaCfg.src, SRC, (Uint32) &loopSrcBuf);
        EDMA_RSETA(&loopEdmaCfg.dst, DST, mcaspAddr);

        EDMA_RSETA(&params->edmaCfgPtr->dst, DST, mcaspAddr);
        /* Register our isrOutput with the EDMA dispatcher */
        EDMA_intHook(chan->tcc, &isrOutput);
    }

    /* Open theEDMA Channel */
#if (CHIP_DM642)
    edmaChanEvent = eventIds[port->devId][chan->mode]; 
#else
    edmaChanEvent = EDMA_map(eventIds[port->devId][chan->mode], 
            params->edmaChan); 
#endif
    chan->xferPram = EDMA_open(edmaChanEvent, EDMA_OPEN_RESET);

    if (chan->xferPram == EDMA_HINV) {
        chanCleanUp(chan, FREETABLEEX);
        return (IOM_EALLOC);
    }

    /* Program the data EDMA job with the TCC */
    EDMA_FSETA(&params->edmaCfgPtr->opt, OPT, TCC, chan->tcc);

#if ENABLELOOPINTR
    EDMA_intHook(loopTcc, &isrLoop);
    EDMA_FSETA(&loopEdmaCfg.opt, OPT, TCINT, EDMA_OPT_TCINT_YES);
    EDMA_FSETA(&loopEdmaCfg.opt, OPT, TCC, loopTcc);
#endif

    /* Program the link PaRAMs with the config struct */
    for (i=0; i < MAXLINKCNT; i++) {
        EDMA_config(chan->pramTbl[i], params->edmaCfgPtr);
    }

    /* Program the Loop EDMA job with its config struct */
    EDMA_config(chan->loophEdma, &loopEdmaCfg);

    /*
     * Link the Loop EDMA job to itself to make it run
     * continuously when there is no data to transmit.
     */
    EDMA_link(chan->loophEdma, chan->loophEdma);

    /* Configure the EDMA channel to start with the Loop EDMA job */
    EDMA_config(chan->xferPram, &loopEdmaCfg);
    EDMA_link(chan->xferPram, chan->loophEdma);

    /* Transfer complete is edge triggered, so clear before enabling */
    EDMA_intClear(chan->tcc);
    EDMA_intEnable(chan->tcc);

#if ENABLELOOPINTR
    EDMA_intClear(loopTcc);
    EDMA_intEnable(loopTcc);
#endif

    /* Enable the EDMA interrupt */
    IRQ_enable(IRQ_EVT_EDMAINT);

    EDMA_enableChannel(chan->xferPram);

    /* Start the McASP */
    if (chan->mode == INPUT) {
        old = ATM_setu(&(port->chanCreated), TRUE);
        
        /* 
         * clock generator is divided from high frequency clock, so start high
         * freq clock generator first 
         */
        if (port->enableHclkg == C6X1X_EDMA_MCASP_RCV) {
            MCASP_enableHclk(port->hMcasp, MCASP_RCV); 
            while(!MCASP_FGETH(port->hMcasp, GBLCTL, RHCLKRST));
        }
        else if ((port->enableHclkg == C6X1X_EDMA_MCASP_RCVXMT) && (old)) {
            MCASP_enableHclk(port->hMcasp, MCASP_RCVXMT); 
            while(!(MCASP_FGETH(port->hMcasp, GBLCTL, RHCLKRST) & 
                    MCASP_FGETH(port->hMcasp, GBLCTL, XHCLKRST)));
        }

        if (port->enableClkg == C6X1X_EDMA_MCASP_RCV) {
            MCASP_enableClk(port->hMcasp, MCASP_RCV); 
            while(!MCASP_FGETH(port->hMcasp, GBLCTL, RCLKRST)); 
        } 
        else if ((port->enableClkg == C6X1X_EDMA_MCASP_RCVXMT) && (old)) {
            MCASP_enableClk(port->hMcasp, MCASP_RCVXMT); 
            while(!( MCASP_FGETH(port->hMcasp, GBLCTL, RCLKRST) & 
                MCASP_FGETH(port->hMcasp, GBLCTL, XCLKRST)));
        } 

#if STARTSERIALIZERSYNC
        if (old) {
            MCASP_enableSers(port->hMcasp, MCASP_RCVXMT);
            while (!(MCASP_FGETH(port->hMcasp, GBLCTL, RSRCLR) &
                    MCASP_FGETH(port->hMcasp, GBLCTL, XSRCLR)));

            while (MCASP_xdata(port->hMcasp));

            MCASP_enableSm(port->hMcasp, MCASP_RCVXMT);
            while (!(MCASP_FGETH(port->hMcasp, GBLCTL, RSMRST) &
                    MCASP_FGETH(port->hMcasp, GBLCTL, XSMRST)));     
        }
#else
        MCASP_enableSers(port->hMcasp, MCASP_RCV);
        while (!MCASP_FGETH(port->hMcasp, GBLCTL, RSRCLR));

        MCASP_enableSm(port->hMcasp, MCASP_RCV);
        while (!MCASP_FGETH(port->hMcasp, GBLCTL, RSMRST));     
#endif

        if (port->enableFsyncg == C6X1X_EDMA_MCASP_RCV) {
              MCASP_enableFsync(port->hMcasp, MCASP_RCV);
              while(!MCASP_FGETH(port->hMcasp, GBLCTL, RFRST));    
        }
        else if ((port->enableFsyncg == C6X1X_EDMA_MCASP_RCVXMT) && (old)) {
            MCASP_enableFsync(port->hMcasp, MCASP_RCVXMT);
            while(!(MCASP_FGETH(port->hMcasp, GBLCTL, RFRST) & 
                    MCASP_FGETH(port->hMcasp, GBLCTL, XFRST)) );       
        } 
    }
    else {
        old = ATM_setu(&(port->chanCreated), TRUE);

        /* 
         * clock generator is divided from high frequency clock, so start high
         * freq clock generator first 
         */
        if (port->enableHclkg == C6X1X_EDMA_MCASP_XMT) {
            MCASP_enableHclk(port->hMcasp, MCASP_XMT); 
            while(!MCASP_FGETH(port->hMcasp, GBLCTL, XHCLKRST));

        }
        else if ((port->enableHclkg  == C6X1X_EDMA_MCASP_RCVXMT) && (old)) {
            MCASP_enableHclk(port->hMcasp, MCASP_RCVXMT); 
            while(!( MCASP_FGETH(port->hMcasp, GBLCTL, RHCLKRST) & 
                    MCASP_FGETH(port->hMcasp, GBLCTL, XHCLKRST)));
        }

        if (port->enableClkg == C6X1X_EDMA_MCASP_XMT) {
            MCASP_enableClk(port->hMcasp, MCASP_XMT); 
            while(!MCASP_FGETH(port->hMcasp, GBLCTL, XCLKRST));
        }
        else if ((port->enableClkg == C6X1X_EDMA_MCASP_RCVXMT) && (old)) {
            MCASP_enableClk(port->hMcasp, MCASP_RCVXMT); 
            while(!( MCASP_FGETH(port->hMcasp, GBLCTL, RCLKRST) & 
                    MCASP_FGETH(port->hMcasp, GBLCTL, XCLKRST)));
        }


#if STARTSERIALIZERSYNC
        if (old) {
            MCASP_enableSers(port->hMcasp, MCASP_RCVXMT);
            while (!(MCASP_FGETH(port->hMcasp, GBLCTL, XSRCLR) &
                    MCASP_FGETH(port->hMcasp, GBLCTL, RSRCLR)));

            while (MCASP_xdata(port->hMcasp));

            MCASP_enableSm(port->hMcasp, MCASP_RCVXMT);
            while (!(MCASP_FGETH(port->hMcasp, GBLCTL, XSMRST) &
                    MCASP_FGETH(port->hMcasp, GBLCTL, RSMRST)));     
        }
#else
        MCASP_enableSers(port->hMcasp, MCASP_XMT);
        while (!(MCASP_FGETH(port->hMcasp, GBLCTL, XSRCLR)));

        while (MCASP_xdata(port->hMcasp));

        MCASP_enableSm(port->hMcasp, MCASP_XMT);
        while (!(MCASP_FGETH(port->hMcasp, GBLCTL, XSMRST)));     
#endif

        if (port->enableFsyncg == C6X1X_EDMA_MCASP_XMT) {
            MCASP_enableFsync(port->hMcasp, MCASP_XMT);
            while(!MCASP_FGETH(port->hMcasp,GBLCTL, XFRST));       
        }
        else if ((port->enableFsyncg == C6X1X_EDMA_MCASP_RCVXMT) && (old)) {
            MCASP_enableFsync(port->hMcasp, MCASP_RCVXMT);
            while(!(MCASP_FGETH(port->hMcasp,GBLCTL, RFRST) & 
                    MCASP_FGETH(port->hMcasp,GBLCTL, XFRST)) );       
        } 
    } 

    /* enable McASP event interrupt if registered */
    /* Clear CKFAIL error before enable interrupt */
    if (chan->mode == INPUT) { 
        if (port->evtCallback != NULL) {
            if (port->evtMask & C6X1X_EDMA_MCASP_EVT_RCKFAIL) { 
                MCASP_FSETSH(port->hMcasp, RINTCTL, RCKFAIL, DISABLE);
                while (MCASP_FGETH(port->hMcasp, RSTAT, RCKFAIL)) {
                    MCASP_FSETSH(port->hMcasp, RSTAT, RCKFAIL, YES);
                }
                MCASP_FSETSH(port->hMcasp, RINTCTL, RCKFAIL, ENABLE);
            }

            IRQ_enable(MCASP_getRcvEventId(port->hMcasp));
        }
    } 
    else { 
        if (port->evtCallback != NULL) { 
            if (port->evtMask & C6X1X_EDMA_MCASP_EVT_XCKFAIL) { 
                MCASP_FSETSH(port->hMcasp, XINTCTL, XCKFAIL, DISABLE);
                while (MCASP_FGETH(port->hMcasp, XSTAT, XCKFAIL)) {
                    MCASP_FSETSH(port->hMcasp, XSTAT, XCKFAIL, YES);
                }
                MCASP_FSETSH(port->hMcasp, XINTCTL, XCKFAIL, ENABLE);
            }

            IRQ_enable(MCASP_getXmtEventId(port->hMcasp));
        }
    }
    
    *chanp = (Ptr) chan;
    return (IOM_COMPLETED);
}

/*
 * ======== mdDeleteChan ========
 * This function frees a channel and all it's associated resources.
 */
static Int mdDeleteChan(Ptr chanp)
{
    ChanHandle chan = (ChanHandle) chanp;
    PortHandle port = chan->port;

    /* Clean up the channel resources */
    chanCleanUp(chan, DELCHAN);

    /*
     * Reset the McASP transmitter or receiver. If the channel is
     * recreated, mdCreateChan() will reenable the transmitter/receiver
     * and by pulling it out of reset it will also restart the EDMA channel.
     */
    if (chan->mode == INPUT) {
        MCASP_resetRcv(port->hMcasp);
    }
    else {
        MCASP_resetXmt(port->hMcasp);
    }

    return (IOM_COMPLETED);
}

/*
 * ======== mdSubmitChan ========
 * This function transmits a buffer to or from the McASP using the EDMA.
 */
static Int mdSubmitChan(Ptr chanp, IOM_Packet *packet)
{
    ChanHandle chan = (ChanHandle) chanp;
    PortHandle port = chan->port;
    Uns imask;

    /* No packets can be submitted while abort or flush is active */
    if (chan->flushPacket || chan->abortPacket) {
        return (IOM_EBADIO);
    }

    /*
     * Check to see if an abort command has been issued. Note that
     * flushing the input channel is handled the same as abort.
     */
    if ((packet->cmd == IOM_FLUSH && chan->mode == INPUT) ||
        packet->cmd == IOM_ABORT) {

        /* Disable interrupts to protect submitCount */
        imask = HWI_disable();

        /* Store the abort packet for the ISR to check */
        if (chan->submitCount > 0) {
            chan->abortPacket = packet;

            /*
             * Disable the EDMA channel while linking the currently
             * executing job while linking it with the Loop job to make
             * sure the currently executing job doesn't complete before
             * the link is complete.
             */
            EDMA_disableChannel(chan->xferPram);
            EDMA_link(chan->xferPram, chan->loophEdma);
            EDMA_enableChannel(chan->xferPram);
        }

        /* Reenable interrupts */
        HWI_restore(imask);

        if (chan->abortPacket) {
            return(IOM_PENDING);
        }

        /* If there were no buffers in the channel, return synchronously */
        packet->status = IOM_COMPLETED;

        return (IOM_COMPLETED);
    }

    /* Check to see if the submitted packet is an output flush packet */
    if (packet->cmd == IOM_FLUSH && chan->mode == OUTPUT) {
        /* Disable interrupts to protect submitCount */
        imask = HWI_disable();

        /* Store the flush packet for the ISR to check */
        if (chan->submitCount > 0) {
            chan->flushPacket = packet;
        }

        /* Reenable interrupts */
        HWI_restore(imask);

        if (chan->flushPacket) {
            return(IOM_PENDING);
        }

        /* If there were no buffers in the channel, return synchronously */
        packet->status = IOM_COMPLETED;

        return (IOM_COMPLETED);
    }

    if (packet->cmd != IOM_READ && packet->cmd != IOM_WRITE) {
        /* Unsupported command passed */
        return (IOM_ENOTIMPL);
    }

    /* maintain cache coherency */
    if (chan->mode == INPUT) {
        /* CACHE uses words, and packet->size is in nmaus (bytes on c6x) */
        if (port->cacheCalls) {
            CACHE_clean(CACHE_L2, packet->addr, packet->size >> 2);
        }
    }
    else {
        /* CACHE uses words, and packet->size is in nmaus (bytes on c6x) */
        if (port->cacheCalls) {
            CACHE_flush(CACHE_L2, packet->addr, packet->size >> 2);
        }
    }

    /* Disable interrupts to protect submitCount */
    imask = HWI_disable();
    /*
     * If there is no space available for the new packet, put it on a
     * queue to be linked in when space is available. Otherwise link it in.
     */
    if (chan->submitCount >= MAXLINKCNT) {
        QUE_enqueue(&chan->packetQueue, packet);
    }
    else {
        linkPacket(chan, packet);
    }

    chan->submitCount++;

    HWI_restore(imask);

    return (IOM_PENDING);
}

/*
 * ======== mdUnBindDev ========
 * This function frees a port and all it's associated resources.
 */
static Int mdUnBindDev(Ptr devp)
{
    PortHandle port = (PortHandle) devp;

    port->inUse = FALSE;

    /* Close the McASP */
    MCASP_close(port->hMcasp);

    return (IOM_COMPLETED);
}

/*
 * ======== C6X1X_EDMA_MCASP_init ========
 * This function initializes the driver's data structures.
 */
#pragma CODE_SECTION(C6X1X_EDMA_MCASP_init, ".text:init");
Void C6X1X_EDMA_MCASP_init()
{
    PortHandle port;
    ChanHandle chan;
    Int i, j;
    /* Make sure the initialization only happens once for thid driver */
    static Bool curInit = FALSE;

    if (curInit) {
        return;
    }
    curInit = TRUE;

    for (i=0; i<NUMPORTS; i++) {
        port = &ports[i];

        port->inUse = FALSE;

        for (j=0; j<NUMCHANS; j++) {
            chan = &port->chans[j];

            chan->inUse = FALSE;
        }
    }
}

