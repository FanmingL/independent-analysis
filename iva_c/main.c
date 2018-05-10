/*************************************************************************
	> File Name: main.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include<stdio.h>
#include "iva.h"
#include "time.h"

#define LEN 256
#define MAX_SIZE_REAL 100
#define MAX_SIZE_COMPLEX 100


int main(int argc, char** argv)
{
    matrice_sys_init(MAX_SIZE_REAL, MAX_SIZE_COMPLEX);
    NEW_MAT_REAL(Matrix1, 5, 5);
    matf_set_rand(Matrix1, 51);
    print_matf(Matrix1, 1);
    NEW_MAT_REAL(Matrix2, 5, 5);
    matf_inverse(Matrix1, Matrix2);
    print_matf(Matrix2, 1);
    NEW_MAT_REAL(Matrix3, 5, 5);
    matf_mul(Matrix1, Matrix2, Matrix3);
    print_matf(Matrix3, 1);
    matrice_sys_exit();
	return 0;
}
