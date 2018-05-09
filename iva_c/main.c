/*************************************************************************
	> File Name: main.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include<stdio.h>
#include "iva.h"
#include "time.h"
#define LEN 2
int main(int argc, char** argv)
{
    MatfP A = matf_ones(LEN, LEN),
    B = matf_ones(LEN, LEN);
    clock_t start, finish;
    for (int i = 0; i < A->rows; i++)
    {
        for (int ii = 0; ii < A->cols; ii++)
        {
            A->data[i][ii] = rand()%50;
        }
    }
    start = clock();
    for (int i = 0; i<10000; i++)
        matf_inverse(A, B);
    finish = clock();
    printf("%.2fms\n",  1000 * (float)(finish - start) / CLOCKS_PER_SEC);
    print_matf(A, 1);
    print_matf(B, 1);
    
    free_matf(A, 1);
	return 0;
}
