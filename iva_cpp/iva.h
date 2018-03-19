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
#include <thread>
#include <mutex>
#include <string>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>

#include "proto/iva.pb.h"
namespace iva
{
	class iva
	{
		public:
			iva();
			//for every single signal, call this approach
			void process(std::vector<int> signal_mixed);
			//get the batch which has been prepared
			std::vector<int> get_current_batch(int source_index);
			//get the flag
			bool is_complete(void);
			void print_parameter(void);
		private:
			config_t config;
			float beta;
			float eta;
			int fft_length;
			int shift_size;
			int window_type;
			bool is_debug;
					
			std::string proto_path;
			bool current_batch_finish_flag;
			std::vector<Eigen::VectorXf> signal_buf;
			std::vector<std::vector<int> > estimate_signal_buf;
			void parameter_read(void);
			template<class T>
			bool ReadProtoFromTextFile(const std::string &file_name, T *proto);
	
	};

}





#endif
