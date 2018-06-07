/*************************************************************************
	> File Name: main.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include <stdio.h>
#include "iva.h"
#include "time.h"
#define LEN 5
#define MAX_SIZE_REAL 1100
#define MAX_SIZE_COMPLEX 1100
#ifndef SOURCE_NUM
#define SOURCE_NUM 2                //default source number is 2
#endif
#ifndef FFT_LEN
#define FFT_LEN 512
#endif
#define SHIFT_SIZE (FFT_LEN / 4)

int main(int argc, char** argv)
{
    FILE *fp, *fp2;
    float buffer[640000][2];
    float **out_signal;
    clock_t start,end, period = 0;
    matrice_sys_init(MAX_SIZE_REAL, MAX_SIZE_COMPLEX, SOURCE_NUM);
    iva_init(&iva_instance, FFT_LEN, SOURCE_NUM, SHIFT_SIZE, 0.5, 0.1);
    fft_init(FFT_LEN, SOURCE_NUM);
    
    fp=fopen("/Users/erdou/Documents/MacToWin/Share/audio/data.prototxt","r");
    fp2=fopen("/Users/erdou/Documents/GitHub/independent-analysis/iva_c/out.prototxt","w");
    if ( fp )
    {
        for (int  i=0;i<595957;i++ )
        {
            fscanf(fp, "%f, %f", &buffer[i][0], &buffer[i][1]);
            //buffer[i][0]  /= 100;
            //buffer[i][1] /= 100;
        }
        fclose(fp);
    }
    start = clock();
    for (int i = 0; i < 595957; i++)
    {
        out_signal =iva_step(&iva_instance, buffer[i]);
        if (out_signal != NULL)
        {
            for (int ii=0;ii<iva_instance.shift_size;ii++)
            {
                fprintf(fp2, "%.4f %.4f\n", out_signal[ii][0], out_signal[ii][1]);
            }
        }
    }
    fclose(fp2);
    end = clock();
    
    period+=end-start;
    printf("time=%f\n",(float)(period)/CLOCKS_PER_SEC);
    iva_exit(&iva_instance);
    matrice_sys_exit();
	return 0;
}


