/*************************************************************************
	> File Name: iva.cpp
	> Author: LuoFanming 
	> Mail: 151190065@smail.nju.edu.cn
	> Created Time: 2018/03/19
 ************************************************************************/
#include "iva.h"
iva::iva::iva()
{
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


}

