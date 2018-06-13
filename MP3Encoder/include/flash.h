/*
 * flash.h
 *
 *  Created on: 2018-6-11
 *      Author: erdou
 */

#ifndef FLASH_H_
#define FLASH_H_

#define		FLASH_UL1	0xAA
#define		FLASH_UL2	0x55
#define		FLASH_UL3	0x80
#define		FLASH_UL4	0xAA
#define		FLASH_UL5	0x55
#define		FLASH_SECTOR_UL6	0x30
#define		FLASH_CHIP_UL6		0x10
#define		FLASH_PROGRAM	0xA0
#define		SECTOR_SIZE	0x0800
#define		BLOCK_SIZE	0x8000
#define		CHIP_SIZE	0x40000

volatile Uint16 *FLASH_5555 = (volatile Uint16 *) (0x90000000+(0x5555<<1));
volatile Uint16 *FLASH_2AAA = (volatile Uint16 *) (0x90000000+(0x2AAA<<1));
/********************************************************************************/
Uint32 Flash_Erase(Uint32 addr,Uint16 type);
void Flash_Readm(Uint32 addr,Uint16 *ptr,Uint32 length);
Uint32 Flash_Reads(Uint32 addr);
void Flash_Writem(Uint32 addr,Uint16 *ptr,Uint32 length);
void Flash_Writes(Uint32 addr,Uint16 data);



#endif /* FLASH_H_ */
