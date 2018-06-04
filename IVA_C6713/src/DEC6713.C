
/*********************************************************************************
* DEC6713.C	v1.00	     							                            *
* Copyright	2003 by SEED Electronic Technology Ltd.
* All rights reserved. Property of SEED Electronic Technology Ltd.			                *
* Designed by:	Hongshuai.Li								                                *
*********************************************************************************/ 
#include <csl.h>
#include <csl_mcasp.h>
#include <csl_i2c.h>
#include <csl_emif.h>
#include <csl_pll.h>
#include "DEC6713.h"


/*Local software delay function*/
static void PLLDelay(int Count)
{
    volatile int i = Count;
    while(i--);
}

/********************************************************************************\
\*DEC6713_init()	-Initialize DEC6713 board.
\*Parameters: NO.
\*		
\*Return:No.
\********************************************************************************/

void DEC6713_init()
{
	volatile Uint8 tempreg;
	/* Initealize the board APIs */
	EMIF_Config MyEMIFcfg0={
		0x30								|
		EMIF_FMKS(GBLCTL,NOHOLD,DISABLE)	|
		EMIF_FMKS(GBLCTL,EKEN,DEFAULT)		|
		EMIF_FMKS(GBLCTL,CLK1EN,DISABLE)	|
		EMIF_FMKS(GBLCTL,CLK2EN,DISABLE),
		
		EMIF_FMKS(CECTL,WRSETUP,DEFAULT)	|
		EMIF_FMKS(CECTL,WRSTRB,DEFAULT)		|
		EMIF_FMKS(CECTL,WRHLD,DEFAULT)		|
		EMIF_FMKS(CECTL,RDSETUP,DEFAULT)	|
		EMIF_FMKS(CECTL,TA,OF(2))			|
		EMIF_FMKS(CECTL,RDSTRB,DEFAULT)		|
		EMIF_FMKS(CECTL,MTYPE,SDRAM32)		|
		EMIF_FMKS(CECTL,RDHLD,DEFAULT),
		
		EMIF_FMKS(CECTL, WRSETUP, OF(2))    |
        EMIF_FMKS(CECTL, WRSTRB, DEFAULT)     |
        EMIF_FMKS(CECTL, WRHLD, OF(3))      |
        EMIF_FMKS(CECTL, RDSETUP, OF(0))    |
        EMIF_FMKS(CECTL, TA, OF(2))         |
        EMIF_FMKS(CECTL, RDSTRB, OF(8))     |
        EMIF_FMKS(CECTL, MTYPE, ASYNC16)    |
        EMIF_FMKS(CECTL, RDHLD, OF(2)),
        
        EMIF_FMKS(CECTL, WRSETUP, OF(2))    |
        EMIF_FMKS(CECTL, WRSTRB, DEFAULT)     |
        EMIF_FMKS(CECTL, WRHLD, OF(3))      |
        EMIF_FMKS(CECTL, RDSETUP, OF(0))    |
        EMIF_FMKS(CECTL, TA, OF(2))         |
        EMIF_FMKS(CECTL, RDSTRB, OF(8))     |
        EMIF_FMKS(CECTL, MTYPE, ASYNC16)    |
        EMIF_FMKS(CECTL, RDHLD, OF(2)),
		
		EMIF_FMKS(CECTL, WRSETUP, OF(5))    |
        EMIF_FMKS(CECTL, WRSTRB, OF(20))    |
        EMIF_FMKS(CECTL, WRHLD, OF(3))      |
        EMIF_FMKS(CECTL, RDSETUP, OF(15))    |
        EMIF_FMKS(CECTL, TA, OF(3))         |
        EMIF_FMKS(CECTL, RDSTRB, OF(20))    |
        EMIF_FMKS(CECTL, MTYPE, ASYNC16)    |
        EMIF_FMKS(CECTL, RDHLD, OF(3)),
        
        EMIF_FMKS(SDCTL,SDBSZ,4BANKS)		|
        EMIF_FMKS(SDCTL,SDRSZ,12ROW)		|
        EMIF_FMKS(SDCTL,SDCSZ,8COL)			|
        EMIF_FMKS(SDCTL,RFEN,ENABLE)		|
        EMIF_FMKS(SDCTL,INIT,YES)			|
        EMIF_FMKS(SDCTL,TRCD,OF(2))			|
        EMIF_FMKS(SDCTL,TRP,OF(2))			|
        EMIF_FMKS(SDCTL,TRC,OF(7)),
        
        //EMIF_FMKS(SDTIM,XRFR,OF(1))			|
        EMIF_FMKS(SDTIM, CNTR, OF(0))       |
        EMIF_FMKS(SDTIM, PERIOD, OF(1400)),
        
		EMIF_FMKS(SDEXT, WR2RD, OF(0))      |
        EMIF_FMKS(SDEXT, WR2DEAC, OF(2))    |
        EMIF_FMKS(SDEXT, WR2WR, OF(1))      |
        EMIF_FMKS(SDEXT, R2WDQM, OF(1))     |
        EMIF_FMKS(SDEXT, RD2WR, OF(0))      |
        EMIF_FMKS(SDEXT, RD2DEAC, OF(1))    |
        EMIF_FMKS(SDEXT, RD2RD, OF(0))      |
        EMIF_FMKS(SDEXT, THZP, OF(2))       |
        EMIF_FMKS(SDEXT, TWR, OF(2))        |
        EMIF_FMKS(SDEXT, TRRD, OF(0))       |
        EMIF_FMKS(SDEXT, TRAS, OF(5))       |
        EMIF_FMKS(SDEXT, TCL, OF(1))
        };
        
	/* Initialize PLL Registers */         
		/* Put PLL in bypass */
	PLL_bypass();
	PLLDelay(20);
	
		/* Reset PLL */
	PLL_reset();
	PLLDelay(20);
	
		/* Set main multiplier/divisor */
	/* below has been modified, from 18 to 24 */
 	PLL_RSET(PLLM, 24);                         // 25MHz  x 18 = 450MHz
    PLL_RSET(PLLDIV0, PLL_PLLDIV0_RMK(1, 0));  // 450MHz / 1 = 450MHz
    PLL_RSET(OSCDIV1, PLL_OSCDIV1_RMK(1, 4));  // 25MHz  / 5 = 5Mhz

    /* Set DSP clock */
    PLL_RSET(PLLDIV1, PLL_PLLDIV1_RMK(1, 1));  // 450MHz / 2 = 225MHz
    PLLDelay(20);
    
    /* Set peripheral clock */
    PLL_RSET(PLLDIV2, PLL_PLLDIV2_RMK(1, 2));  // 450MHz / 3 = 150MHz
    PLLDelay(20);
    
    /* Set EMIF clock */
    PLL_RSET(PLLDIV3, PLL_PLLDIV3_RMK(1, 4));  // 450MHz / 5 = 90MHz
    PLLDelay(20);
    
    	/* Take PLL out of reset */
    PLL_deassert();
    PLLDelay(1500);
    
    	/* Enalbe PLL */
    PLL_enable();
    PLLDelay(20);
   
   	/* Initialize EMIF */
    EMIF_config(&MyEMIFcfg0);
    
	/* Set CPLD registers to default state  */    
	tempreg = DEC6713_CTL_REG;
   	DEC6713_cpld_rset(tempreg, 0);   	
      	
}
/********************************************************************************\
\*Uint8 DEC6713_cpld_rget()		-Read CPLD register?
\*Parameters:
\*		regnum: The related register.

\*Return: The related register value.
\********************************************************************************/
   	
