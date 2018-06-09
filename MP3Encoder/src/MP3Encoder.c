/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/*
 *  ======== main.c ========
 * 
 *  This example demonstrates the use of IOM drivers with SIOs and tasks by 
 *  using the DIO class driver with a user defined device mini-driver 
 *  called "codec" and a class driver DIO instance called "dio_codec". This is 
 *  the loopback application where audio is read from an input SIO, then sent 
 *  back via an output SIO.

 *  The following objects need to be created in the DSP/BIOS
 *  configuration for this application:
 *
 *  * A UDEV object, which links in a user device driver. In this
 *    case the UDEV is a codec based IOM device driver.
 *  * A DIO object, which links the UDEV object.
 *  * A TSK object, with the function to run set to the function demo
 *    defined in this file.
 *  * A LOG named trace for debug and status output.
 */
#include <std.h>

#include <log.h>
#include <sys.h>
#include <mem.h>
#include <sio.h>
#include <csl.h>
#include <csl_cache.h>
#include <csl_i2c.h>
#include <csl_mcasp.h>

#include <dec6713.h>
#include <dec6713_edma_aic23.h>
#include <my_sys.h>
#include <iva.h>
#include <csl_pll.h>
#include <led.h>
#define NUM_CODEC_CHANNELS	 2	/* stereo: left + right		*/
#define SAMPLEING_RATE		8	/* 8 samples/ms			*/
#define FRAME_SIZE			8	/* 10 ms					*/
#define	NFRAMES				100	/* 100 frames = 1 second	*/
#define	BUFLEN		(NUM_CODEC_CHANNELS*SAMPLEING_RATE*FRAME_SIZE)
#define CHANELL_LEN (BUFLEN / 2)
#ifdef _6x_
extern far LOG_Obj trace;

/* 
 * Buffers placed in external memory are aligned on a 128 bytes boundary.
 * In addition, the buffer should be of a size multiple of 128 bytes for 
 * the cache work optimally on the C6x.
 */
#define BUFALIGN 128    /* alignment of buffer to allow use of L2 cache */
#else
extern LOG_Obj trace;
#define BUFALIGN 1
#endif

#define BUFSIZE (BUFLEN * sizeof(short)) 

/*
 *  ======== DEC6713_DEVPARAMS ========
 *  This static initialization defines the default parameters used for
 *  DEC6713_EDMA_AIC23 IOM driver
 */
DEC6713_EDMA_AIC23_DevParams DEC6713_CODEC_DEVPARAMS =
        DEC6713_EDMA_AIC23_DEFAULT_DEVPARAMS;

/* inStream and outStream are SIO handles created in main */
SIO_Handle inStream, outStream;

/* Function prototype */
static Void prime();
static Void PlayAudio();
/*
 * ======== main ========
 */
float tem_in[2];
void tsk_Audio();
Void main()
 {
	//MCASP_Handle hMcasp = (MCASP_Handle)EC6713_AIC23_OpenCodec();
	//DEC6713_LED_init();
	//pll_set();
	DEC6713_init();
	//pll_set();
	PLL_RSET(PLLM, 24);
	led_configuration();
	//tsk_Audio();
		//iva_step(&iva_instance, tem_in);
    //LOG_printf(&trace, "echo started");
}

//short *pEchoBuf;
const int echoBufSize = NFRAMES * BUFLEN;
int echoBufOffset = 0;
int delayTime = 500;
int echoAtt = 64;

/*
 *  ======== initEchoBuffer ========
 * Allocate echo buffer and fill with silence
 */
//int initEchoBuffer()
//{
	//pEchoBuf = MEM_calloc(0, echoBufSize * sizeof(short), BUFALIGN);
	//return (pEchoBuf == MEM_ILLEGAL) ? -1 : 0;
//	return 0;
//}

/*
 *  ======== copyWithEcho ========
 * Copy an incoming buffer with stereo audio samples into an outgoing
 * buffer, mixing with samples from the echo buffer indexed by
 * the parameter timeDelay.  The parameter "a" indicates the mix
 * attenuation * 256 (i.e. 64 = 0.25).
 */
/* odd(i % 2 = 1) -> left */
/* even( i % 2 = 0 ) -> right */

//float left_channel[CHANELL_LEN], right_channel[CHANELL_LEN];
float double_buffer[2];
float **out_buffer;
#define SHORT_MAX  32768
void PlayAudio(short *inBuf, short *outBuf)
{
	int i,j;
	for(i=0;i<BUFLEN;i+=2)
	{
		double_buffer[0] = ((float)(*(inBuf++))) / SHORT_MAX;
		double_buffer[1] = ((float)(*(inBuf++))) / SHORT_MAX;
		if ((out_buffer = iva_step(&iva_instance, double_buffer))!=NULL)
		{
			for (j = 0; j < SHIFT_SIZE; j++)
			{
				*outBuf++ = (short)(out_buffer[j][0] * SHORT_MAX);
				*outBuf++ = (short)(out_buffer[j][1] * SHORT_MAX);
			}
		}
		//*outBuf++ = (short)(double_buffer[0] * SHORT_MAX);
		//*outBuf++ = (short)(double_buffer[1] * SHORT_MAX);
	}
}

