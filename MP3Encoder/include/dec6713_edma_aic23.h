/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DDK 1.11.00.00 11-04-03 (ddk-b13)" */
/* 
 *  ======== dec6713_edma_aic23.h ========
 */

#ifndef DEC6713_EDMA_AIC23_
#define DEC6713_EDMA_AIC23_

#include <iom.h>
#include <c6x1x_edma_mcasp.h>

#include <aic23.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Version number definition */
#define DEC6713_EDMA_AIC23_VERSION_1 0xAB01


/* Default IRQ number for EDMA is 8 */          
#define DEC6713_EDMA_AIC23_EDMA_ID_DEFAULT  8
#define DEC6713_EDMA_AIC23_INTR_MASK_DEFAULT 1

/* DM642 prototype EVM codec clock frequency 12.288Mhz */
#define AIC23_REG8_12288                                                  \
    AIC23_9BITWORD(         /* REG 8: sample rate control */              \
        0,                  /* reserved */                                \
        0,                  /* clock output divider: 1 (MCLK) */          \
        0,                  /* clock input  divider: 1 (MCLK) */          \
        0,0,0,0,0,          /* sampling rate: ADC 48kHz DAC 48kHz */      \
        0                   /* clock mode select (USB/normal): normal */  \
    ) 

/* DM642 production EVM codec clock frequency 18.432Mhz */
#define AIC23_REG8_18432                                                  \
    AIC23_9BITWORD(         /* REG 8: sample rate control */              \
        0,                  /* reserved */                                \
        0,                  /* clock output divider: 1 (MCLK) */          \
        0,                  /* clock input  divider: 1 (MCLK) */          \
        0,0,0,0,1,          /* sampling rate: ADC 48kHz DAC 48kHz */      \
        0                   /* clock mode select (USB/normal): normal */  \
    ) 

/* 
 *  define an AIC23_DEFAULTPARAMS with 10 default register values 
 *  for DM642 production EVM
 */
#define AIC23_DEFAULTPARAMS_DEC6713 {                                  \
    AIC23_REG0_DEFAULT,                                                 \
    AIC23_REG1_DEFAULT,                                                 \
    AIC23_REG2_DEFAULT,                                                 \
    AIC23_REG3_DEFAULT,                                                 \
    AIC23_REG4_DEFAULT,                                                 \
    AIC23_REG5_DEFAULT,                                                 \
    AIC23_REG6_DEFAULT,                                                 \
    AIC23_REG7_DEFAULT,                                                 \
    AIC23_REG8_18432,                                                   \
    AIC23_REG9_DEFAULT                                                  \
}

#define DEC6713_EDMA_AIC23_DEFAULT_DEVPARAMS {                         \
    DEC6713_EDMA_AIC23_VERSION_1,                                      \
    TRUE, /* Buffers are in external memory by default */               \
    /* Driver disable the McASP high frequecy rate generator */         \
    C6X1X_EDMA_MCASP_EXTERNAL,                                          \
    /* Driver disable the McASP clk rate generator */                   \
    C6X1X_EDMA_MCASP_EXTERNAL,                                          \
    /* Driver disable the McASP frame sync generator */                 \
    C6X1X_EDMA_MCASP_EXTERNAL,                                          \
    DEC6713_EDMA_AIC23_EDMA_ID_DEFAULT, /* Default IRQ number is 8 */  \
    -1, /* use default */                                               \
    -1, /* use default */                                               \
    NULL,  /* do not register mcasp event */                            \
    DEC6713_EDMA_AIC23_INTR_MASK_DEFAULT,                              \
    DEC6713_EDMA_AIC23_INTR_MASK_DEFAULT,                              \
    DEC6713_EDMA_AIC23_INTR_MASK_DEFAULT,                              \
    AIC23_DEFAULTPARAMS_DEC6713                                        \
}
             
/* Driver function table to be used by applications. */
extern far IOM_Fxns DEC6713_EDMA_AIC23_FXNS;

/* Device setup parameters */
typedef struct DEC6713_EDMA_AIC23_DevParams {
    Int versionId;   /* Set to the version number used by the application */
    Bool cacheCalls; /* Set to TRUE if buffers are in external memory */
    Uns enableClkg;  /* Set VALUE for internal clock generator */
    Uns enableHclkg; /* Set VALUE for internal high frequency clock generator */
    Uns enableFsyncg;     /* Set VALUE for internal framesync generator */
    Int irqId;       /* IRQ number to use for EDMA interrupt */
    Int inEvtIrqId;  /* IRQ number used for McASP Event interrupt */
    Int outEvtIrqId; /* IRQ number used for McASP Event interrupt */
    C6X1X_EDMA_MCASP_EvtCallback *evtCallback; /* Register events callback */ 
    Uns inEvtIntrMask;    /* Interrupt mask, set while executing input ISR */
    Uns outEvtIntrMask;   /* Interrupt mask, set while executing output ISR */
    Uns edmaIntrMask;
    AIC23_Params aic23Config; 
} DEC6713_EDMA_AIC23_DevParams;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif /* DEC6713_EDMA_AIC23_ */



