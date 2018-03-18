/*************************************************************************
	> File Name: iva_parameter.h
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/19
 ************************************************************************/

#ifndef _IVA_PARAMETER_H
#define _IVA_PARAMETER_H
#include <iostream>
namespace iva
{
	typedef	struct 
	{
		//smoothing factor
		float beta;		
		//gradient desent coefficient
		float eta;					
		//FFT length
		int fft_length;
		//window shift size
		int shift_size;
		//window type
		//1(default) Hanning Window
		//2 		 Hamming Window
		//3 		 Bartlett Window
		//4 		 Rectangle Window
	}config_info_t; 
}	


#endif
