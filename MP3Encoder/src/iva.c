/*************************************************************************
	> File Name: iva.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include <stdio.h>
#include <iva.h>
#include <mathf.h>
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
    NEW_MULTI_MAT_COMPLEX(iva_it->norm_matrix, iva_it->effective_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_input, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_sperated, shift_size, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_overlapadd, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_reconstruct, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->window, fft_length, 1, 1);
    NEW_MULTI_MAT_REAL(iva_it->data_buffer, shift_size, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->data_fft, iva_it->fft_length, source_num, 1);
    iva_it->data_fft->rows = iva_it->effective_length;
    NEW_MULTI_MAT_REAL(iva_it->data_windowed, fft_length, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->ksi, iva_it->effective_length, 1, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->data_estimate, iva_it->effective_length, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->Phi, iva_it->effective_length, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->gradient_unmix, source_num, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->R_temp_unmix, source_num, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->temp1_unmix, source_num, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->temp_est, iva_it->effective_length, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->temp_norm, source_num, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->temp_c1_unmix, source_num, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->temp_c2_unmix, 1, source_num, 1);
    NEW_MULTI_MAT_COMPLEX(iva_it->temp_c3_unmix, 1, source_num, 1);
    NEW_MULTI_MAT_REAL(iva_it->temp_process, 1, source_num, 1);
    
    for (i = 0; i<iva_it->effective_length; i++)
    {
        matc_set_eye(iva_it->unmix_matrix + i);
    }
    matc_set_zeros(iva_it->R_temp_unmix);
    matf_set_hamming(iva_it->window);
    matf_set_all_num(iva_it->ksi, 1);
    iva_it->div_temp  = (1-beta) / 2;
}

void iva_exit(iva_tP iva_it)
{
    
}

float** iva_step(iva_tP iva_it, float *data)
{
    memcpy(iva_it->data_buffer->data[iva_it->data_buffer_count++], data,
           iva_it->source_num*sizeof(float));
    if ((iva_it->data_buffer_count) == iva_it->shift_size)
    {
        iva_it->data_buffer_count = 0;
        iva_frame_shift(iva_it->shift_size, iva_it->fft_length, iva_it->source_num, iva_it->data_input->data);
        iva_frame_cat(iva_it->shift_size, iva_it->fft_length, iva_it->source_num, iva_it->data_buffer->data, iva_it->data_input->data);
        iva_process(iva_it);
        return iva_it->data_sperated->data;
    }
    return NULL;
}

void iva_process(iva_tP iva_it)
{
	matf_set_zeros(iva_it->temp_process);
    
    matf_row_mul(iva_it->data_input, iva_it->window, iva_it->data_windowed);
    
    fft_real(iva_it->data_windowed, iva_it->data_fft);
    
    /* imlement iva algorithm */
    iva_estimate(iva_it, iva_it->data_fft, iva_it->unmix_matrix, iva_it->data_estimate, iva_it->effective_length, iva_it->source_num);

    iva_norm_matrix(iva_it, iva_it->unmix_matrix, iva_it->norm_matrix, iva_it->effective_length, iva_it->source_num);
    
    matc_metrix(iva_it->data_estimate, iva_it->temp_process);

    matc_real_col_div(iva_it->data_estimate, iva_it->temp_process, iva_it->Phi);

    iva_unmix_matrix_update(iva_it, iva_it->unmix_matrix, iva_it->ksi, iva_it->Phi, iva_it->data_estimate, iva_it->effective_length, iva_it->source_num, iva_it->eta);
    
    iva_ksi_update(iva_it, iva_it->ksi, iva_it->data_fft, iva_it->effective_length, iva_it->source_num, iva_it->beta);
    
    iva_est_update(iva_it, iva_it->data_estimate, iva_it->norm_matrix, iva_it->effective_length, iva_it->source_num);
    
    /* reconstruct signal */
    ifft_real(iva_it->data_estimate, iva_it->data_reconstruct);
    matf_add(iva_it->data_overlapadd, iva_it->data_reconstruct, iva_it->data_overlapadd);
    iva_frame_split(iva_it->shift_size, iva_it->fft_length, iva_it->source_num, iva_it->data_overlapadd->data, iva_it->data_sperated->data);
    iva_frame_shift(iva_it->shift_size, iva_it->fft_length, iva_it->source_num,
                    iva_it->data_overlapadd->data);
    iva_frame_cat_zeros(iva_it->shift_size, iva_it->fft_length, iva_it->source_num, iva_it->data_overlapadd->data);
}

