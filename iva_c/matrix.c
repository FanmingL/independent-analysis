/*************************************************************************
	> File Name: matrix.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include<stdio.h>
#include "matrix.h"
//_out = _a + _b
void c_add(c_num *_a, c_num *_b, c_num *_out)
{
    _out->real = _a->real + _b->real;
    _out->imag = _a->imag + _b->imag;
}

//_out = _a - _b
void c_sub(c_num *_a, c_num *_b, c_num *_out)
{
    _out->real = _a->real - _b->real;
    _out->imag = _a->imag - _b->imag;
}

//_out = _a * _b
void c_mul(c_num *_a, c_num *_b, c_num *_out)
{
    _out->real = _a->real * _b->real - _a->imag * _b->imag;
    _out->imag = _a->real * _b->imag + _a->imag * _b->real;
}

//_out = _a * _b where _b is real number
void c_real_mul(c_num *_a, float _b, c_num *_out)
{
    _out->real = _a->real * _b;
    _out->imag = _a->imag * _b;
}

//_out = _a / _b where _b is real number
void c_real_div(c_num *_a, float _b, c_num *_out)
{
    _b = 1 / _b;
    _out->real = _a->real * _b;
    _out->imag = _a->imag * _b;
}

//_out = _a / _b where _a is real number
void real_c_div(float _a, c_num *_b, c_num *_out)
{
    float sum_temp = _b->real * _b->real + _b->imag * _b->imag;
    _out->real = _b->real / sum_temp * _a;
    _out->imag = -_b->imag / sum_temp * _a;
}

//_out = _a / _b
void c_div(c_num *_a, c_num *_b, c_num *out)
{
    float sum_temp = _b->real * _b->real + _b->imag * _b->imag;
    out->real = (_a->real * _b->real + _a->imag * _b->imag ) / sum_temp;
    out->imag = (_a->imag * _b->real - _a->real * _b->imag ) / sum_temp;
}

//print the number
void c_print(c_num *_num)
{
    if (_num->imag >= 0)
    printf("%.2f + %.2fi ", _num->real, _num->imag);
    else
    printf("%.2f - %.2fi ", _num->real, -_num->imag);
}

//print the number with \n
void c_println(c_num *_num)
{
    if (_num->imag >= 0)
        printf("%.2f + %.2fi\n", _num->real, _num->imag);
    else
        printf("%.2f - %.2fi\n", _num->real, -_num->imag);
}

//create a complex number
c_num *c_create(int _size)
{
    c_num *_num;
    MY_MEM_ALLOCATE(_num, _size, c_num);
    return _num;
}

//free a complex number ptr
void c_free(c_num *_num)
{
    free((void *)_num);
}

//free and allocate
void matf_reallocate(MatfP _data, int size_in, int rows, int cols, int _size)
{
#if ENABLE_ASSERT
    static int sequence = 0;
#endif
    int i;
    for (; size_in>0; size_in--)
    {
        i = (size_in + _data - 1)->rows;
        for (; i > 0; i-- )
        {
            free((void *)(*((_data + size_in - 1)->data + i - 1)));
        }
        free((void * )(_data + size_in - 1)->data);
    }
    
    for (;_size>0;_size--)
    {
        (_data + _size - 1)->cols = cols;
        (_data + _size - 1)->rows = rows;
        MY_MEM_ALLOCATE((_data + _size - 1)->data, rows, float*);
        for (i=0; i<rows; i++)
        {
            MY_MEM_ALLOCATE(*((_data + _size - 1)->data + i), cols, float);
        }
    }
#if ENABLE_ASSERT
    printf("it's %d times that size is not  appropriate\n\n", ++sequence);
#endif
}

//create a matrix
MatfP matf_create(int rows, int cols, int _size)
{
    Matf *_data;
    int i;
    MY_MEM_ALLOCATE(_data, _size, Matf);
    for (;_size>0;_size--)
    {
        (_data + _size - 1)->cols = cols;
        (_data + _size - 1)->rows = rows;
        MY_MEM_ALLOCATE((_data + _size - 1)->data, rows, float*);
        for (i=0; i<rows; i++)
        {
            MY_MEM_ALLOCATE(*((_data + _size - 1)->data + i), cols, float);
        }
    }
    return _data;
}

//free a matrix ptr
void free_matf(MatfP _mat, int _size)
{
    int _row;
    
    for (; _size>0; _size--)
    {
        _row = (_size + _mat - 1)->rows;
        for (; _row > 0; _row-- )
        {
            free((void *)(*((_mat + _size - 1)->data + _row - 1)));
        }
        free((void * )(_mat + _size - 1)->data);
    }
    free((void *)_mat);
}

//_out = zeros(rows, cols)
MatfP matf_zeros(int rows, int cols)
{
    MatfP _data = matf_create(rows, cols, 1);
    for (;rows>0;rows--)
    memset(_data->data[rows - 1], 0, cols*sizeof(float));
    return _data;
}

//_out = ones(rows, cols)
MatfP matf_ones(int rows, int cols)
{
    MatfP _data = matf_create(rows, cols, 1);
    int col, row;
    for (row = 0; row < rows; row++)
    {
        for (col = 0; col < cols; col++)
        {
            _data->data[row][col] = 1;
        }
    }
    return _data;
}

//_out = eye(rows, cols)
MatfP matf_eye(int rows, int cols)
{
    MatfP _data = matf_zeros(rows, cols);
    int row;
    for(row = 0; row < MY_MIN(rows, cols); row++)
    {
        _data->data[row][row] = 1;
    }
    return _data;
}

MatfP matf_eye_n(int rows, int cols, int sizes)
{
    MatfP _data = matf_create(rows, cols, sizes);
    int row, size;
    for (size=0; size < sizes; size ++)
    {
        for (row = 0; row < rows; row++)
        {
            memset((_data + size)->data[row], 0, cols * sizeof(float));
        }
        for(row = 0; row < MY_MIN(rows, cols); row++)
        {
            (_data + size)->data[row][row] = 1;
        }
    }
    return _data;
}

void print_matf(MatfP _data, int _size)
{
    int size, col, row;
    for (size = 0; size < _size; size++)
    {
        for (row = 0; row < (_data + size)->rows; row++)
        {
            for (col = 0; col < (_data + size)->cols; col++)
            {
                printf("%.2f ",(_data + size)->data[row][col]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
}

//_out = _a + _b
MatfP matf_add(MatfP _a, MatfP _b, MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] + _b->data[row][col];
        }
    }
    return _out;
}

//_out = _a - _b
MatfP matf_sub(MatfP _a, MatfP _b, MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] - _b->data[row][col];
        }
    }
    return _out;
}

//_out = _a .* _b
MatfP matf_cwise_mul(MatfP _a, MatfP _b, MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] * _b->data[row][col];
        }
    }
    return _out;
}

//_out = _a * _b
MatfP matf_real_mul(MatfP _a, float _b, MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _out->cols);
    assert(_a->rows == _out->rows);
    assert(_out != NULL);
#endif
    int col, row;
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] * _b;
        }
    }
    return _out;
}

//_out = _a / _b
MatfP matf_real_div(MatfP _a, float _b,MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _out->cols);
    assert(_a->rows == _out->rows);
    assert(_out != NULL);
#endif
    int col, row;
    _b = 1 / _b;
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] * _b;
        }
    }
    return _out;
}

//_out = _a ./ _b
MatfP matf_cwise_div(MatfP _a, MatfP _b, MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] / _b->data[row][col];
        }
    }
    return _out;
}

//_out = _a * _b
MatfP matf_mul(MatfP _a, MatfP _b,MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->rows);
    assert(_out != NULL);
#endif
    int row, col, mul_index;
    float _temp;
    if (_out->rows != _a->rows || _out->cols != _b->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _b->cols, 1);
    }
    for (row = 0; row < _out->rows; row++)
    {
        for (col = 0; col < _out->cols; col++)
        {
            _temp = 0;
            for (mul_index = 0; mul_index < _a->cols; mul_index++)
            {
                _temp += _a->data[row][mul_index] * _b->data[mul_index][col];
            }
            _out->data[row][col] = _temp;
        }
    }
    return _out;
}

MatfP matf_copy(MatfP _a, MatfP _out)
{
    int row;
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        memcpy(_out->data[row], _a->data[row], _a->cols * sizeof(float));
    }
    
    return _out;
}

void swap(float *_a, float *_b)
{
    float temp = *_a;
    *_a = *_b;
    *_b = temp;
}

MatfP matf_transpose(MatfP _a, MatfP _out)
{
    int row, col;
    if (_out->rows != _a->cols || _out->cols != _a->rows)
    {
        matf_reallocate(_out, 1, _a->cols, _a->rows, 1);
    }
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[col][row] = _a->data[row][col];
        }
    }
    return _out;
}

void matf_LU_compose(MatfP _a, MatfP _out_L, MatfP _out_U)
{
#if ENABLE_ASSERT
    assert(_a->cols==_a->rows);
#endif
    int i, j, k, N=_a->rows;
    float s;
    if (_out_U->rows != _a->rows || _out_U->cols != _a->cols)
    {
        matf_reallocate(_out_U, 1, _a->rows, _a->cols, 1);
    }
    if (_out_L->rows != _a->rows || _out_L->cols != _a->cols)
    {
        matf_reallocate(_out_L, 1, _a->rows, _a->cols, 1);
    }
    
    for (i = 0; i < N; i++)
    {
        memset(_out_L->data[i], 0, N * sizeof(float));
        memset(_out_U->data[i], 0, N * sizeof(float));
    }
    /* here start caculate the L and U, det(L) = 1, det(U) = det(A), A = L*U, A^-1 = U^-1*L^-1 */
    for (i = 0; i < N;i++)
    {
        _out_L->data[i][i] = 1;
    }
    
    for (i = 0;i < N;i++)
    {
        for (j = i;j < N;j++)
        {
            s = 0;
            for (k = 0;k < i;k++)
            {
                s += _out_L->data[i][k] * _out_U->data[k][j];
            }
            _out_U->data[i][j] = _a->data[i][j] - s;
        }
        
        for (j = i + 1;j < N;j++)
        {
            s = 0;
            for (k = 0; k < i; k++)
            {
                s += _out_L->data[j][k] * _out_U->data[k][i];
            }
            _out_L->data[j][i] = (_a->data[j][i] - s) / _out_U->data[i][i];
        }
    }
}

