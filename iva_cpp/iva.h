/*************************************************************************
	> File Name: iva.h
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/19
 ************************************************************************/

#ifndef _IVA_H
#define _IVA_H
#include "Eigen/Dense"
#include "unsupported/Eigen/FFT"
#include <iostream>
#include <vector>
#include "iva_parameter.h"
namespace iva
{  
	class iva
	{
		public:
			iva();
			//for every single signal, call this approach
			void process(std::vector<int> signal_mixed);
			std::vector<int> get_current_batch(int source_index);
			bool is_complete(void);
		private:
			config_info_t config;
			
			
			bool current_batch_finish_flag;
			std::vector<Eigen::VectorXf> signal_buf;
			std::vector<std::vector<int> > estimate_signal_buf;
			void parameter_read(void);
	};

}





#endif
