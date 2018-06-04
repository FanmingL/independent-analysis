/*
 * sys.h
 *
 *  Created on: 2018-6-4
 *      Author: erdou
 */

#ifndef SYS_H_
#define SYS_H_
#include <csl.h>
#include <iva.h>
#include <DEC6713.h>
#define MAX_SIZE_REAL 50
#define MAX_SIZE_COMPLEX 400
#define SOURCE_NUM 2                //default source number is 2
#define FFT_LEN 256
#define SHIFT_SIZE (FFT_LEN/2)
void sys_init(void);


#endif /* SYS_H_ */