void iva_frame_shift(int shift_size, int fft_length, int source_num, float **data)
{
    //int i=0;
    //for (; i < fft_length - shift_size; i++)
    //{
    //    memcpy(data[i], data[i+shift_size], source_num * sizeof(float));
    //}
    memcpy(data[0], data[shift_size], (fft_length - shift_size) * source_num * sizeof(float));
}

void iva_frame_cat(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out)
{
   // int i=fft_length - shift_size;
   // for (; i < fft_length; i++)
   // {
   //     memcpy(data_out[i], data_in[i + shift_size - fft_length], source_num * sizeof(float));
   // }
    memcpy(data_out[fft_length - shift_size], data_in[0], shift_size * source_num * sizeof(float));
}

void iva_frame_cat_zeros(int shift_size, int fft_length, int source_num, float ** data_out)
{
    /*int i=fft_length - shift_size;
    for (; i < fft_length; i++)
    {
        memset(data_out[i], 0, source_num * sizeof(float));
    }*/
    memset(data_out[fft_length - shift_size], 0, shift_size * source_num * sizeof(float));
}

void iva_frame_split(int shift_size, int fft_length, int source_num, float **data_in, float ** data_out)
{
    /*int i=0;
    for (; i < shift_size; i++)
    {
        memcpy(data_out[i], data_in[i], source_num * sizeof(float));
    }*/
    memcpy(data_out[0], data_in[0],shift_size *  source_num * sizeof(float));
}

void iva_estimate(iva_tP iva_it, MatcP signal_obs, MatcP unmix, MatcP out, int fft_length, int source_num)
{
    int i;
#if 0


    for (i = 0; i < fft_length; i++)
    {
        matc_transpose(unmix + i, iva_it->temp_c1_unmix);
        memcpy(iva_it->temp_c2_unmix->data[0], signal_obs->data[i], source_num * sizeof(c_num));
        matc_mul(iva_it->temp_c2_unmix, iva_it->temp_c1_unmix, iva_it->temp_c3_unmix);
        memcpy(out->data[i], iva_it->temp_c3_unmix->data[0], source_num * sizeof(c_num));
    }


#else
     //int col, row;
     //c_num temp1, temp2;
     for (i = 0; i < fft_length; i++)
     {
    	 out->data[i][0].real = ((*(unmix + i)).data)[0][0].real * signal_obs->data[i][0].real +
    	         		((*(unmix + i)).data)[0][1].real * signal_obs->data[i][1].real -
    	         		((*(unmix + i)).data)[0][0].imag * signal_obs->data[i][0].imag -
    	         		((*(unmix + i)).data)[0][1].imag * signal_obs->data[i][1].imag;
    	 out->data[i][0].imag = ((*(unmix + i)).data)[0][0].imag * signal_obs->data[i][0].real +
    	         		((*(unmix + i)).data)[0][1].imag * signal_obs->data[i][1].real +
    	         		((*(unmix + i)).data)[0][0].real * signal_obs->data[i][0].imag+
    	         		((*(unmix + i)).data)[0][1].real * signal_obs->data[i][1].imag;
    	 out->data[i][1].real = ((*(unmix + i)).data)[1][0].real * signal_obs->data[i][0].real +
    	 	         		((*(unmix + i)).data)[1][1].real * signal_obs->data[i][1].real -
    	 	         		((*(unmix + i)).data)[1][0].imag * signal_obs->data[i][0].imag -
    	 	         		((*(unmix + i)).data)[1][1].imag * signal_obs->data[i][1].imag;
    	 out->data[i][1].imag = ((*(unmix + i)).data)[1][0].imag * signal_obs->data[i][0].real +
	         		((*(unmix + i)).data)[1][1].real * signal_obs->data[i][1].imag +
	         		((*(unmix + i)).data)[1][0].real * signal_obs->data[i][0].imag +
	         		((*(unmix + i)).data)[1][1].imag * signal_obs->data[i][1].real;
     }
#endif
    
    
    
}

