/*************************************************************************
	> File Name: fft.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include<stdio.h>
#include "fft.h"
#include "matrix.h"

/* for both real and complex signal */
MatcP fft(MatcP signal, MatcP fft_signal, int fft_length)
{
    
    return fft_signal;
}

/* for both real and complex signal */
MatcP ifft(MatcP fft_signal, MatcP signal, int fft_length)
{
    
    return signal;
}


/* only for real signal */
MatcP fft_shift(MatcP fft_singal_full, MatcP fft_signal_half)
{
    
    return fft_signal_half;
}

/* only for real signal */
MatcP fft_ishift(MatcP fft_singal_half, MatcP fft_signal_full)
{
    
    return fft_signal_full;
}

/* only for real signal, return length is fft_length/2 */
MatcP fft_real(MatfP signal, MatcP fft_signal, int fft_length)
{
    
    return fft_signal;
}

/* only for real signal, return length is fft_length */
MatfP ifft_real(MatcP fft_signal, MatfP signal, int fft_length)
{
    
    return signal;
}

/* only for real signal */
MatfP hanning_window_get(int fft_length)
{
    int i;
    MatfP hanning_win = matf_zeros(fft_length, 1);
    for (i = 0; i < fft_length; i++)
    {
        hanning_win->data[i][0] = 1. / 2. * ( 1 + cosf( M_TWOPI * ( i ) / ( fft_length - 1 ) ) );
    }
    return hanning_win;
}

/*only for real singal*/
MatfP add_win_real(MatfP win, MatfP signal, MatfP signal_out)
{
    matf_row_mul(signal, win, signal_out);
    return signal_out;
}


