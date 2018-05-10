/*************************************************************************
	> File Name: main.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include <stdio.h>
#include "iva.h"
#include "time.h"
#include "data.h"
#define LEN 5
#define MAX_SIZE_REAL 500
#define MAX_SIZE_COMPLEX 500
#ifndef SOURCE_NUM
#define SOURCE_NUM 2                //default source number is 2
#endif
#ifndef FFT_LEN
#define FFT_LEN 256
#endif


//…calculating…


int main(int argc, char** argv)
{
    FILE *fp;
    float temp[1][2];
    clock_t start,end;
    matrice_sys_init(MAX_SIZE_REAL, MAX_SIZE_COMPLEX);
    iva_init(&iva_instance, FFT_LEN, SOURCE_NUM, FFT_LEN/2, 0.5f, 0.001f);
    start = clock();
    if ( fp=fopen("/Users/erdou/Documents/GitHub/independent-analysis/iva_c/data.prototxt","r") )
    {
        for (int  i=0;i<320000;i++ )
        {
            fscanf(fp, "%f, %f", &temp[0][0], &temp[0][1]);
            if (iva_step(&iva_instance, temp[0]))
            {
             //   print_matf(iva_instance.data_sperated, 1);
            }
        }
        fclose(fp);
    }
    end = clock();
    printf("time=%f\n",(float)(end-start)/CLOCKS_PER_SEC);
    iva_exit(&iva_instance);
    matrice_sys_exit();
	return 0;
}


