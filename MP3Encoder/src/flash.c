/*
 * flash.c
 *
 *  Created on: 2018-6-11
 *      Author: erdou
 */
#include <stdio.h>
#include <csl.h>
#include <csl_irq.h>
#include <csl_chip.h>
#include <csl_emif.h>
#include <csl_irq.h>
#include <flash.h>
#include <math.h>
volatile Uint16 obeserve;
volatile Uint32 addr_observe;
volatile Uint16 data_observe;
Uint32 Flash_Erase(Uint32 addr,Uint16 type)
{
	Uint32 i,j;
	addr_observe = addr;
	data_observe = type;
	*FLASH_5555 = FLASH_UL1;	//first
	*FLASH_2AAA = FLASH_UL2;	//second
	*FLASH_5555 = FLASH_UL3;	//third
	*FLASH_5555 = FLASH_UL4;
	*FLASH_2AAA = FLASH_UL5;
	switch(type)
	{
		case 0x50:		//block erase
			*(Uint16 *)(addr_observe) = data_observe;
			while((*(Uint16 *)(addr_observe) & 0x80) != 0x80)obeserve =  (*(Uint16 *)addr_observe);
			for(i = 0; i < BLOCK_SIZE; i++)
			{
				if(*(Uint16 *)((addr_observe) + i) != 0xffff)
				{
					j = 0;
					break;
				}
			}
			j = 1;
			break;

		case 0x30:		//sector erase
			(*(Uint16 *)addr_observe) = data_observe;
			while(((*(Uint16 *)addr_observe) & 0x80) != 0x80)obeserve =  (*(Uint16 *)addr_observe);
			for(i = 0; i < SECTOR_SIZE; i++)
			{
				if((*(Uint16 *)(addr_observe + i)) != 0xffff)
				{
					j = 0;
					break;
				}
			}
			j = 1;
			break;

		case 0x10:		//chip erase
	//	for(;;)
	//	{
			*FLASH_5555 = data_observe;
	//	}
			while((*FLASH_5555 & 0x80) != 0x80);
			for(i = 0; i < CHIP_SIZE; i++)
			{
				if(*(Uint16 *)(addr_observe + i) != 0xffff)
				{
					j = 0;
					break;
				}
			}
			j = 1;
			break;

		default:
			break;
	}
	return (j);
}

/********************************************************************************\
\*  Write a single data. *\
\********************************************************************************/
void Flash_Writes(Uint32 addr,Uint16 data)
{
	//Uint16 TempData=0;
	*FLASH_5555 = FLASH_UL1;
	*FLASH_2AAA = FLASH_UL2;
	*FLASH_5555 = FLASH_PROGRAM;
    //for(;;)
    //{
	addr_observe = addr;
	data_observe = data;
	//TempData = *(Uint16 *)(addr);
	//}
	//TempData = *(Uint16 *)(addr);
	*(Uint16 *)addr_observe = data_observe;
	while(1)
	{
		obeserve = *(Uint16 *)addr_observe;
		if (obeserve == data_observe)break;
	}
}

/********************************************************************************\
\* Write the certain length data. *\
\********************************************************************************/
void Flash_Writem(Uint32 addr,Uint16 *ptr,Uint32 length)
{
	Uint32 i;
	for(i  = 0; i < length; i++)
	{
	//	for(;;)
	//	{
		Flash_Writes(addr+2*i,*(ptr+i));
	//	}
	}
}

/********************************************************************************\
\* Read a single data. *\
\********************************************************************************/
Uint32 Flash_Reads(Uint32 addr)
{
	addr_observe = addr;
	return (*(Uint16 *)addr_observe);
}

/********************************************************************************\
\* Read the certain length data. *\
\********************************************************************************/
void Flash_Readm(Uint32 addr,Uint16 *ptr,Uint32 length)
{
	Uint32 i;
	for(i = 0; i < length; i++)
	{
		*(ptr + i) = Flash_Reads(addr+2*i);
	}
}

/********************************************************************************\
\* End of DEC6713_FLASH.C *\
\********************************************************************************/



