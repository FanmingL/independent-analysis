/*************************************************************************
	> File Name: matrix.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include <stdio.h>
#include <matrix.h>
#include <my_sys.h>
#define VAL_ADD(x,y,out) out.real = x.real + y.real;\
							out.imag = x.imag + y.imag;
#define VAL_MUL(x,y,out) out.real = x.real * y.real - x.imag * y.imag;\
							out.imag = x.real * y.imag + x.imag * y.real;
#define VAL_SUB(x,y,out) out.real = x.real - y.real;\
							out.imag = x.imag - y.imag;
Matc* L_g, *L_inverse_g, *U_g, *U_inverse_g;
//_out = _a + _b
void c_add(c_num *_a, c_num *_b, c_num *_out)
{
    _out->real = _a->real + _b->real;
    _out->imag = _a->imag + _b->imag;
}

// _a = 0 + 0i;
void c_set_zero(c_num *_a, int _size)
{
    memset(_a, 0, _size * sizeof(c_num));
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

//_out = conj(_a)
void c_conj(c_num *_a, c_num *out)
{
    out->real = _a->real;
    out->imag = -_a->imag;
}

//print the number
void c_print(c_num *_num)
{
    if (_num->imag >= 0)
    printf("%.2f + %.2fi, ", _num->real, _num->imag);
    else
    printf("%.2f - %.2fi, ", _num->real, -_num->imag);
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
            memset(*((_data + _size - 1)->data + i), 0, sizeof(float) * cols);
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
        MY_MEM_ALLOCATE((_data + _size - 1)->data[0], rows * cols, float);
        for (i=0; i<rows; i++)
        {
        	(_data + _size - 1)->data[i] = i * cols + (_data + _size - 1)->data[0];
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

MatfP matf_zeros_n(int rows, int cols, int sizes)
{
    MatfP _data = matf_create(rows, cols, sizes);
    int row, size;
    for (size=0; size < sizes; size ++)
    {
        for (row = 0; row < rows; row++)
        {
            memset((_data + size)->data[row], 0, cols * sizeof(float));
        }
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
                printf("%.2f, ",(_data + size)->data[row][col]);
            }
            printf(";\n");
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] * _b->data[row][col];
        }
    }
    return _out;
}

MatfP matf_row_mul(MatfP _a, MatfP _b, MatfP _out)
{
#if ENABLE_ASSERT
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col] * _b->data[row][0];
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _b->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _b->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->cols || _out->cols != _a->rows)
    {
        matf_reallocate(_out, 1, _a->cols, _a->rows, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out_U->rows != _a->rows || _out_U->cols != _a->cols)
    {
        matf_reallocate(_out_U, 1, _a->rows, _a->cols, 1);
    }
    if (_out_L->rows != _a->rows || _out_L->cols != _a->cols)
    {
        matf_reallocate(_out_L, 1, _a->rows, _a->cols, 1);
    }
#endif
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
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
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


//create a complex matrix
MatcP matc_create(int rows, int cols, int _size)
{
    Matc *_data;
    int i;
    MY_MEM_ALLOCATE(_data, _size, Matc);
    for (;_size>0;_size--)
    {

        (_data + _size - 1)->cols = cols;
        (_data + _size - 1)->rows = rows;
        MY_MEM_ALLOCATE((_data + _size - 1)->data, rows, c_num*);
        MY_MEM_ALLOCATE((_data + _size - 1)->data[0], rows * cols, c_num);
        for (i=0; i<rows; i++)
        {
        	(_data + _size - 1)->data[i] = i * cols + (_data + _size - 1)->data[0];
        }
    }
    return _data;
}

//free a matrix ptr
void free_matc(MatcP _mat, int _size)
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

void print_matc(MatcP _data, int _size)
{
    int size, col, row;
    for (size = 0; size < _size; size++)
    {
        for (row = 0; row < (_data + size)->rows; row++)
        {
            for (col = 0; col < (_data + size)->cols; col++)
            {
                c_print(&((_data + size)->data[row][col]));
            }
            printf(";\n");
        }
        printf("\n\n");
    }
}

//_out = zeros(rows, cols)
MatcP matc_zeros(int rows, int cols)
{
    MatcP _data = matc_create(rows, cols, 1);
    for (;rows>0;rows--)
        memset(_data->data[rows - 1], 0, cols*sizeof(c_num));
    return _data;
}

//_out = ones(rows, cols)
MatcP matc_ones(int rows, int cols)
{
    MatcP _data = matc_create(rows, cols, 1);
    int col, row;
    for (row = 0; row < rows; row++)
    {
        for (col = 0; col < cols; col++)
        {
            _data->data[row][col].real = 1;
            _data->data[row][col].imag = 0;
        }
    }
    return _data;
}

//_out = eye(rows, cols)
MatcP matc_eye(int rows, int cols)
{
    MatcP _data = matc_zeros(rows, cols);
    int row;
    for(row = 0; row < MY_MIN(rows, cols); row++)
    {
        _data->data[row][row].real = 1;
    }
    return _data;
}

MatcP matc_eye_n(int rows, int cols, int sizes)
{
    MatcP _data = matc_create(rows, cols, sizes);
    int row, size;
    for (size=0; size < sizes; size ++)
    {
        for (row = 0; row < rows; row++)
        {
            memset((_data + size)->data[row], 0, cols * sizeof(c_num));
        }
        for(row = 0; row < MY_MIN(rows, cols); row++)
        {
            (_data + size)->data[row][row].real = 1;
        }
    }
    return _data;
}

MatcP matc_zeros_n(int rows, int cols, int sizes)
{
    MatcP _data = matc_create(rows, cols, sizes);
    int row, size;
    for (size=0; size < sizes; size ++)
    {
        for (row = 0; row < rows; row++)
        {
            memset((_data + size)->data[row], 0, cols * sizeof(c_num));
        }
    }
    return _data;
}

//free and allocate
void matc_reallocate(MatcP _data, int size_in, int rows, int cols, int _size)
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
        MY_MEM_ALLOCATE((_data + _size - 1)->data, rows, c_num*);
        for (i=0; i<rows; i++)
        {
            MY_MEM_ALLOCATE(*((_data + _size - 1)->data + i), cols, c_num);
            memset(*((_data + _size - 1)->data + i), 0, sizeof(c_num) * cols);
        }
        
    }
#if ENABLE_ASSERT
    printf("it's %d times that size is not  appropriate\n\n", ++sequence);
#endif
}

//free and allocate
void matc_resize(MatcP _data, int rows, int cols)
{
#if ENABLE_ASSERT
    assert(_data != NULL);
#endif
    int i;
    i = _data->rows;
    free((void *)((_data->data[0])));
    free((void * )(_data->data));

    _data->cols = cols;
    _data->rows = rows;
    MY_MEM_ALLOCATE(_data->data, rows, c_num*);
    MY_MEM_ALLOCATE(*(_data->data), cols * rows, c_num);
    memset(*(_data->data), 0, sizeof(c_num) * cols * rows);
    for (i=0; i<rows; i++)
    {
    	*(_data->data + i) = i * cols + *(_data->data);
    }
}

//free and allocate
void matf_resize(MatfP _data, int rows, int cols)
{
#if ENABLE_ASSERT
    assert(_data != NULL);
#endif
    int i;
	i = _data->rows;
	free((void *)((_data->data[0])));
	free((void * )(_data->data));

	_data->cols = cols;
	_data->rows = rows;
	MY_MEM_ALLOCATE(_data->data, rows, float*);
	MY_MEM_ALLOCATE(*(_data->data), cols * rows, float);
	memset(*(_data->data), 0, sizeof(float) * cols * rows);
	for (i=0; i<rows; i++)
	{
		*(_data->data + i) = i * cols + *(_data->data);
	}
}

//_out = _a + _b
MatcP matc_add(MatcP _a, MatcP _b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_add(&(_a->data[row][col]), &(_b->data[row][col]), &(_out->data[row][col]));
        }
    }
    return _out;
}

//_out = _a - _b
MatcP matc_sub(MatcP _a, MatcP _b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_sub(&(_a->data[row][col]), &(_b->data[row][col]), &(_out->data[row][col]));
        }
    }
    return _out;
}

//_out = _a .* _b
MatcP matc_cwise_mul(MatcP _a, MatcP _b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_mul(&(_a->data[row][col]), &(_b->data[row][col]), &(_out->data[row][col]));
        }
    }
    return _out;
}

//_out = _a * _b
MatcP matc_complex_mul(MatcP _a, c_num *_b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _out->cols);
    assert(_a->rows == _out->rows);
    assert(_out != NULL);
#endif
    int col, row;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_sub(&(_a->data[row][col]), (_b), &(_out->data[row][col]));
        }
    }
    return _out;
}

//_out = _a / _b
MatcP matc_complex_div(MatcP _a, c_num *_b,MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _out->cols);
    assert(_a->rows == _out->rows);
    assert(_out != NULL);
#endif
    int col, row;
    c_num _temp;
    c_real_div(_b, 1, &_temp);
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_mul(&(_a->data[row][col]), &(_temp), &(_out->data[row][col]));
        }
    }
    return _out;
}

//_out = _a ./ _b
MatcP matc_cwise_div(MatcP _a, MatcP _b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
    assert(_a->rows == _b->rows);
    assert(_out != NULL);
#endif
    int col, row;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_sub(&(_a->data[row][col]), &(_b->data[row][col]), &(_out->data[row][col]));
        }
    }
    return _out;
}


/*
 *  _out->real = _a->real * _b->real - _a->imag * _b->imag;
    _out->imag = _a->real * _b->imag + _a->imag * _b->real;
 */

