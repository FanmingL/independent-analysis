/*************************************************************************
	> File Name: iva.cpp
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/19
 ************************************************************************/
#include "iva.h"
#include <google/protobuf/message.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/text_format.h>
#include "proto/iva.pb.h"
iva::iva::iva()
{
	proto_path = PROTO_CONFIG_PATH;
    data_path  = PROTO_DATA_PATH;
    parameter_read();
    window_generate();
    signal_buf_prepare.setZero(fft_length,source_num);
    signal_buf_process.setZero(fft_length,source_num);
    buffer_size_now = 0;
    if (if_read_data)data_load();
    estimate_signal_buf = (float **) malloc(shift_size * sizeof(float * ));
    for (int i=0;i<shift_size;i++)estimate_signal_buf[i] = (float *)malloc(source_num * sizeof(float));
    for (int i=0;i<fft_length;i++)unmix_matrix.push_back(Eigen::MatrixXcf::Identity(source_num,source_num));
    r_matrix = unmix_matrix;
    norm_matrix = unmix_matrix;
    I_matrix = Eigen::MatrixXf::Identity(source_num,source_num);
}

iva::iva::~iva()
{
    if (if_read_data)
    {
        for (int i=0;i<time_points;i++)free((void *)raw_data[i]);
        free((void *)raw_data);
    }
    for (int i=0;i<shift_size;i++) free((void *)estimate_signal_buf[i]);
    free((void *)estimate_signal_buf);
}

void iva::iva::process(void)
{
    
    if (current_batch_finish_prepare_flag)
    {
        Eigen::MatrixXf signal_observed = signal_buf_process;
        Eigen::MatrixXcf signal_fft(fft_length,source_num),signal_esitimate_fft(fft_length,source_num);
        //step 1 window the signal and FFT
        for (int i=0;i<source_num;i++)
        {
            signal_observed.col(i) = signal_observed.col(i).array() * window.array();
            signal_fft.col(i)      = fft.fwd(signal_observed.col(i));
        }
        for (int i=0;i<fft_length/2+1;i++)
        {
            signal_esitimate_fft.row(i) =  (unmix_matrix[i] * signal_fft.row(i).transpose()).transpose();
//            norm_matrix[i]              =  (unmix_matrix[i].ldlt().solve(I_matrix)).diagonal().asDiagonal();
            
        }
        current_batch_finish_prepare_flag = false;
        current_batch_finish_process_flag = true;
    }
}

void iva::iva::data_prepare(std::vector<float> signal_mixed)
{
    for (int i=0;i<source_num;i++)
    {
        signal_buf_prepare(buffer_size_now,i) = signal_mixed[i];
    }
    buffer_size_now++;
    if (buffer_size_now == shift_size)
    {
        signal_buf_process = signal_buf_prepare;
        if (current_batch_finish_prepare_flag)
        {
            std::cout<<"last data will be abandoned\n";
        }
        current_batch_finish_prepare_flag = true;
        buffer_size_now = 0;
    }
}


void iva::iva::window_generate(void)
{
    window.setOnes(fft_length,1);
    if (window_type==1) //Hanning window
    {
        Eigen::VectorXf n(fft_length);
        n.setLinSpaced(fft_length, 0, (fft_length-1));
        n = n*2*M_PI/(fft_length-1);
        n = n.array().cos();
        window = 1 - 0.5f * (n.array() + 1);
    }
    else if(window_type==2) //Rectangle window
    {
        window.setOnes(fft_length,1);
    }
    else if (window_type==3) //Bartlett window
    {
        
    }
    else if(window_type==4) //Hamming window
    {
        
        
    }
}

float** iva::iva::get_current_batch(int source_index)
{
    current_batch_finish_process_flag = false;
	return estimate_signal_buf;
}

bool iva::iva::is_processed(void)
{
	return current_batch_finish_process_flag;
}


void iva::iva::parameter_read(void)
{
    config_t config;
	read_proto_from_text_file(proto_path.c_str(), &config);
	beta 		= config.beta();
	eta  		= config.eta();
	fft_length	= config.fft_length();
	shift_size 	= config.shift_size();
	window_type = config.window_type();
	is_debug 	= config.is_debug();
	source_num  = config.source_num();
    if_read_data= config.if_read_data();
    sample_rate = config.sample_rate();
}
void iva::iva::data_load(void)
{
    acoustics_t signal_read;
    read_proto_from_text_file(data_path.c_str(), &signal_read);
    time_points = signal_read.data_size() / source_num;
    raw_data = (float **) malloc(time_points * sizeof(float * ));
    
    for (int i=0;i<time_points;i++)
    {
        raw_data[i] = (float *)malloc(source_num * sizeof(float));
    }
    if (is_debug)
    {
        std::cout<<"data size is "<<signal_read.data_size()<<std::endl;
    }
    for (int i=0;i<time_points;i++)
    {
        for (int s=0;s<source_num;s++)
        {
            raw_data[i][s] = signal_read.data(i*source_num + s);
        }
    }
}
			
void iva::iva::print_parameter(void)
{
	std::cout 	<< " \tbeta:\t\t "<<beta
				<< " \n\teta:\t\t "<<eta
				<< " \n\tfft_length:\t "<<fft_length
				<< " \n\tshift_size:\t "<<shift_size
				<< " \n\twindow_type:\t "<<window_type
				<< " \n\tsources_num:\t "<<source_num
				<< " \n\tis_debug:\t "<<is_debug
				<< " \n\tconfig_path:\t"<<proto_path
                << " \n\tdata_path:\t"<<data_path
				<<std::endl;
}


template<class T>
bool iva::read_proto_from_text_file(const std::string &file_name, T *proto){
	using google::protobuf::io::FileInputStream;
	using google::protobuf::io::FileOutputStream;
	using google::protobuf::io::ZeroCopyInputStream;
	using google::protobuf::io::CodedInputStream;
	using google::protobuf::io::ZeroCopyOutputStream;
	using google::protobuf::io::CodedOutputStream;
	using google::protobuf::Message;
		    
	int fd = open(file_name.c_str(), O_RDONLY);
	if (fd == -1) {
		std::cout<<"Can't Open the file\n";
		return false;
	}
	FileInputStream *input = new FileInputStream(fd);
	bool success = google::protobuf::TextFormat::Parse(input, proto);
	delete input;
	close(fd);
	return success;
}

bool iva::iva::is_debug_mode()
{
    return is_debug;
}

std::string iva::iva::get_data_path(void)
{
    return data_path;
}


std::string iva::iva::get_config_path(void)
{
    return proto_path;
}

float** iva::iva::get_raw_data(void)
{
    
    return raw_data;
}
int iva::iva::get_source_num(void)
{
    return source_num;
}
int iva::iva::get_time_points(void)
{
    
    return time_points;
}


