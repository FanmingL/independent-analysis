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
        iva(std::string config_path_, std::string data_path_);
		~iva();
        //for every single signal, call this approach
        void process(void);
        //get the batch which has been prepared
        float** get_current_batch();
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
        int get_shift_size(void);
        float get_sample_rate(void);
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
        std::vector<Eigen::MatrixXcf> r_matrix,r_matrix_diag;
        std::vector<Eigen::MatrixXcf> norm_matrix;
        
        bool current_batch_finish_process_flag;
        bool current_batch_finish_prepare_flag;
        Eigen::MatrixXf signal_buf_prepare,signal_buf_process,signal_buf_estimate,signal_buf_estimate_overlap;
        Eigen::MatrixXf I_matrix;
        Eigen::MatrixXf zero_matrix;
        
       	Eigen::MatrixXcf phi_matrix;
        Eigen::VectorXf ksi_vector;
		void data_load(void);
        float** estimate_signal_buf;
        void parameter_read(void);
        Eigen::MatrixXf frame_shift(Eigen::MatrixXf mat);
        void algorithm_init(void);
        //template<class T>
        //friend bool read_proto_from_text_file<T>(const std::string &file_name, T *proto);
        void window_generate(void);
        Eigen::FFT<float> fft;
	};
    
    class iva_double_channel
    {
    public:
        iva_double_channel();
        iva_double_channel(std::string config_path_, std::string data_path_);
        ~iva_double_channel();
        //for every single signal, call this approach
        void process(void);
        //get the batch which has been prepared
        float** get_current_batch();
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
        int get_shift_size(void);
        float  get_sample_rate(void);
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
        std::vector<Eigen::Matrix2cf> unmix_matrix;
        std::vector<Eigen::Matrix2cf> r_matrix,r_matrix_diag;
        std::vector<Eigen::Matrix2cf> norm_matrix;
        
        bool current_batch_finish_process_flag;
        bool current_batch_finish_prepare_flag;
        Eigen::Matrix<float, Eigen::Dynamic, 2> signal_buf_prepare,signal_buf_process,signal_buf_estimate,signal_buf_estimate_overlap;
        Eigen::Matrix2f I_matrix;
        Eigen::Vector2f zero_matrix;
        
        Eigen::Matrix<std::complex<float>, Eigen::Dynamic, 2> phi_matrix;
        Eigen::VectorXf ksi_vector;
        void data_load(void);
        float** estimate_signal_buf;
        void parameter_read(void);
        Eigen::Matrix<float, Eigen::Dynamic, 2> frame_shift(Eigen::Matrix<float, Eigen::Dynamic, 2> mat);
        void algorithm_init(void);
        void window_generate(void);
        Eigen::FFT<float> fft;
    };
    template<class T>
    bool read_proto_from_text_file(const std::string &file_name, T *proto);
    
}





#endif
