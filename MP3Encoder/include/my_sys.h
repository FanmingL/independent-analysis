/*
 * sys.h
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */

#ifndef _MY_SYS_H_
#define _MY_SYS_H_
#include <csl.h>
typedef struct
{
	float beta;
	float eta;
	float ratio;
	Uint16 save_flag;
	Uint16 run_flag;
}iva_configure_t;
//#include <iva.h>
#define MAX_SIZE_REAL 100
#define MAX_SIZE_COMPLEX 400
#define SOURCE_NUM 2                //default source number is 2
#define FFT_LEN 128
#define SHIFT_SIZE (FFT_LEN/2)
void my_sys_init(void);
void pll_set(void);
extern iva_configure_t iva_configure_instance;
#endif /* MY_SYS_H_ */