/* Read CPLD register(8bits) */
Uint16 DEC6713_cpld_rget(Uint8 regnum)
{
   	Uint16 *pdata;
   	static Uint16 RegVal = 0;
   	/* Return register value. */
   	pdata = (Uint16 *)(DEC6713_CPLD_BASE + regnum);
   	RegVal = *pdata;
   	return (RegVal & 0xff);
}
/********************************************************************************\
\*Uint8 DEC6713_cpld_rset()		-Write CPLD register.
\*Parameters:
\*		regnum: The related register
\*		regval:To be writen register value.

\*Return: No.
\********************************************************************************/

/* Write CPLD register(8bits). */
void DEC6713_cpld_rset(Uint8 regnum,Uint16 regval)
{
	Uint16 *pdata;
	pdata = (Uint16 *)(DEC6713_CPLD_BASE + regnum);
	*pdata = regval & 0xFF;
}
/********************************************************************************\
\*Uint8 DEC6713_wait()		-DEC6713 Delay function
\*Parameters:
\*		delay:Delay time.

\*Return:NO.
\********************************************************************************/
	/* Spin in a delay loop for delay iterations */
void DEC6713_wait(Uint32 delay)
{
    volatile Uint32 i, n;
    n = 0;
    for (i = 0; i < delay; i++)
    {
        n = n + 1;
    }
}

/********************************************************************************\
\* End of DEC6713.C *\
\********************************************************************************/
