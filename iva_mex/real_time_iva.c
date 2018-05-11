/*************************************************************************
	> File Name: real_time_iva.c
	> Author: FanmingL 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/11
 ************************************************************************/

#include <stdio.h>
#include "mex.h"
#include "iva.h"
#include "matrix.h"
#include "fft.h"
#define MAX_REAL_BUFFER     2000
#define MAX_COMPLEX_BUFFER  2000

//arg0 rows of the screen
//arg1 cols of the screen
//arg2 iterators times
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])//in argc in argv out argc out argv
{
    int rows, cols, i, j, ii, jj, ff;
    double *in, *out;
    float beta, eta;
    int fft_length, shift_size;
    float **data_out;
    float *data_in;
    if (nrhs>=1)
    {
        rows=mxGetM(prhs[0]),cols=mxGetN(prhs[0]);//M rows N cols
        in=mxGetPr(prhs[0]);
        plhs[0]=mxCreateDoubleMatrix(rows,cols,mxREAL);
        out=mxGetPr(plhs[0]);
    }
    else{
        mexErrMsgTxt("Error: There are at least 1 input parameters required.");
    }
    if (nrhs>=2)
    {
        double *temp1 = mxGetPr(prhs[1]);
        fft_length = (int)(*temp1);
    }
    else
    {
        fft_length = 512;
    }
    if (nrhs>=3)
    {
        double *temp2 = mxGetPr(prhs[2]);
        shift_size = (int)(*temp2);
    }
    else
    {
        shift_size = fft_length / 4;
    }
    if (nrhs>=4)
    {
        double *temp3 = mxGetPr(prhs[3]);
        beta = (float)(*temp3);
    }
    else
    {
        beta = 0.5;
    }
    if (nrhs>=5)
    {
        double *temp4 = mxGetPr(prhs[4]);
        eta = (float)(*temp4);
    }
    else
    {
        eta = 0.1f;
    }
    
    matrice_sys_init(MAX_REAL_BUFFER, MAX_COMPLEX_BUFFER);
    iva_init(&iva_instance, fft_length, cols, shift_size, beta, eta);
    data_in = malloc(cols * sizeof(float));
    ii = 0;
    for (i = 0; i < rows; i++)
    {
        for(j = 0; j < cols; j++)
        {
            data_in[j] =*(in + j * rows + i);
        }
        data_out = iva_step(&iva_instance, data_in);
        if (data_out != NULL)
        {
            ff = ii;
            for (;ii<=i;ii++)
            {
                for (jj = 0; jj < cols; jj++)
                {
                    *(out + rows * jj + ii) = data_out[ii - ff][jj];
                }
            }
        }
    }
    
    free(data_in);
    iva_exit(&iva_instance);
    matrice_sys_exit();
}