//_out = _a * _b
MatcP matc_mul(MatcP _a, MatcP _b,MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->rows);
    assert(_out != NULL);
#endif
    int row, col, mul_index;
    c_num _temp;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _b->cols)
    {
        (_out, 1, _a->rows, _b->cols, 1);
    }
#endif
    for (row = 0; row < _out->rows; row++)
    {
        
        for (col = 0; col < _out->cols; col++)
        {
            memset(&(_out->data[row][col]), 0, sizeof(c_num));
            
            for (mul_index = 0; mul_index < _a->cols; mul_index++)
            {

            	VAL_MUL(_a->data[row][mul_index], _b->data[mul_index][col], _temp);
            	VAL_ADD(_temp, _out->data[row][col], _out->data[row][col]);

            }
        }
    }
    return _out;
}

void matc_LU_compose(MatcP _a, MatcP _out_L, MatcP _out_U)
{
#if ENABLE_ASSERT
    assert(_a->cols==_a->rows);
#endif
    int i, j, k, N=_a->rows;
    c_num s1, s2;
#if ENABLE_ASSERT
    if (_out_U->rows != _a->rows || _out_U->cols != _a->cols)
    {
        matc_reallocate(_out_U, 1, _a->rows, _a->cols, 1);
    }
    if (_out_L->rows != _a->rows || _out_L->cols != _a->cols)
    {
        matc_reallocate(_out_L, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (i = 0; i < N; i++)
    {
        memset(_out_L->data[i], 0, N * sizeof(c_num));
        memset(_out_U->data[i], 0, N * sizeof(c_num));
    }
    /* here start caculate the L and U, det(L) = 1, det(U) = det(A), A = L*U, A^-1 = U^-1*L^-1 */
    for (i = 0; i < N;i++)
    {
        _out_L->data[i][i].real = 1;
    }
    
    for (i = 0;i < N;i++)
    {
        for (j = i;j < N;j++)
        {
            c_set_zero(&s1, 1);
            for (k = 0;k < i;k++)
            {
                c_mul(&(_out_L->data[i][k]), &(_out_U->data[k][j]), &(s2));
                c_add(&(s2), &(s1), &s1);
            }
            c_sub(&(_a->data[i][j]), &(s1), &(_out_U->data[i][j]));
        }
        
        for (j = i + 1;j < N;j++)
        {
            c_set_zero(&s1, 1);
            for (k = 0; k < i; k++)
            {
                c_mul(&(_out_L->data[j][k]), &(_out_U->data[k][i]), &(s2));
                c_add(&(s2), &(s1), &s1);
            }
            c_sub(&(_a->data[j][i]), &(s1), &(s2));
            c_div(&(s2), &(_out_U->data[i][i]), &(_out_L->data[j][i]));
        }
    }
}


MatcP matc_inverse(MatcP _a, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->rows==_a->cols);
#endif
    int N = _a->rows;
    int i, j, k;
    c_num s1, s2;
    matc_set_eye(L_g);
    matc_set_eye(L_inverse_g);
    matc_set_zeros(U_g);
    matc_set_zeros(U_inverse_g);
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    /* here start caculate the L and U, det(L) = 1, det(U) = det(A), A = L*U, A^-1 = U^-1*L^-1 */
    for (i = 0;i < N;i++)
    {
        for (j = i;j < N;j++)
        {
            c_set_zero(&s1, 1);
            for (k = 0;k < i;k++)
            {
            	VAL_MUL(L_g->data[i][k], U_g->data[k][j], s2);
            	VAL_ADD(s2, s1, s1);
            }
            VAL_SUB(_a->data[i][j], s1, U_g->data[i][j]);
        }
        
        for (j = i + 1;j < N;j++)
        {
            c_set_zero(&s1, 1);
            for (k = 0; k < i; k++)
            {
            	VAL_MUL((L_g->data[j][k]), (U_g->data[k][i]), (s2));
            	VAL_ADD(s2, s1, s1);
            }
            VAL_SUB(_a->data[j][i], s1, s2);
            c_div(&(s2), &(U_g->data[i][i]), &(L_g->data[j][i]));
        }
    }
#if ENABLE_ASSERT
    for(i = 0; i < N; i++)
    {
        assert(U->data[i][i].real != 0 || U->data[i][i].imag != 0);
    }
#endif
    /* here start caculate the inverse of L */
    for (i= 1;i < N;i++)
    {
        for (j = 0;j < i;j++)
        {
            c_set_zero(&s1, 1);
            for (k = 0;k < i;k++)
            {
            	VAL_MUL(L_g->data[i][k], L_inverse_g->data[k][j], s2);
            	VAL_SUB(s1, s2, s1);
            }
            L_inverse_g->data[i][j] = s1;
        }
    }
    /* here start caculate the inverse of U */
    for (i = 0;i < N;i++)
    {
        real_c_div(1, &(U_g->data[i][i]), &(U_inverse_g->data[i][i]));
    }
    for (i = 1;i < N;i++)
    {
        for (j = i - 1;j >=0;j--)
        {
            c_set_zero(&s1, 1);
            for (k = j + 1;k <= i;k++)
            {
            	VAL_MUL(U_g->data[j][k], U_inverse_g->data[k][i], s2);
            	VAL_SUB(s1, s2, s1);
            }
            c_div(&(s1), &(U_g->data[j][j]), &(U_inverse_g->data[j][i]));
        }
    }
    matc_mul(U_inverse_g, L_inverse_g, _out);
    return _out;
}

c_num matc_det(MatcP _a)
{
#if ENABLE_ASSERT
    assert(_a->rows==_a->cols);
#endif
    int N = _a->rows;
    int i, j, k;
    c_num s1, s2, _out, temp;
    
    MatcP L = matc_eye(N, N),
    U = matc_zeros(N, N);

    /* here start caculate the L and U, det(L) = 1, det(U) = det(A), A = L*U, A^-1 = U^-1*L^-1 */
    for (i = 0;i < N;i++)
    {
        for (j = i;j < N;j++)
        {
            c_set_zero(&s1, 1);
            for (k = 0;k < i;k++)
            {
                c_mul(&(L->data[i][k]), &(U->data[k][j]), &(s2));
                c_add(&(s2), &(s1), &s1);
            }
            c_sub(&(_a->data[i][j]), &(s1), &(U->data[i][j]));
        }
        
        for (j = i + 1;j < N;j++)
        {
            c_set_zero(&s1, 1);
            for (k = 0; k < i; k++)
            {
                c_mul(&(L->data[j][k]), &(U->data[k][i]), &(s2));
                c_add(&(s2), &(s1), &s1);
            }
            c_sub(&(_a->data[j][i]), &(s1), &(s2));
            c_div(&(s2), &(U->data[i][i]), &(L->data[j][i]));
        }
    }
    _out.real = 1;
    _out.imag = 0;
    for(i = 0; i < N; i++)
    {
        c_mul(&_out, &(U->data[i][i]), &temp);
        _out = temp;
    }
    free_matc(L, 1);
    free_matc(U, 1);
    return _out;
}

MatcP matc_transpose(MatcP _a, MatcP _out)
{
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->cols || _out->cols != _a->rows)
    {
        matc_reallocate(_out, 1, _a->cols, _a->rows, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[col][row] = _a->data[row][col];
        }
    }
    return _out;
}

