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
#include <cmath>
namespace iva
{
	class iva
	{
    public:
        iva();
        ~iva();
        //for every single signal, call this approach
        void process(void);
        //get the batch which has been prepared
        float** get_current_batch(int source_index);
        //get the flag
        bool is_processed(void);
        bool is_debug_mode(void);
        void print_parameter(void);
        void data_prepare(std::vector<float> signal_mixed);
        std::string get_data_path(void);
        std::string get_config_path(void);
        float** get_raw_data(void);
        int get_source_num(void);
        int get_time_points(void);
    private:
        
        float beta;
        float eta;
        int fft_length;
        int shift_size;
        int window_type;
        int time_points;
        bool is_debug;
        int source_num;
        int buffer_size_now;
        bool if_read_data;
        float sample_rate;
        Eigen::VectorXf window;
        float **raw_data;
        
        std::string proto_path;
        std::string data_path;
        std::vector<Eigen::MatrixXcf> unmix_matrix;
        std::vector<Eigen::MatrixXcf> r_matrix;
        std::vector<Eigen::MatrixXcf> norm_matrix;
        
        bool current_batch_finish_process_flag;
        bool current_batch_finish_prepare_flag;
        Eigen::MatrixXf signal_buf_prepare,signal_buf_process;
        Eigen::MatrixXf I_matrix;
        void data_load(void);
        float** estimate_signal_buf;
        void parameter_read(void);
        //template<class T>
        //friend bool read_proto_from_text_file<T>(const std::string &file_name, T *proto);
        void window_generate(void);
        Eigen::FFT<float> fft;
	};
    template<class T>
    bool read_proto_from_text_file(const std::string &file_name, T *proto);
    
}





#endif
