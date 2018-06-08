/*************************************************************************
	> File Name: fft.h
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#ifndef _FFT_H
#define _FFT_H
#include <matrix.h>
#include <math.h>
#define NOT2POW(a) (((a)-1)&(a)||(a)<=0)
MatcP fft(MatcP signal, MatcP fft_signal);
MatcP ifft(MatcP fft_signal, MatcP signal);
MatcP fft_shift(MatcP fft_singal_full, MatcP fft_signal_half);
MatcP fft_ishift(MatcP fft_singal_half, MatcP fft_signal_full);
MatcP fft_real(MatfP signal, MatcP fft_signal);
MatfP ifft_real(MatcP fft_signal, MatfP signal);
MatfP hanning_window_get(int fft_length);
MatfP add_win_real(MatfP win, MatfP signal, MatfP signal_out);
void fft_init(void);
#endif