MatcP matc_conj(MatcP _a, MatcP _out)
{
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_conj(&(_a->data[row][col]), &(_out->data[row][col]));
        }
    }
    return _out;
}

void matc_set_conj(MatcP _a)
{
    int row, col;
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _a->data[row][col].imag = -_a->data[row][col].imag;
        }
    }
}

MatcP matc_real_mul(MatcP _a, float _b, MatcP _out)
{
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for(col = 0; col < _a->cols; col++)
        {
            c_real_mul(&(_a->data[row][col]), _b, &(_out->data[row][col]));
        }
    }
    return _out;
}

MatcP matc_real_cwise_mul(MatcP _a, MatfP _b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->rows==_b->rows);
    assert(_a->cols==_b->cols);
#endif
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for(col = 0; col < _a->cols; col++)
        {
            c_real_mul(&(_a->data[row][col]), (_b->data[row][col]), &(_out->data[row][col]));
        }
    }
    return _out;
}

MatcP matc_real_row_mul(MatcP _a, MatfP _b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->rows==_b->rows);
#endif
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for(col = 0; col < _a->cols; col++)
        {
            c_real_mul(&(_a->data[row][col]), (_b->data[row][0]), &(_out->data[row][col]));
        }
    }
    return _out;
}

int __BASE_NUM_REAL__, __BASE_NUM_COMPLEX__, __MAX_SIZE_REAL__, __MAX_SIZE_COMPLEX__;
Matf *__REAL_BASE__;
Matc *__COMPLEX_BASE__;
int __MATRICE_INTI__=0;
int __MULTI_TEMP_COUNT__ = 0;
int matrice_sys_init(int max_size_real, int max_size_complex)
{
    if (__MATRICE_INTI__==0)
    {
        __BASE_NUM_REAL__           = 0;
        __BASE_NUM_COMPLEX__        = 0;
        __REAL_BASE__               = matf_zeros_n(1, 1, max_size_real);
        __COMPLEX_BASE__            = matc_zeros_n(1, 1, max_size_complex);
        __MAX_SIZE_REAL__           = max_size_real;
        __MAX_SIZE_COMPLEX__        = max_size_complex;
        __MATRICE_INTI__            = 1;
#if ENABLE_TIME
        srand((unsigned) time(NULL));
#endif
        NEW_MULTI_MAT_COMPLEX(L_g, SOURCE_NUM, SOURCE_NUM, 1);
        NEW_MULTI_MAT_COMPLEX(U_g, SOURCE_NUM, SOURCE_NUM, 1);
        NEW_MULTI_MAT_COMPLEX(L_inverse_g, SOURCE_NUM, SOURCE_NUM, 1);
        NEW_MULTI_MAT_COMPLEX(U_inverse_g, SOURCE_NUM, SOURCE_NUM, 1);
        return 0;
    }
    return 1;
}

