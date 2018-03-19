/*************************************************************************
	> File Name: iva.cpp
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/19
 ************************************************************************/
#include "iva.h"
iva::iva::iva()
{
	proto_path = PROTO_CONFIG_PATH;
	parameter_read();
}

void iva::iva::process(std::vector<int> signal_mixed)
{


}


std::vector<int> iva::iva::get_current_batch(int source_index)
{
	return (estimate_signal_buf[source_index]);
};

bool iva::iva::is_complete(void)
{
	return current_batch_finish_flag;
}


void iva::iva::parameter_read(void)
{
	ReadProtoFromTextFile(proto_path.c_str(), &config);
	beta 		= config.beta();
	eta  		= config.eta();
	fft_length	= config.fft_length();
	shift_size 	= config.shift_size();
	window_type = config.window_type();
	is_debug 	= config.is_debug();
}
			
void iva::iva::print_parameter(void)
{
	std::cout 	<< " \tbeta:\t\t "<<beta
				<< " \n\teta:\t\t "<<eta
				<< " \n\tfft_length:\t "<<fft_length
				<< " \n\tshift_size:\t "<<shift_size
				<< " \n\twindow_type:\t "<<window_type
				<< " \n\tis_debug:\t "<<is_debug
				<<std::endl;
}


template<class T>
bool iva::iva::ReadProtoFromTextFile(const std::string &file_name, T *proto) {
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
	
	
