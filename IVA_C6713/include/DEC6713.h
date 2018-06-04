/********************************************************************************\
\*DEC6713.H	  	v1.00	     							                            *
* Copyright	2004 by SEED Electronic Technology Ltd.
* All rights reserved. Property of SEED Electronic Technology Ltd.			                *
* Designed by:	Hongshuai.Li								                                *
\********************************************************************************/
#ifndef DEC6713_
#define DEC6713_

#include <csl.h>

/* CPLD address definitions */
//#define DEC6713_CPLD_BASE    0x90100000		// For 32bits interface
//#define DEC6713_CPLD_BASE	 0x90080000		// For 8bits interface
#define DEC6713_CPLD_BASE    0xb0000000
/* CPLD Register Indices for 8bits interface*/
//#define DEC6713_CTL_REG    	 	0;
//#define DEC6713_INTSTAT_REG		1;
//#define DEC6713_STAT_REG			2;
//#define DEC6713_WDT_REG			3;

/* CPLD Register Indices for 16bits interface*/
#define DEC6713_CTL_REG    	 	0<<1;
#define DEC6713_INTSTAT_REG		1<<1;
#define DEC6713_STAT_REG		2<<1;
#define DEC6713_WDT_REG			3<<1;

/* Initialize all board APIs */
void DEC6713_init();

/* Read an 8-bit value from a CPLD register */
Uint16 DEC6713_cpld_rget(Uint8 regnum);

/* Write an 8-bit value to a CPLD register */
void DEC6713_cpld_rset(Uint8 regnum, Uint16 regval);

/* Spin in a delay loop for delay iterations */
void DEC6713_wait(Uint32 delay);

#endif