void matrice_sys_exit(void)
{
    if (__MATRICE_INTI__==1)
    {
        free_matf(__REAL_BASE__, __MAX_SIZE_REAL__);
        free_matc(__COMPLEX_BASE__, __MAX_SIZE_COMPLEX__);
        __BASE_NUM_REAL__           = 0;
        __BASE_NUM_COMPLEX__        = 0;
        __MAX_SIZE_REAL__           = 0;
        __MAX_SIZE_COMPLEX__        = 0;
        __MATRICE_INTI__            = 0;
    }
}

void matc_set_zeros(MatcP _data)
{
    int row;
    for (row = 0; row < _data->rows; row++)
    {
        memset(_data->data[row], 0, _data->cols * sizeof(c_num));
    }
}

void matc_set_eye(MatcP _data)
{
    int i = 0;
    matc_set_zeros(_data);
    for (;i < MY_MIN(_data->cols, _data->rows); i++)
    {
        _data->data[i][i].real = 1;
    }
}

void matc_set_all_num(MatcP _data, c_num num)
{
    int row, col;
    for (row = 0; row < _data->rows; row++)
    {
        for (col = 0; col < _data->cols; col++)
        {
            _data->data[row][col] = num;
        }
    }
}

void matc_set_rand(MatcP _data, int range)
{
    int row, col;
    for (row = 0; row < _data->rows; row++)
    {
        for (col = 0; col < _data->cols; col++)
        {
            _data->data[row][col].real = rand() % range;
            _data->data[row][col].imag = rand() % range;
        }
    }
}

