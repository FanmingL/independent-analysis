/*************************************************************************
	> File Name: iva.h
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2019/05/09
 ************************************************************************/

#ifndef _IVA_H
#define _IVA_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "matrix.h"
#include "fft.h"

#ifndef SOURCE_NUM 
	#define SOURCE_NUM 2				//default source number is 2
#endif
#ifndef FFT_LEN
	#define FFT_LEN 256
#endif

typedef struct
{
	float data_input[SOURCE_NUM][FFT_LEN];
	float data_sperate[SOURCE_NUM][FFT_LEN];
	


}iva_t;
extern iva_t iva_instance;

void data_prepare(float **data);
#endif
