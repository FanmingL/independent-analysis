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
    Matc *Phi;
    Matc *data_fft;
    Matf *data_buffer;
    Matf *ksi;
    int data_buffer_count;
    int effective_length;
    Matf *data_windowed;
    Matf* temp_process;
    Matc *gradient_unmix,
    *temp1_unmix,
    *R_temp_unmix,
    *temp_est,
    *temp_norm,
    *temp_c1_unmix,
    *temp_c2_unmix,
    *temp_c3_unmix;
    float div_temp;
}iva_t;
typedef iva_t* iva_tP;
extern iva_t iva_instance;

void iva_init(iva_tP iva_it, int fft_length, int source_num,
              int shift_size, float beta, float eta);
float** iva_step(iva_tP iva_it, float *data);
void iva_exit(iva_tP iva_it);
void iva_process(iva_tP iva_it);
void iva_frame_shift(int shift_size, int fft_length, int source_num, float **data);
void iva_frame_cat(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out);
void iva_frame_cat_zeros(int shift_size, int fft_length, int source_num, float ** data_out);
void iva_frame_split(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out);
void iva_estimate(iva_tP iva_it, MatcP signal_obs, MatcP unmix, MatcP out, int fft_length, int source_num);
void iva_R_matrix(MatcP esti, MatcP Phi, MatcP R, int fft_length, int source_num);
void iva_unmix_matrix_update(iva_tP iva_it, MatcP G, MatfP Ksi, MatcP Phi, MatcP esti, int fft_length, int source_num, float eta);
void iva_ksi_update(iva_tP iva_it, MatfP Ksi, MatcP fft_data, int fft_length, int source_num, float beta);
void iva_est_update(iva_tP iva_it, MatcP est, MatcP norm, int fft_length, int source_num);
void iva_norm_matrix(iva_tP iva_it, MatcP unmix, MatcP norm, int fft_length, int source_num);
#endif
