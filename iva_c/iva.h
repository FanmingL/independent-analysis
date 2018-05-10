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



typedef struct
{
    Matf *data_input;
    Matf *data_reconstruct;
    Matf *data_overlapadd;
    Matf *data_sperated;
    Matc *data_estimate;
    int fft_length;
    int source_num;
    int shift_size;
    int inited_flag;
    float beta;
    float eta;
    Matf *window;
    Matc *unmix_matrix;
    Matc *norm_matrix;
    Matc *R_matrix;
    Matc *Phi;
    Matc *data_fft;
    Matf *data_buffer;
    Matf *ksi;
    int data_buffer_count;
    int effective_length;
    Matf *data_windowed;
}iva_t;
typedef iva_t* iva_tP;
extern iva_t iva_instance;

void iva_init(iva_tP iva_it, int fft_length, int source_num,
              int shift_size, float beta, float eta);
int iva_step(iva_tP iva_it, float *data);
void iva_exit(iva_tP iva_it);
void iva_process(iva_tP iva_it);
void iva_frame_shift(int shift_size, int fft_length, int source_num, float **data);
void iva_frame_cat(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out);
void iva_frame_split(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out);
void matc_step2(MatcP signal_obs, MatcP unmix, MatcP out, int fft_length, int source_num);
void matc_R_matrix(MatcP esti, MatcP Phi, MatcP R, int fft_length, int source_num);
void matc_unmix_matrix_update(MatcP G, MatfP Ksi, MatcP R, int fft_length, int source_num, float eta);
void matc_ksi_update(MatfP Ksi, MatcP fft_data, int fft_length, int source_num, float beta);
void est_update(MatcP est, MatcP norm, int fft_length, int source_num);
#endif
