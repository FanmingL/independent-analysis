/*************************************************************************
	> File Name: fft.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include <stdio.h>
#include <fft.h>
#include <my_sys.h>
#include <matrix.h>
#include <mathf.h>
Matc* signal_temp1, *signal_temp2, *signal_temp3;
Matc* fft_temp1, *fft_temp2, *fft_temp3;
float cos_buffer[FFT_LEN],sin_buffer[FFT_LEN];
unsigned int ex = 0;
void fft_init(void)
{
	unsigned int t = FFT_LEN;
	NEW_MULTI_MAT_COMPLEX(signal_temp1, FFT_LEN,  SOURCE_NUM, 1);
	NEW_MULTI_MAT_COMPLEX(signal_temp2, FFT_LEN,  SOURCE_NUM, 1);
	NEW_MULTI_MAT_COMPLEX(signal_temp3, FFT_LEN,  SOURCE_NUM, 1);
	NEW_MULTI_MAT_COMPLEX(fft_temp2, FFT_LEN,  SOURCE_NUM, 1);
	NEW_MULTI_MAT_COMPLEX(fft_temp3, FFT_LEN,  SOURCE_NUM, 1);
	for(;(!(t&1));t>>=1) ex++;
	for (t = 0; t < FFT_LEN; t++)
	{
		cos_buffer[t] = cosf(-M_PI*t/FFT_LEN);
		sin_buffer[t] = sinf(-M_PI*t/FFT_LEN);
	}
}

/* for both real and complex signal */
MatcP fft(MatcP signal, MatcP fft_signal)
{
    unsigned int fft_length = signal->rows;
    unsigned int t=fft_length;
    unsigned int i,j,k,col;
    unsigned int cols = signal->cols;
    unsigned int index;
    float tr,ti,rr,ri,yr,yi;
#if ENABLE_ASSERT
    assert(!NOT2POW(fft_length));
#endif

#if ENABLE_ASSERT
    if (fft_signal->rows != signal->rows || fft_signal->cols != signal->cols)
    {
        matc_reallocate(fft_signal, 1, signal->rows, fft_signal->cols, 1);
    }
#endif
    for (col = 0; col < cols; col++)
    {
        for(i=0;i<fft_length;i++){
            k=i;
            j=0;
            t=ex;
            while((t--)>0){
                j<<=1;
                j|=k&1;
                k>>=1;
            }
            if(j>=i){
                fft_signal->data[i][col] = signal->data[j][col];
                fft_signal->data[j][col] = signal->data[i][col];
            }
        }
        for(i=0;i<ex;i++){
            t=1<<i;
            for(j=0;j<fft_length;j+=t<<1){
                for(k=0;k<t;k++){
                    //ti=-M_PI*k/t;
                    //rr=cosf(ti);
                    //ri=sinf(ti);
                	index = k * fft_length / t;
                	rr = cos_buffer[index];
                	ri = sin_buffer[index];

                    tr=fft_signal->data[j+k+t][col].real;
                    ti=fft_signal->data[j+k+t][col].imag;
                    
                    yr=rr*tr-ri*ti;
                    yi=rr*ti+ri*tr;
                    
                    tr=fft_signal->data[j+k][col].real;
                    ti=fft_signal->data[j+k][col].imag;
                    
                    fft_signal->data[j+k][col].real=tr+yr;
                    fft_signal->data[j+k][col].imag=ti+yi;
                    fft_signal->data[j+k+t][col].real=tr-yr;
                    fft_signal->data[j+k+t][col].imag=ti-yi;
                }
            }
        }
    }
    return fft_signal;
}


/* for both real and complex signal */
MatcP ifft(MatcP fft_signal, MatcP signal)
{
    matc_copy(fft_signal, signal_temp1);
    matc_set_conj(signal_temp1);
    fft(signal_temp1, signal);
    matc_set_conj(signal);
    matc_real_mul(signal, 1./fft_signal->rows, signal);
    return signal;
}


/* only for real signal, assuming that fft_length is even number  */
MatcP fft_shift(MatcP fft_signal_full, MatcP fft_signal_half)
{
#if ENABLE_ASSERT
    if (fft_signal_full->rows / 2 + 1 != fft_signal_half->rows || fft_signal_full->cols != fft_signal_half->cols)
    {
        matc_reallocate(fft_signal_half, 1, fft_signal_full->rows / 2 + 1, fft_signal_full->cols, 1);
    }
#endif
    //int row;

    memcpy(fft_signal_half->data[0], fft_signal_full->data[0],(fft_signal_full->rows / 2 + 1)* fft_signal_half->cols * sizeof(c_num));

   /* for (row = 0; row < fft_signal_full->rows / 2 + 1; row++)
    {
        memcpy(fft_signal_half->data[row], fft_signal_full->data[row], fft_signal_half->cols * sizeof(c_num));
    }*/
    return fft_signal_half;
}

/* only for real signal */
MatcP fft_ishift(MatcP fft_signal_half, MatcP fft_signal_full)
{
#if ENABLE_ASSERT
    if (fft_signal_full->rows / 2 + 1 != fft_signal_half->rows || fft_signal_full->cols != fft_signal_half->cols)
    {
        matc_reallocate(fft_signal_full, 1, (fft_signal_half->rows - 1) * 2 , fft_signal_half->cols, 1);
    }
#endif
    int row = fft_signal_full->rows / 2 + 1, col;
    memcpy(fft_signal_full->data[0], fft_signal_half->data[0],(fft_signal_full->rows / 2 + 1)* fft_signal_half->cols * sizeof(c_num));
/*
    for (row = 0; row < fft_signal_full->rows / 2 + 1; row++)
    {
        memcpy(fft_signal_full->data[row], fft_signal_half->data[row], fft_signal_half->cols * sizeof(c_num));
    }*/
    for ( ;row < fft_signal_full->rows; row++)
    {
        for (col = 0; col < fft_signal_full -> cols; col ++)
        {
            c_conj(&(fft_signal_half->data[fft_signal_full->rows - row][col]), &(fft_signal_full->data[row][col]));
        }
    }
    return fft_signal_full;
}

/* only for real signal, return length is fft_length/2 + 1 */
MatcP fft_real(MatfP signal, MatcP fft_signal)
{
    matf_convert2com(signal, signal_temp2);
    fft(signal_temp2, fft_signal);

    //fft_shift(fft_temp2, fft_signal);

    return fft_signal;
}

/* only for real signal, return length is fft_length */
MatfP ifft_real(MatcP fft_signal, MatfP signal)
{
    fft_ishift(fft_signal, fft_temp3);
    ifft(fft_temp3, signal_temp3);
    matc_convert2real(signal_temp3, signal);
    return signal;
}

/* only for real signal */
MatfP hanning_window_get(int fft_length)
{
    NEW_MAT_REAL(hanning_win, fft_length, 1);
    matf_set_hanning(hanning_win);
    return hanning_win;
}

/*only for real singal*/
MatfP add_win_real(MatfP win, MatfP signal, MatfP signal_out)
{
    matf_row_mul(signal, win, signal_out);
    return signal_out;
}


