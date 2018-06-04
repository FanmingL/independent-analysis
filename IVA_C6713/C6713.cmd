/****************************************************************************/
/*  C6713.cmd                                                               */
/*  Copyright (c) 2010 Texas Instruments Incorporated                       */
/*  Author: Rafael de Souza                                                 */
/*                                                                          */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on an TMS320C6713        */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/
-heap 0x15000			/* fft length up to 512 */
-stack 0x2000			/* avoid fatal error */

MEMORY
{
    IRAM     o = 0x00000000  l = 0x00030000  /* 192kB - Internal RAM */
    L2RAM    o = 0x00030000  l = 0x00010000  /* 64kB - Internal RAM/CACHE */
    EMIFCE0  o = 0x80000000  l = 0x10000000  /* SDRAM in 6713 DSK */
    EMIFCE1  o = 0x90000000  l = 0x10000000  /* Flash/CPLD in 6713 DSK */
    EMIFCE2  o = 0xA0000000  l = 0x10000000  /* Daughterboard in 6713 DSK */
    EMIFCE3  o = 0xB0000000  l = 0x10000000  /* Daughterboard in 6713 DSK */
}

SECTIONS
{
    .text          >  IRAM
    .stack         >  IRAM
    .bss           >  IRAM
    .cio           >  IRAM
    .const         >  IRAM
    .data          >  IRAM
    .switch        >  IRAM
    .sysmem        >  IRAM				/* heap & stack are store in Internal RAM */
    .far           >  IRAM
    .args          >  IRAM
    .ppinfo        >  IRAM
    .ppdata        >  IRAM
  
    /* COFF sections */
    .pinit         >  IRAM
    .cinit         >  IRAM
  
    /* EABI sections */
    .binit         >  IRAM
    .init_array    >  IRAM
    .neardata      >  IRAM
    .fardata       >  IRAM
    .rodata        >  IRAM
    .c6xabi.exidx  >  IRAM
    .c6xabi.extab  >  IRAM
}
