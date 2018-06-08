/*************************************************************************
	> File Name: matrix.h
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#ifndef _MATRIX_H
#define _MATRIX_H
#define ENABLE_ASSERT 0
#define ENABLE_TIME 0

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#define MY_MEM_ALLOCATE(VAR_PTR, NUM, TYPE)  {VAR_PTR = ((TYPE*)(malloc((NUM) * sizeof(TYPE))));}
#if ENABLE_ASSERT
#include <assert.h>
#define NEW_MAT_REAL(name, row, col)        \
                                            assert(__MATRICE_INTI__ == 1);\
                                            assert(__BASE_NUM_REAL__ < __MAX_SIZE_REAL__);\
                                            matf_resize(__REAL_BASE__ + __BASE_NUM_REAL__, row, col);\
                                            MatfP name =  __REAL_BASE__ + __BASE_NUM_REAL__;\
                                            __BASE_NUM_REAL__++;


#define NEW_MAT_COMPLEX(name, row, col)    \
                                            assert(__MATRICE_INTI__ == 1);\
                                            assert(__BASE_NUM_COMPLEX__ < __MAX_SIZE_COMPLEX__);\
                                            matc_resize(__COMPLEX_BASE__ + __BASE_NUM_COMPLEX__, row, col);\
                                            MatcP name =  __COMPLEX_BASE__ + __BASE_NUM_COMPLEX__;\
                                            __BASE_NUM_COMPLEX__++;
#define NEW_MULTI_MAT_COMPLEX(Ptr, row, col, size) \
                                            assert(__MATRICE_INTI__ == 1);\
                                            Ptr = __COMPLEX_BASE__ + __BASE_NUM_COMPLEX__;\
                                            for (__MULTI_TEMP_COUNT__=0;__MULTI_TEMP_COUNT__<size;__MULTI_TEMP_COUNT__++)\
                                            {\
                                            assert(__BASE_NUM_COMPLEX__ < __MAX_SIZE_COMPLEX__);\
                                            matc_resize(__COMPLEX_BASE__ + __BASE_NUM_COMPLEX__, row, col);\
                                            __BASE_NUM_COMPLEX__++;\
                                            }
#define NEW_MULTI_MAT_REAL(Ptr, row, col, size) \
                                            assert(__MATRICE_INTI__ == 1);\
                                            Ptr = __REAL_BASE__ + __BASE_NUM_REAL__;\
                                            for (__MULTI_TEMP_COUNT__=0;__MULTI_TEMP_COUNT__<size;__MULTI_TEMP_COUNT__++)\
                                            {\
                                            assert(__BASE_NUM_REAL__ < __MAX_SIZE_REAL__);\
                                            matf_resize(__REAL_BASE__ + __BASE_NUM_REAL__, row, col);\
                                            __BASE_NUM_REAL__++;\
                                            }

#else
#define NEW_MAT_REAL(name, row, col)        \
                                            matf_resize(__REAL_BASE__ + __BASE_NUM_REAL__, row, col);\
                                            MatfP name =  __REAL_BASE__ + __BASE_NUM_REAL__;\
                                            __BASE_NUM_REAL__++;


#define NEW_MAT_COMPLEX(name, row, col)    \
                                            matc_resize(__COMPLEX_BASE__ + __BASE_NUM_COMPLEX__, row, col);\
                                            MatcP name =  __COMPLEX_BASE__ + __BASE_NUM_COMPLEX__;\
                                            __BASE_NUM_COMPLEX__++;
#define NEW_MULTI_MAT_COMPLEX(Ptr, row, col, size) \
                                            Ptr = __COMPLEX_BASE__ + __BASE_NUM_COMPLEX__;\
                                            for (__MULTI_TEMP_COUNT__=0;__MULTI_TEMP_COUNT__<size;__MULTI_TEMP_COUNT__++)\
                                            {\
                                            matc_resize(__COMPLEX_BASE__ + __BASE_NUM_COMPLEX__, row, col);\
                                            __BASE_NUM_COMPLEX__++;\
                                            }
#define NEW_MULTI_MAT_REAL(Ptr, row, col, size) \
                                            Ptr = __REAL_BASE__ + __BASE_NUM_REAL__;\
                                            for (__MULTI_TEMP_COUNT__=0;__MULTI_TEMP_COUNT__<size;__MULTI_TEMP_COUNT__++)\
                                            {\
                                            matf_resize(__REAL_BASE__ + __BASE_NUM_REAL__, row, col);\
                                            __BASE_NUM_REAL__++;\
                                            }
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
    int cols;
    int rows;
    c_num **data;
}Matc;
typedef Matc*const MatcP;
extern int __BASE_NUM_REAL__, __BASE_NUM_COMPLEX__, __MAX_SIZE_REAL__, __MAX_SIZE_COMPLEX__, __MATRICE_INTI__, __MULTI_TEMP_COUNT__ ;
extern Matf *__REAL_BASE__;
extern Matc *__COMPLEX_BASE__;
int matrice_sys_init(int max_size_real, int max_size_complex);
void matrice_sys_exit(void);
void c_add(c_num *_a, c_num *_b, c_num *_out);
void c_set_zero(c_num *_a, int _size);
void c_sub(c_num *_a, c_num *_b, c_num *_out);
void c_mul(c_num *_a, c_num *_b, c_num *_out);
void c_div(c_num *_a, c_num *_b, c_num *_out);
void c_conj(c_num *_a, c_num *out);
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
MatfP matf_zeros_n(int rows, int cols, int sizes);
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
MatfP matf_row_mul(MatfP _a, MatfP _b, MatfP _out);
void swap(float *_a, float *_b);
void matf_LU_compose(MatfP _a, MatfP _out_L, MatfP _out_U);
MatfP matf_inverse(MatfP _a, MatfP _out);
float matf_det(MatfP _a);
MatcP matc_create(int rows, int cols, int _size);
void free_matc(MatcP _mat, int _size);
void print_matc(MatcP _data, int _size);
MatcP matc_zeros(int rows, int cols);
MatcP matc_ones(int rows, int cols);
MatcP matc_eye(int rows, int cols);
MatcP matc_eye_n(int rows, int cols, int sizes);
MatcP matc_zeros_n(int rows, int cols, int sizes);
void matc_reallocate(MatcP _data, int size_in, int rows, int cols, int _size);
MatcP matc_add(MatcP _a, MatcP _b, MatcP _out);
MatcP matc_sub(MatcP _a, MatcP _b, MatcP _out);
MatcP matc_cwise_mul(MatcP _a, MatcP _b, MatcP _out);
MatcP matc_complex_mul(MatcP _a, c_num *_b, MatcP _out);
MatcP matc_complex_div(MatcP _a, c_num *_b,MatcP _out);
MatcP matc_real_mul(MatcP _a, float _b, MatcP _out);
MatcP matc_cwise_div(MatcP _a, MatcP _b, MatcP _out);
MatcP matc_real_cwise_mul(MatcP _a, MatfP _b, MatcP _out);
MatcP matc_real_row_mul(MatcP _a, MatfP _b, MatcP _out);
MatcP matc_mul(MatcP _a, MatcP _b,MatcP _out);
void matc_LU_compose(MatcP _a, MatcP _out_L, MatcP _out_U);
MatcP matc_inverse(MatcP _a, MatcP _out);
c_num matc_det(MatcP _a);
MatcP matc_transpose(MatcP _a, MatcP _out);
MatcP matc_conj(MatcP _a, MatcP _out);
void matc_resize(MatcP _data, int rows, int cols);
void matf_resize(MatfP _data, int rows, int cols);
void matc_set_zeros(MatcP _data);
void matc_set_eye(MatcP _data);
void matc_set_all_num(MatcP _data, c_num num);
void matf_set_zeros(MatfP _data);
void matf_set_eye(MatfP _data);
void matf_set_all_num(MatfP _data, float num);
void matc_set_rand(MatcP _data, int range);
void matf_set_rand(MatfP _data, int range);
void matf_set_hanning(MatfP hanning_win);
void matf_set_hamming(MatfP hamming_win);
void matf_set_rectwin(MatfP rect_win);
void matf_convert2com(MatfP _a, MatcP _out);
void matc_set_conj(MatcP _a);
MatcP matc_copy(MatcP _a, MatcP _out);
void matc_convert2real(MatcP _a, MatfP _out);
MatcP matc_real_col_div(MatcP _a, MatfP _b, MatcP _out);
MatfP matc_metrix(MatcP _a, MatfP _out);
MatcP matc_select_diag(MatcP _a, MatcP _out);
#endif
