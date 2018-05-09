/*************************************************************************
	> File Name: matrix.h
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#ifndef _MATRIX_H
#define _MATRIX_H
#include "stdlib.h"
#include "string.h"
#define MY_MEM_ALLOCATE(VAR_PTR, NUM, TYPE)  {VAR_PTR = ((TYPE*)(malloc((NUM) * sizeof(TYPE))));}
#define ENABLE_ASSERT 0
#if ENABLE_ASSERT
#include <assert.h>
#endif

#ifndef M_TWOPI
# define M_TWOPI       6.2831853071795862319959  /* 2*pi */
#endif

#ifndef M_PI
# define M_PI 3.141592653589793238462643383279502884196
#endif

#define MY_MAX(A, B) (A < B ? B : A)
#define MY_MIN(A, B) (A < B ? A : B)

typedef struct
{
    float real;
    float imag;
}c_num;

typedef struct
{
    int cols;
    int rows;
    float **data;
}Matf;
typedef Matf*const MatfP;
typedef struct
{
    float cols;
    float rows;
    c_num **data;
}Matc;

void c_add(c_num *_a, c_num *_b, c_num *_out);
void c_sub(c_num *_a, c_num *_b, c_num *_out);
void c_mul(c_num *_a, c_num *_b, c_num *_out);
void c_div(c_num *_a, c_num *_b, c_num *_out);
void c_real_mul(c_num *_a, float _b, c_num *_out);
void c_real_div(c_num *_a, float _b, c_num *_out);
void real_c_div(float _a, c_num *_b, c_num *out);
void c_print(c_num *_num);
void c_println(c_num *_num);
c_num *c_create(int _size);
MatfP matf_create(int rows, int cols, int _size);
void free_matf(MatfP _mat, int _size);
MatfP matf_zeros(int rows, int cols);
MatfP matf_ones(int rows, int cols);
MatfP matf_eye(int rows, int cols);
MatfP matf_eye_n(int rows, int cols, int sizes);
void print_matf(MatfP _data, int _size);
MatfP matf_sub(MatfP _a, MatfP _b, MatfP _out);
MatfP matf_cwise_mul(MatfP _a, MatfP _b, MatfP _out);
MatfP matf_real_div(MatfP _a, float _b, MatfP _out);
MatfP matf_cwise_div(MatfP _a, MatfP _b, MatfP _out);
MatfP matf_mul(MatfP _a, MatfP _b, MatfP _out);
MatfP matf_add(MatfP _a, MatfP _b, MatfP _out);
MatfP matf_real_mul(MatfP _a, float _b, MatfP _out);
void matf_reallocate(MatfP _data, int size_in, int rows, int cols, int _size);
MatfP matf_copy(MatfP _a, MatfP _out);
MatfP matf_transpose(MatfP _a, MatfP _out);
void swap(float *_a, float *_b);
void matf_LU_compose(MatfP _a, MatfP _out_L, MatfP _out_U);
MatfP matf_inverse(MatfP _a, MatfP _out);
float matf_det(MatfP _a);
#endif
