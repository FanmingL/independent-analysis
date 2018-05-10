/*************************************************************************
	> File Name: main.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include<stdio.h>
#include "iva.h"
#include "time.h"

#define LEN 5
#define FFT_LEN_ 256
#define MAX_SIZE_REAL 100
#define MAX_SIZE_COMPLEX 100


int main(int argc, char** argv)
{
    matrice_sys_init(MAX_SIZE_REAL, MAX_SIZE_COMPLEX);
    NEW_MAT_REAL(A, LEN, LEN);
    NEW_MAT_REAL(B, LEN, LEN);
    NEW_MAT_REAL(C, LEN, LEN);
    
    NEW_MAT_COMPLEX(E, LEN, LEN);
    NEW_MAT_COMPLEX(F, LEN, LEN);
    NEW_MAT_COMPLEX(G, LEN, LEN);
    
    
    matf_set_rand(A, 40);
    matf_inverse(A, B);
    matf_mul(A, B, C);
    print_matf(C, 1);
    
    matc_set_rand(E, 40);
    print_matc(E, 1);
    matc_inverse(E, F);
    matc_mul(E, F, G);
    print_matc(G, 1);
    
    NEW_MAT_REAL(signal, FFT_LEN_, 2);
    NEW_MAT_REAL(H, FFT_LEN_, 2);
    NEW_MAT_REAL(WIN, FFT_LEN_, 1);
    
    NEW_MAT_REAL(signal_recon, FFT_LEN_, 2);
    NEW_MAT_COMPLEX(signal_fft, FFT_LEN_ / 2 + 1, 2);
    
    matf_set_rand(signal, 41);
    matf_set_all_num(H, 20);
    matf_set_hamming(WIN);
    matf_sub(signal, H, signal);
    matf_row_mul(signal, H, signal);
    print_matf(signal, 1);
    
    fft_real(signal, signal_fft);
    
    //print_matc(signal_fft, 1);
    
    ifft_real(signal_fft, signal_recon);
    print_matf(signal_recon, 1);

    
    matrice_sys_exit();
	return 0;
}
