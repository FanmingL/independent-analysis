/*************************************************************************
	> File Name: main.c
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/05/09
 ************************************************************************/

#include <stdio.h>
#include "iva.h"
#include "time.h"

#define LEN 5
#define FFT_LEN_ 256
#define MAX_SIZE_REAL 100
#define MAX_SIZE_COMPLEX 100


int main(int argc, char** argv)
{
    matrice_sys_init(MAX_SIZE_REAL, MAX_SIZE_COMPLEX);
    
    
    matrice_sys_exit();
	return 0;
}