void iva_R_matrix(MatcP esti, MatcP Phi, MatcP R, int fft_length, int source_num)
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

void iva_unmix_matrix_update(iva_tP iva_it, MatcP G, MatfP Ksi, MatcP Phi, MatcP esti, int fft_length, int source_num, float eta)
{
    int i, col, row;
    for (i = 0; i < fft_length; i++)
    {
        for (col = 0; col < source_num; col ++)
        {
            for (row = 0;row < source_num ; row++)
            {
                if (row != col)
                {
                    (iva_it->R_temp_unmix)->data[row][col].real = -Phi->data[i][row].real * esti->data[i][col].real-
                    Phi->data[i][row].imag * esti->data[i][col].imag;
                    (iva_it->R_temp_unmix)->data[row][col].imag = Phi->data[i][row].real * esti->data[i][col].imag -
                    Phi->data[i][row].imag * esti->data[i][col].real;
                }
            }
        }
        matc_mul(iva_it->R_temp_unmix, (G + i), iva_it->gradient_unmix);
        matc_real_mul(iva_it->gradient_unmix, eta / sqrtf(Ksi->data[i][0]), iva_it->temp1_unmix);
        matc_add((G + i), iva_it->temp1_unmix, (G + i));
    }

}

void iva_ksi_update(iva_tP iva_it, MatfP Ksi, MatcP fft_data, int fft_length, int source_num, float beta)
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
        Ksi->data[i][0] = beta * Ksi->data[i][0] + iva_it->div_temp * temp ;
    }
}

void iva_est_update(iva_tP iva_it, MatcP est, MatcP norm, int fft_length, int source_num)
{
    matc_cwise_mul(est, norm, iva_it->temp_est);
    matc_copy(iva_it->temp_est, est);
}

/*
 void c_div(c_num *_a, c_num *_b, c_num *out)
 {
 float sum_temp = _b->real * _b->real + _b->imag * _b->imag;
 out->real = (_a->real * _b->real + _a->imag * _b->imag ) / sum_temp;
 out->imag = (_a->imag * _b->real - _a->real * _b->imag ) / sum_temp;
 }

 */
#define VAL_DIV(x,y,out,abs_y) out.real = (x.real * y.real + x.imag * y.imag ) / abs_y;\
out.imag = (x.imag * y.real - x.real * y.imag ) / abs_y;
void iva_norm_matrix(iva_tP iva_it, MatcP unmix, MatcP norm, int fft_length, int source_num)
{
    int i = 0, t = 0;
    c_num det_val ;
    float abs_y = 0;
    for (i = 0; i < fft_length; i++)
    {
#if 0
        matc_inverse(unmix + i, iva_it->temp_norm);
        for (t = 0; t < source_num; t++)
        {
            norm->data[i][t] = iva_it->temp_norm->data[t][t];
        }
#else
        /*x_inverse = [x(2,2), - x(1,2);-x(2,1), x(1,1)] / (x(1,1) * x(2,2) -x(1,2) * x(2,1));*/
        det_val.real = ((*(unmix + i)).data)[0][0].real * ((*(unmix + i)).data)[1][1].real-
        ((*(unmix + i)).data)[0][0].imag * ((*(unmix + i)).data)[1][1].imag-(
                                                                             ((*(unmix + i)).data)[0][1].real * ((*(unmix + i)).data)[1][0].real-
                                                                             ((*(unmix + i)).data)[0][1].imag * ((*(unmix + i)).data)[1][0].imag
                                                                             );
        det_val.imag = ((*(unmix + i)).data)[0][0].imag * ((*(unmix + i)).data)[1][1].real+
        ((*(unmix + i)).data)[0][0].real * ((*(unmix + i)).data)[1][1].imag-(
                                                                             ((*(unmix + i)).data)[0][1].imag * ((*(unmix + i)).data)[1][0].real+
                                                                             ((*(unmix + i)).data)[0][1].real * ((*(unmix + i)).data)[1][0].imag
                                                                             );
        abs_y = det_val.real*det_val.real+det_val.imag*det_val.imag;
        VAL_DIV((((*(unmix + i)).data)[1][1]),det_val,(norm->data[i][0]),abs_y);
        VAL_DIV((((*(unmix + i)).data)[0][0]),det_val,(norm->data[i][1]),abs_y);

#endif
    }
}