void matf_set_zeros(MatfP _data)
{
    int row;
    for (row = 0; row < _data->rows; row++)
    {
        memset(_data->data[row], 0, _data->cols * sizeof(float));
    }
}

void matf_set_eye(MatfP _data)
{
    int i = 0;
    matf_set_zeros(_data);
    for (;i < MY_MIN(_data->cols, _data->rows); i++)
    {
        _data->data[i][i] = 1;
    }
}

void matf_set_all_num(MatfP _data, float num)
{
    int row, col;
    for (row = 0; row < _data->rows; row++)
    {
        for (col = 0; col < _data->cols; col++)
        {
            _data->data[row][col] = num;
        }
    }
}

void matf_set_rand(MatfP _data, int range)
{
    int row, col;
    for (row = 0; row < _data->rows; row++)
    {
        for (col = 0; col < _data->cols; col++)
        {
            _data->data[row][col] = rand() % range;
        }
    }
}

void matf_set_hanning(MatfP hanning_win)
{
    int i;
    int fft_length = hanning_win->rows;
    for (i = 0; i < fft_length; i++)
    {
        hanning_win->data[i][0] = 1. / 2. * ( 1 - cosf( M_TWOPI * ( i ) / ( fft_length - 1 ) ) );
    }
}

void matf_set_hamming(MatfP hamming_win)
{
    int i;
    int fft_length = hamming_win->rows;
    for (i = 0; i < fft_length; i++)
    {
        hamming_win->data[i][0] = 0.54f - 0.46f * cosf( M_TWOPI * ( i ) / ( fft_length - 1 ) ) ;
    }
}

