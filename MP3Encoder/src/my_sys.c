/*
 * sys.c
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */
#include <my_sys.h>
#include <iva.h>
#include <csl.h>
#include <csl_pll.h>

void my_sys_init(void)
{
	matrice_sys_init(MAX_SIZE_REAL, MAX_SIZE_COMPLEX);
	iva_init(&iva_instance, FFT_LEN, SOURCE_NUM, SHIFT_SIZE, 0.5f, 0.02f);
	fft_init();
}

static void PLLDelay(int Count)
{
    volatile int i = Count;
    while(i--);
}

void pll_set(void)
{
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
}




