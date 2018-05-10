/*************************************************************************
	> File Name: main.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include<stdio.h>
#include "iva.h"
#include "time.h"
#define LEN 4
int main(int argc, char** argv)
{
    MatcP A = matc_ones(LEN, LEN),
    L = matc_ones(LEN, LEN),
    U = matc_ones(LEN, LEN),
    A2 = matc_ones(LEN, LEN),
    A3 = matc_ones(LEN, LEN);
    c_num t;
    clock_t start, finish;
    for (int i = 0; i<A->rows; i++)
    {
        for (int j = 0; j<A->cols; j++)
        {
            A->data[i][j].real = rand()%10;
            A->data[i][j].imag = rand()%10;
        }
    }
    start = clock();
    for (int i = 0; i< 10000; i++)
    {
        matc_inverse(A, A2);
        t = matc_det(A);
    }
    finish = clock();
    printf("A:\n"); 
    print_matc(A, 1);
	printf("A^-1\n");
    print_matc(A2, 1);
	printf("A * A^-1\n");
    print_matc(matc_mul(A2, A, A3), 1);
    printf("%.2fms\n",  1000 * (float)(finish - start) / CLOCKS_PER_SEC);
    c_println(&t);
    free_matc(A, 1);
    free_matc(L, 1);
    free_matc(U, 1);
    free_matc(A2, 1);
    free_matc(A3, 1);
    
	return 0;
}