void matf_set_rectwin(MatfP rect_win)
{
    int i;
    int fft_length = rect_win->rows;
    for (i = 0; i < fft_length; i++)
    {
        rect_win->data[i][0] = 1 ;
    }
}

void matf_convert2com(MatfP _a, MatcP _out)
{
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col].real = _a->data[row][col];
        }
    }
}

/* drop out the imag part */
void matc_convert2real(MatcP _a, MatfP _out)
{
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            _out->data[row][col] = _a->data[row][col].real;
        }
    }
}

MatcP matc_copy(MatcP _a, MatcP _out)
{
    int row;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif)
    memcpy(_out->data[0], _a->data[0], _a->rows * _a->cols * sizeof(c_num));
    //for (row = 0; row < _a->rows; row++)
    //{
    //
    //}
    
    return _out;
}

MatcP matc_real_col_div(MatcP _a, MatfP _b, MatcP _out)
{
#if ENABLE_ASSERT
    assert(_a->cols == _b->cols);
#endif
    int row, col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    for (row = 0; row < _a->rows ; row++)
    {
        for (col = 0; col < _a->cols; col++)
        {
            c_real_div(&(_a->data[row][col]), _b->data[0][col], &(_out->data[row][col]));
        }
    }
    return _out;
}

//_out = sqrt(diag(transpose(conj(_a)) * _a))
MatfP matc_metrix(MatcP _a, MatfP _out)
{
    int row, col;
    float temp;
#if ENABLE_ASSERT
    if (_out->rows != 1 || _out->cols != _a->cols)
    {
        matf_reallocate(_out, 1, 1, _a->cols, 1);
    }
#endif
    for (col = 0; col < _a->cols; col ++)
    {
        temp = 0;
        for (row = 0; row < _a->rows; row++)
        {
            temp += (_a->data[row][col].real * _a->data[row][col].real +
                    _a->data[row][col].imag * _a->data[row][col].imag);
        }
        _out->data[0][col] = sqrtf(temp);
    }
    
    return _out;
}

MatcP matc_select_diag(MatcP _a, MatcP _out)
{
    int col;
#if ENABLE_ASSERT
    if (_out->rows != _a->rows || _out->cols != _a->cols)
    {
        matc_reallocate(_out, 1, _a->rows, _a->cols, 1);
    }
#endif
    matc_set_zeros(_out);
    for (col = 0; col < MY_MIN(_a->cols, _a->rows); col++)
    {
        _out->data[col][col] = _a->data[col][col];
    }
    return _out;
}