MatfP matf_inverse(MatfP _a, MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->rows==_a->cols);
#endif
    int N = _a->rows;
    int i, j, k;
    float s;
    MatfP L = matf_eye(N, N),
    L_inverse = matf_eye(N, N),
    U = matf_zeros(N, N),
    U_inverse = matf_zeros(N, N);
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
    /* here start caculate the L and U, det(L) = 1 */
    for (i = 0;i < N;i++)
    {
        for (j = i;j < N;j++)
        {
            s = 0;
            for (k = 0;k < i;k++)
            {
                s += L->data[i][k] * U->data[k][j];
            }
            U->data[i][j] = _a->data[i][j] - s;
        }
        
        for (j = i + 1;j < N;j++)
        {
            s = 0;
            for (k = 0; k < i; k++)
            {
                s += L->data[j][k] * U->data[k][i];
            }
            L->data[j][i] = (_a->data[j][i] - s) / U->data[i][i];
        }
    }
#if ENABLE_ASSERT
    for(i = 0; i < N; i++)
    {
        assert(U->data[i][i] != 0);
    }
#endif
    /* here start caculate the inverse of L */
    for (i= 1;i < N;i++)
    {
        for (j = 0;j < i;j++)
        {
            s = 0;
            for (k = 0;k < i;k++)
            {
                s += L->data[i][k] * L_inverse->data[k][j];
            }
            L_inverse->data[i][j] = -s;
        }
    }
    /* here start caculate the inverse of U */
    for (i = 0;i < N;i++)
    {
        U_inverse->data[i][i] = 1 / U->data[i][i];
    }
    for (i = 1;i < N;i++)
    {
        for (j = i - 1;j >=0;j--)
        {
            s = 0;
            for (k = j + 1;k <= i;k++)
            {
                s += U->data[j][k] * U_inverse->data[k][i];
            }
            U_inverse->data[j][i] = -s / U->data[j][j];
        }
    }
    matf_mul(U_inverse, L_inverse, _out);    
    free_matf(L, 1);
    free_matf(U, 1);
    free_matf(L_inverse, 1);
    free_matf(U_inverse, 1);
    return _out;
}

float matf_det(MatfP _a)
{
#if ENABLE_ASSERT
    assert(_a->rows==_a->cols);
#endif
    float out;
    int N = _a->rows;
    int i, j, k;
    float s;
    MatfP L = matf_eye(N, N),
    U = matf_zeros(N, N);
    /* here start caculate the L and U, det(L) = 1 */
    for (i = 0;i < N;i++)
    {
        for (j = i;j < N;j++)
        {
            s = 0;
            for (k = 0;k < i;k++)
            {
                s += L->data[i][k] * U->data[k][j];
            }
            U->data[i][j] = _a->data[i][j] - s;
        }
        
        for (j = i + 1;j < N;j++)
        {
            s = 0;
            for (k = 0; k < i; k++)
            {
                s += L->data[j][k] * U->data[k][i];
            }
            L->data[j][i] = (_a->data[j][i] - s) / U->data[i][i];
        }
    }
    out = 1;
    for(i = 0; i < N; i++)
    {
        out *= U->data[i][i];
    }
    free_matf(L, 1);
    free_matf(U, 1);
    return out;
}
