/********************************************************************************\
\*DEC6713.H	  	v1.00	     							                            *
* Copyright	2004 by SEED Electronic Technology Ltd.
* All rights reserved. Property of SEED Electronic Technology Ltd.			                *
* Designed by:	Hongshuai.Li								                                *
\********************************************************************************/
#ifndef DEC6713_
#define DEC6713_

#ifdef __cplusplus
extern "C" {
#endif

#include <csl.h>
#include <csl_i2c.h>


/* CPLD address definitions */
#define DEC6713_CPLD_BASE    0xB0000000

/* CPLD Register Indices */
#define DEC6713_CTL_REG    	 	0<<1
#define DEC6713_INTSTAT_REG		1<<1
#define DEC6713_STAT_REG		2<<1
#define DEC6713_WDT_REG			3<<1

I2C_Handle DEC6713_I2C_hI2C;
/* Initialize all board APIs */
void DEC6713_init();

/* Read an 8-bit value from a CPLD register */
Uint16 DEC6713_cpld_rget(Int16 regnum);

/* Write an 8-bit value to a CPLD register */
void DEC6713_cpld_rset(Int16 regnum, Uint16 regval);

/* Spin in a delay loop for delay iterations */
void DEC6713_wait(Uint32 delay);

#ifdef __cplusplus
}
#endif

#endif