/*
 * ======== createStreams ========
 */
static Void createStreams()
{
    SIO_Attrs attrs;
    
    /* align the buffer to allow it to be used with L2 cache */
    attrs = SIO_ATTRS;
    attrs.align = BUFALIGN;
    attrs.model = SIO_ISSUERECLAIM;
	LOG_printf(&trace, "echo started");
    /* open the I/O streams */
    inStream = SIO_create("/dio_codec", SIO_INPUT, BUFSIZE, &attrs);
    LOG_printf(&trace, "echo started");
    if (inStream == NULL) {
    	while(1);
        SYS_abort("Create input stream FAILED.");
    }

    outStream = SIO_create("/dio_codec", SIO_OUTPUT, BUFSIZE, &attrs);
    if (outStream == NULL) {
    	while(1);
        SYS_abort("Create output stream FAILED.");
    }
}

/*
 * ======== prime ========
 */
static Void prime()
{
    Ptr buf0, buf1, buf2, buf3;

    LOG_printf(&trace, "Allocate buffers started");

    /* Allocate buffers for the SIO buffer exchanges */
#if 0
    buf0 = (Ptr)MEM_calloc(0, BUFSIZE, BUFALIGN);
    buf1 = (Ptr)MEM_calloc(0, BUFSIZE, BUFALIGN);
    buf2 = (Ptr)MEM_calloc(0, BUFSIZE, BUFALIGN);
    buf3 = (Ptr)MEM_calloc(0, BUFSIZE, BUFALIGN);
#else
    buf0 = (Ptr)malloc(BUFSIZE);memset(buf0, 0, BUFSIZE);

        buf1 = (Ptr)malloc(BUFSIZE);memset(buf1, 0, BUFSIZE);
        buf2 = (Ptr)malloc(BUFSIZE);memset(buf2, 0, BUFSIZE);
        buf3 = (Ptr)malloc(BUFSIZE);memset(buf3, 0, BUFSIZE);
#endif
    if (buf0 == NULL || buf1 == NULL || buf2 == NULL || buf3 == NULL) {
        SYS_abort("MEM_calloc failed.");
    } 
    
    /* Issue the first & second empty buffers to the input stream */
    if (SIO_issue(inStream, buf0, SIO_bufsize(inStream), NULL) != SYS_OK) {
        SYS_abort("Error issuing buffer to the input stream");
    }
    if (SIO_issue(inStream, buf1, SIO_bufsize(inStream), NULL) != SYS_OK) {
        SYS_abort("Error issuing buffer to the input stream");
    }

    /* Issue the first & second empty buffers to the output stream */
    if (SIO_issue(outStream, buf2, SIO_bufsize(outStream), NULL) != SYS_OK) {
        SYS_abort("Error issuing buffer to the output stream");
    }
    if (SIO_issue(outStream, buf3, SIO_bufsize(outStream), NULL) != SYS_OK) {
        SYS_abort("Error issuing buffer to the output stream");
    }
}

/*
 * ======== tskAudioDemo ========
 * This function copies from the input SIO to the output SIO. You could
 * easily replace the copy function with a signal processing algorithm. 
 */
short *inbuf, *outbuf;
Void tsk_Audio()
{
    Int nmadus;         /* number of minimal addressable units */


	/* Call createStream function to create I/O streams */
    createStreams();
    
    /* Call prime function to do priming */
    prime();
    
    //initEchoBuffer();
    my_sys_init();
    /* Loop forever looping back buffers */
    for (;;) {
    	//iva_step(&iva_instance, tem_in);
        /* Reclaim full buffer from the input stream */
        if ((nmadus = SIO_reclaim(inStream, (Ptr *)&inbuf, NULL)) < 0) {
            SYS_abort("Error reclaiming full buffer from the input stream");
        }

        /* Reclaim empty buffer from the output stream to be reused */
        if (SIO_reclaim(outStream, (Ptr *)&outbuf, NULL) < 0) {
            SYS_abort("Error reclaiming empty buffer from the output stream");
        }

		/* process echo algorithm */
		PlayAudio(inbuf, outbuf);
		led_toggle();
        /* Issue full buffer to the output stream */
        if (SIO_issue(outStream, outbuf, nmadus, NULL) != SYS_OK) {
            SYS_abort("Error issuing full buffer to the output stream");
        }

        /* Issue an empty buffer to the input stream */
        if (SIO_issue(inStream, inbuf, SIO_bufsize(inStream), NULL) != SYS_OK) {
            SYS_abort("Error issuing empty buffer to the input stream");
        }
    }
}

Void tsk_Process(void)
{
		while(1);
}


