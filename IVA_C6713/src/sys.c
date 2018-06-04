/*
 * sys.c
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */
#include <sys.h>

extern far void vectors();
void sys_init(void)
{
	/* csl init */
	CSL_init();
	/* board init */
	DEC6713_init();
	/* algorithm init */
	matrice_sys_init(MAX_SIZE_REAL, MAX_SIZE_COMPLEX);
	iva_init(&iva_instance, FFT_LEN, SOURCE_NUM, SHIFT_SIZE, 0.5f, 0.11f);
	fft_init();
	/* load IRQ vector table */
	IRQ_setVecs(vectors);
  	IRQ_globalEnable();
  	IRQ_nmiEnable();
}




