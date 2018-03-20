/*************************************************************************
	> File Name: main.cpp
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/20
 ************************************************************************/

#include<iostream>
#include "iva.h"
using namespace iva;
using namespace std;

int main(int argc, char ** argv)
{
	iva_optimized iva("/Users/erdou/Documents/code/cpp/iva/demo/config/iva.prototxt");
	iva.print_parameter();
    
	return 0;
}



