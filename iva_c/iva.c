/*************************************************************************
	> File Name: iva.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include <stdio.h>
#include "iva.h"

iva_t iva_instance;

void iva_init(iva_tP iva_it, int fft_length, int source_num, int shift_size, float beta, float eta)
{
    int i=0;
    iva_it->fft_length      = fft_length;
    iva_it->source_num      = source_num;
    iva_it->inited_flag     = 1;
    iva_it->beta            = beta;
    iva_it->eta             = eta;
    iva_it->shift_size      = shift_size;
    iva_it->effective_length = fft_length / 2 + 1;
    iva_it->data_buffer_count = 0;
    NEW_MULTI_MAT_COMPLEX(iva_it->unmix_matrix, source_num, source_num, iva_it->effective_length);
    NEW_MULTI_MAT_COMPLEX(iva_it->R_matrix, source_num, source_num, iva_it->effective_length);
    NEW_MULTI_MAT_COMPLEX(iva_it->norm_matrix, source_num, source_num, iva_it->effective_length);
    NEW_MULTI_MAT_REAL(iva_it->data_input, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_sperated, shift_size, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_overlapadd, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_reconstruct, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->window, fft_length, 1, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_buffer, shift_size, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->data_fft, iva_it->effective_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_windowed, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->ksi, fft_length, 1, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->data_estimate, iva_it->effective_length, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->Phi, iva_it->effective_length, source_num, 1);
    
    matf_set_hanning(iva_it->window);
    matf_set_all_num(iva_it->ksi, 1);
    for (i = 0; i<fft_length; i++)
    {
        matc_set_eye(iva_it->unmix_matrix + i);
    }
}

void iva_exit(iva_tP iva_it)
{
    
}

int iva_step(iva_tP iva_it, float *data)
{
    memcpy(iva_it->data_buffer->data[iva_it->data_buffer_count++], data,
           iva_it->source_num*sizeof(float));
    if ((iva_it->data_buffer_count) == iva_it->shift_size)
    {
        iva_it->data_buffer_count = 0;
        iva_frame_shift(iva_it->shift_size, iva_it->fft_length, iva_it->source_num, iva_it->data_input->data);
        iva_frame_cat(iva_it->shift_size, iva_it->fft_length, iva_it->source_num, iva_it->data_buffer->data, iva_it->data_input->data);
        iva_process(iva_it);
        return 1;
    }
    return 0;
}

void iva_process(iva_tP iva_it)
{
    int i=0;
    MatfP temp = matf_zeros(1, iva_it->source_num);
    MatcP temp1 = matc_zeros(iva_it->source_num, iva_it->source_num);
    MatcP temp2 = matc_zeros(iva_it->source_num, iva_it->source_num);
    matf_row_mul(iva_it->data_input, iva_it->window, iva_it->data_windowed);
    fft_real(iva_it->data_windowed, iva_it->data_fft);
    
    matc_step2(iva_it->data_fft, iva_it->unmix_matrix, iva_it->data_estimate, iva_it->effective_length, iva_it->source_num);
    for (i = 0; i < iva_it->effective_length; i++)
    {
        matc_inverse((iva_it->unmix_matrix + i), temp1);
        matc_select_diag(temp1, temp2);
        matc_copy(temp2, (iva_it->norm_matrix + i));
    }
    matc_metrix(iva_it->data_estimate, temp);
    matc_real_col_div(iva_it->data_estimate, temp, iva_it->Phi);
    matc_R_matrix(iva_it->data_estimate, iva_it->Phi, iva_it->R_matrix, iva_it->effective_length, iva_it->source_num);
    matc_unmix_matrix_update(iva_it->unmix_matrix, iva_it->ksi, iva_it->R_matrix, iva_it->effective_length, iva_it->source_num, iva_it->eta);
    matc_ksi_update(iva_it->ksi, iva_it->data_fft, iva_it->effective_length, iva_it->source_num, iva_it->beta);
    est_update(iva_it->data_estimate, iva_it->norm_matrix, iva_it->effective_length, iva_it->source_num);
    
    ifft_real(iva_it->data_estimate, iva_it->data_reconstruct);
    matf_add(iva_it->data_overlapadd, iva_it->data_reconstruct, iva_it->data_overlapadd);
    iva_frame_split(iva_it->shift_size, iva_it->fft_length, iva_it->source_num, iva_it->data_overlapadd->data, iva_it->data_sperated->data);
    iva_frame_shift(iva_it->shift_size, iva_it->fft_length, iva_it->source_num,
                    iva_it->data_overlapadd->data);
    
    free_matf(temp, 1);
    free_matc(temp1, 1);
    free_matc(temp2, 1);
}

void iva_frame_shift(int shift_size, int fft_length, int source_num, float **data)
{
    int i=0;
    for (; i < fft_length - shift_size; i++)
    {
        memcpy(data[i], data[i+shift_size], source_num * sizeof(float));
    }
}

void iva_frame_cat(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out)
{
    int i=fft_length - shift_size;
    for (; i < fft_length; i++)
    {
        memcpy(data_out[i], data_in[i + shift_size - fft_length], source_num * sizeof(float));
    }
}

void iva_frame_split(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out)
{
    int i=0;
    for (; i < shift_size; i++)
    {
        memcpy(data_out[i], data_in[i], source_num * sizeof(float));
    }
}

void matc_step2(MatcP signal_obs, MatcP unmix, MatcP out, int fft_length, int source_num)
{
    int col, row, i;
    c_num temp1, temp2;
    for (i = 0; i < fft_length; i++)
    {
        for (col = 0; col < source_num; col++)
        {
            c_set_zero(&temp1, 0);
            for (row = 0; row < source_num; row++)
            {
                c_mul(&(signal_obs->data[i][row]), &((unmix + i)->data[col][row]), &temp2);
                c_add(&temp1, &temp2, &temp1);
            }
            out->data[i][col] = temp1;
        }
    }
}

void matc_R_matrix(MatcP esti, MatcP Phi, MatcP R, int fft_length, int source_num)
{
    int col, row, i;
    for (i = 0; i <fft_length; i++)
    {
        for (col = 0; col < source_num; col ++)
        {
            for (row = 0;row < source_num ; row++)
            {
                (R + i)->data[row][col].real = Phi->data[i][row].real * esti->data[i][col].real+
                                Phi->data[i][row].imag * esti->data[i][col].imag;
                (R + i)->data[row][col].imag = -Phi->data[i][row].real * esti->data[i][col].imag +
                                Phi->data[i][row].imag * esti->data[i][col].real;
            }
        }
    }
}

void matc_unmix_matrix_update(MatcP G, MatfP Ksi, MatcP R, int fft_length, int source_num, float eta)
{
    MatcP gradient = matc_zeros(source_num, source_num),
    temp1 = matc_zeros(source_num, source_num),
    temp2 = matc_zeros(source_num, source_num);
    int i;
    for (i = 0; i < fft_length; i++)
    {
        matc_select_diag((R + i), temp1);
        matc_sub(temp1, (R + i), temp2);
        matc_mul(temp2, (G + i), gradient);
        matc_real_mul(gradient, eta / sqrtf(Ksi->data[i][0]), temp1);
        matc_add((G + i), temp1, (G + i));
    }
    free_matc(gradient, 1);
    free_matc(temp1, 1);
    free_matc(temp2, 1);
}

void matc_ksi_update(MatfP Ksi, MatcP fft_data, int fft_length, int source_num, float beta)
{
    int i = 0, col = 0;
    float temp = 0;
    for (i = 0; i < fft_length; i++)
    {
        temp = 0;
        for (col = 0; col < source_num; col++)
        {
            temp += (fft_data->data[i][col].real * fft_data->data[i][col].real+
                     fft_data->data[i][col].imag * fft_data->data[i][col].imag);
        }
        Ksi->data[i][0] = beta * Ksi->data[i][0] + (1 - beta) * temp / 2.0f;
    }
}

void est_update(MatcP est, MatcP norm, int fft_length, int source_num)
{
    int i = 0, col = 0;
    c_num temp;
    for (i = 0; i < fft_length; i++)
    {
        for (col = 0; col < source_num; col++)
        {
            c_mul(&(est->data[i][col]), &((norm + i)->data[col][col]), &temp);
            est->data[i][col] = temp;
        }
    }
    
}






